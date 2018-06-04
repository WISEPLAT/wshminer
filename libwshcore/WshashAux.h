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

#pragma once

#include <condition_variable>
#include <libwshash/wshash.h>
#include <libdevcore/Log.h>
#include <libdevcore/Worker.h>
#include "BlockHeader.h"

namespace dev
{
namespace wsh
{

struct Result
{
	h256 value;
	h256 mixHash;
};

class WshashAux
{
public:
	struct LightAllocation
	{
		LightAllocation(h256 const& _seedHash);
		~LightAllocation();
		bytesConstRef data() const;
		Result compute(h256 const& _headerHash, uint64_t _nonce) const;
		wshash_light_t light;
		uint64_t size;
	};

	using LightType = std::shared_ptr<LightAllocation>;

	static h256 seedHash(unsigned _number);
	static uint64_t number(h256 const& _seedHash);

	static LightType light(h256 const& _seedHash);

	static Result eval(h256 const& _seedHash, h256 const& _headerHash, uint64_t  _nonce) noexcept;

private:
	WshashAux() = default;
	static WshashAux& get();

	Mutex x_lights;
	std::unordered_map<h256, LightType> m_lights;

	Mutex x_epochs;
	std::unordered_map<h256, unsigned> m_epochs;
	h256s m_seedHashes;
};

struct WorkPackage
{
	WorkPackage() = default;
	explicit WorkPackage(BlockHeader const& _bh) :
		boundary(_bh.boundary()),
		header(_bh.hashWithout()),
		seed(WshashAux::seedHash(static_cast<unsigned>(_bh.number())))
	{ }
	void reset() { header = h256(); }
	explicit operator bool() const { return header != h256(); }

	h256 boundary;
	h256 header;	///< When h256() means "pause until notified a new work package is available".
	h256 seed;
	h256 job;

	uint64_t startNonce = 0;
	int exSizeBits = -1;
	int job_len = 8;
};

struct Solution
{
	uint64_t nonce;
	h256 mixHash;
	WorkPackage work;
	bool stale;
};

}
}
