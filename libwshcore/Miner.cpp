#include "Miner.h"
#include "WshashAux.h"

using namespace dev;
using namespace wsh;

unsigned dev::wsh::Miner::s_dagLoadMode = 0;

unsigned dev::wsh::Miner::s_dagLoadIndex = 0;

unsigned dev::wsh::Miner::s_dagCreateDevice = 0;

uint8_t* dev::wsh::Miner::s_dagInHostMemory = NULL;

bool dev::wsh::Miner::s_exit = false;

