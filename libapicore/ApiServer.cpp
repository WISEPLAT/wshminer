#include "ApiServer.h"

#include <wshminer-buildinfo.h>

ApiServer::ApiServer(AbstractServerConnector *conn, serverVersion_t type, Farm &farm, bool &readonly) : AbstractServer(*conn, type), m_farm(farm)
{
	this->bindAndAddMethod(Procedure("miner_getstat1", PARAMS_BY_NAME, JSON_OBJECT, NULL), &ApiServer::getMinerStat1);
	this->bindAndAddMethod(Procedure("miner_getstathr", PARAMS_BY_NAME, JSON_OBJECT, NULL), &ApiServer::getMinerStatHR);	
	if (!readonly) {
		this->bindAndAddMethod(Procedure("miner_restart", PARAMS_BY_NAME, JSON_OBJECT, NULL), &ApiServer::doMinerRestart);
		this->bindAndAddMethod(Procedure("miner_reboot", PARAMS_BY_NAME, JSON_OBJECT, NULL), &ApiServer::doMinerReboot);
	}
}

void ApiServer::getMinerStat1(const Json::Value& request, Json::Value& response)
{
	(void) request; // unused
	
	auto runningTime = std::chrono::duration_cast<std::chrono::minutes>(steady_clock::now() - this->m_farm.farmLaunched());
	
	SolutionStats s = m_farm.getSolutionStats();
	WorkingProgress p = m_farm.miningProgress(true);
	
	ostringstream totalMhWsh; 
	ostringstream totalMhDcr; 
	ostringstream detailedMhWsh;
	ostringstream detailedMhDcr;
	ostringstream tempAndFans;
	ostringstream poolAddresses;
	ostringstream invalidStats;
	
	totalMhWsh << std::fixed << std::setprecision(0) << (p.rate() / 1000.0f) << ";" << s.getAccepts() << ";" << s.getRejects();
	totalMhDcr << "0;0;0"; // DualMining not supported
	invalidStats << s.getFailures() << ";0"; // Invalid + Pool switches
    poolAddresses << m_farm.get_pool_addresses(); 
	invalidStats << ";0;0"; // DualMining not supported
	
	int gpuIndex = 0;
	int numGpus = p.minersHashes.size();
	for (auto const& i: p.minersHashes)
	{
		detailedMhWsh << std::fixed << std::setprecision(0) << (p.minerRate(i) / 1000.0f) << (((numGpus -1) > gpuIndex) ? ";" : "");
		detailedMhDcr << "off" << (((numGpus -1) > gpuIndex) ? ";" : ""); // DualMining not supported
		gpuIndex++;
	}

	gpuIndex = 0;
	numGpus = p.minerMonitors.size();
	for (auto const& i : p.minerMonitors)
	{
		tempAndFans << i.tempC << ";" << i.fanP << (((numGpus - 1) > gpuIndex) ? "; " : ""); // Fetching Temp and Fans
		gpuIndex++;
	}

	response[0] = wshminer_get_buildinfo()->project_version;  //miner version.
	response[1] = toString(runningTime.count()); // running time, in minutes.
	response[2] = totalMhWsh.str();              // total WSH hashrate in MH/s, number of WSH shares, number of WSH rejected shares.
	response[3] = detailedMhWsh.str();           // detailed WSH hashrate for all GPUs.
	response[4] = totalMhDcr.str();              // total DCR hashrate in MH/s, number of DCR shares, number of DCR rejected shares.
	response[5] = detailedMhDcr.str();           // detailed DCR hashrate for all GPUs.
	response[6] = tempAndFans.str();             // Temperature and Fan speed(%) pairs for all GPUs.
	response[7] = poolAddresses.str();           // current mining pool. For dual mode, there will be two pools here.
	response[8] = invalidStats.str();            // number of WSH invalid shares, number of WSH pool switches, number of DCR invalid shares, number of DCR pool switches.
}

void ApiServer::getMinerStatHR(const Json::Value& request, Json::Value& response)
{
	(void) request; // unused
	
	//TODO:give key-value format
	auto runningTime = std::chrono::duration_cast<std::chrono::minutes>(steady_clock::now() - this->m_farm.farmLaunched());
	
	SolutionStats s = m_farm.getSolutionStats();
	WorkingProgress p = m_farm.miningProgress(true,true);
	
	ostringstream version; 
	ostringstream runtime; 
	Json::Value detailedMhWsh;
	Json::Value detailedMhDcr;
	Json::Value temps;
	Json::Value fans;
	Json::Value powers;
	ostringstream poolAddresses;
	
	version << wshminer_get_buildinfo()->project_version;
	runtime << toString(runningTime.count());
    poolAddresses << m_farm.get_pool_addresses(); 
	
	int gpuIndex = 0;
	for (auto const& i: p.minersHashes)
	{
		detailedMhWsh[gpuIndex] = (p.minerRate(i));
		//detailedMhDcr[gpuIndex] = "off"; //Not supported
		gpuIndex++;
	}

	gpuIndex = 0;
	for (auto const& i : p.minerMonitors)
	{
		temps[gpuIndex] = i.tempC ; // Fetching Temps 
		fans[gpuIndex] = i.fanP; // Fetching Fans
		powers[gpuIndex] =  i.powerW; // Fetching Power
		gpuIndex++;
	}

	response["version"] = version.str();		// miner version.
	response["runtime"] = runtime.str();		// running time, in minutes.
	// total WSH hashrate in MH/s, number of WSH shares, number of WSH rejected shares.
	response["wshhashrate"] = (p.rate());
	response["wshhashrates"] = detailedMhWsh;  
	response["wshshares"] 	= s.getAccepts(); 
	response["wshrejected"] = s.getRejects();   
	response["wshinvalid"] 	= s.getFailures(); 
	response["wshpoolsw"] 	= 0;             
	// Hardware Info
	response["temperatures"] = temps;             		// Temperatures(C) for all GPUs
	response["fanpercentages"] = fans;             		// Fans speed(%) for all GPUs
	response["powerusages"] = powers;         			// Power Usages(W) for all GPUs
	response["pooladdrs"] = poolAddresses.str();        // current mining pool. For dual mode, there will be two pools here.
}

void ApiServer::doMinerRestart(const Json::Value& request, Json::Value& response)
{
	(void) request; // unused
	(void) response; // unused
	
	this->m_farm.restart();
}

void ApiServer::doMinerReboot(const Json::Value& request, Json::Value& response)
{
	(void) request; // unused
	(void) response; // unused
	
	// Not supported
}
