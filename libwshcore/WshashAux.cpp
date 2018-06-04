/*
	This file is part of cpp-wiseplat.

	cpp-wiseplat is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-wiseplat is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-wiseplat.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file WshashAux.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include "WshashAux.h"
#include <libwshash/internal.h>

using namespace std;
using namespace chrono;
using namespace dev;
using namespace wsh;

WshashAux& WshashAux::get()
{
	static WshashAux instance;
	return instance;
}

h256 WshashAux::seedHash(unsigned _number)
{
	unsigned epoch = _number / WSHASH_EPOCH_LENGTH;
	WshashAux& wshash = WshashAux::get();
	Guard l(wshash.x_epochs);
	if (epoch >= wshash.m_seedHashes.size())
	{
		h256 ret;
		unsigned n = 0;
		if (!wshash.m_seedHashes.empty())
		{
			ret = wshash.m_seedHashes.back();
			n = wshash.m_seedHashes.size() - 1;
		}
		wshash.m_seedHashes.resize(epoch + 1);
		for (; n <= epoch; ++n, ret = sha3(ret))
			wshash.m_seedHashes[n] = ret;
	}
	return wshash.m_seedHashes[epoch];
}

uint64_t WshashAux::number(h256 const& _seedHash)
{
	WshashAux& wshash = WshashAux::get();
	Guard l(wshash.x_epochs);
	unsigned epoch = 0;
	auto epochIter = wshash.m_epochs.find(_seedHash);
	if (epochIter == wshash.m_epochs.end())
	{
		for (h256 h; h != _seedHash && epoch < 2048; ++epoch, h = sha3(h), wshash.m_epochs[h] = epoch) {}
		if (epoch == 2048)
		{
			std::ostringstream error;
			error << "apparent block number for " << _seedHash << " is too high; max is " << (WSHASH_EPOCH_LENGTH * 2048);
			throw std::invalid_argument(error.str());
		}
	}
	else
		epoch = epochIter->second;
	return epoch * WSHASH_EPOCH_LENGTH;
}

WshashAux::LightType WshashAux::light(h256 const& _seedHash)
{
	// TODO: Use epoch number instead of seed hash?

	WshashAux& wshash = WshashAux::get();
	Guard l(wshash.x_lights);
	if (wshash.m_lights.count(_seedHash))
		return wshash.m_lights.at(_seedHash);
	return (wshash.m_lights[_seedHash] = make_shared<LightAllocation>(_seedHash));
}

WshashAux::LightAllocation::LightAllocation(h256 const& _seedHash)
{
	uint64_t blockNumber = WshashAux::number(_seedHash);
	light = wshash_light_new(blockNumber);
	if (!light)
		BOOST_THROW_EXCEPTION(ExternalFunctionFailure("wshash_light_new()"));
	size = wshash_get_cachesize(blockNumber);
}

WshashAux::LightAllocation::~LightAllocation()
{
	wshash_light_delete(light);
}

bytesConstRef WshashAux::LightAllocation::data() const
{
	return bytesConstRef((byte const*)light->cache, size);
}

Result WshashAux::LightAllocation::compute(h256 const& _headerHash, uint64_t _nonce) const
{
	wshash_return_value r = wshash_light_compute(light, *(wshash_h256_t*)_headerHash.data(), _nonce);
	if (!r.success)
		BOOST_THROW_EXCEPTION(DAGCreationFailure());
	return Result{h256((uint8_t*)&r.result, h256::ConstructFromPointer), h256((uint8_t*)&r.mix_hash, h256::ConstructFromPointer)};
}

Result WshashAux::eval(h256 const& _seedHash, h256 const& _headerHash, uint64_t _nonce) noexcept
{
	try
	{
		return get().light(_seedHash)->compute(_headerHash, _nonce);
	}
	catch(...)
	{
		return Result{~h256(), h256()};
	}
}
