#pragma once

#include "ApiServer.h"
#include <libwshcore/Farm.h>
#include <libwshcore/Miner.h>
#include <jsonrpccpp/server/connectors/tcpsocketserver.h>

using namespace jsonrpc;
using namespace dev;
using namespace dev::wsh;

class Api
{
public:
	Api(const int &port, Farm &farm);
private:
	ApiServer *m_server;
	Farm &m_farm;
};

