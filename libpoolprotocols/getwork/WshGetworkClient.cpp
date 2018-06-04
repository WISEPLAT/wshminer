#include "WshGetworkClient.h"
#include <chrono>

using namespace std;
using namespace dev;
using namespace wsh;

WshGetworkClient::WshGetworkClient(unsigned const & farmRecheckPeriod) : PoolClient(), Worker("getwork")
{
	m_farmRecheckPeriod = farmRecheckPeriod;
	m_authorized = true;
	m_connection_changed = true;
	m_solutionToSubmit.nonce = 0;
	startWorking();
}

WshGetworkClient::~WshGetworkClient()
{
	p_client = nullptr;
}

void WshGetworkClient::connect()
{
	if (m_connection_changed) {
		stringstream ss;
		ss <<  "http://" + m_conn.Host() << ':' << m_conn.Port();
		p_client = new ::JsonrpcGetwork(new jsonrpc::HttpClient(ss.str()));
	}

//	cnote << "connect to " << m_host;

	m_client_id = h256::random();
	m_connection_changed = false;
	m_justConnected = true; // We set a fake flag, that we can check with workhandler if connection works
}

void WshGetworkClient::disconnect()
{
	m_connected = false;
	m_justConnected = false;

	// Since we do not have a real connected state with getwork, we just fake it.
	if (m_onDisconnected) {
		m_onDisconnected();
	}
}

void WshGetworkClient::submitHashrate(string const & rate)
{
	// Store the rate in temp var. Will be handled in workLoop
	m_currentHashrateToSubmit = rate;
}

void WshGetworkClient::submitSolution(Solution solution)
{
	// Store the solution in temp var. Will be handled in workLoop
	m_solutionToSubmit = solution;
}

// Handles all getwork communication.
void WshGetworkClient::workLoop()
{
	while (true)
	{
		if (m_connected || m_justConnected) {

			// Submit solution
			if (m_solutionToSubmit.nonce) {
				try
				{
					bool accepted = p_client->wsh_submitWork("0x" + toHex(m_solutionToSubmit.nonce), "0x" + toString(m_solutionToSubmit.work.header), "0x" + toString(m_solutionToSubmit.mixHash));
					if (accepted) {
						if (m_onSolutionAccepted) {
							m_onSolutionAccepted(false);
						}
					}
					else {
						if (m_onSolutionRejected) {
							m_onSolutionRejected(false);
						}
					}

					m_solutionToSubmit.nonce = 0;
				}
				catch (jsonrpc::JsonRpcException const& _e)
				{
					cwarn << "Failed to submit solution.";
					cwarn << boost::diagnostic_information(_e);
				}
			}

			// Get Work
			try
			{
				Json::Value v = p_client->wsh_getWork();
				WorkPackage newWorkPackage;
				newWorkPackage.header = h256(v[0].asString());
				newWorkPackage.seed = h256(v[1].asString());

				// Since we do not have a real connected state with getwork, we just fake it.
				// If getting work succeeds we know that the connection works
				if (m_justConnected && m_onConnected) {
					m_justConnected = false;
					m_connected = true;
					m_onConnected();
				}

				// Check if header changes so the new workpackage is really new
				if (newWorkPackage.header != m_prevWorkPackage.header) {
					m_prevWorkPackage.header = newWorkPackage.header;
					m_prevWorkPackage.seed = newWorkPackage.seed;
					m_prevWorkPackage.boundary = h256(fromHex(v[2].asString()), h256::AlignRight);

					if (m_onWorkReceived) {
						m_onWorkReceived(m_prevWorkPackage);
					}
				}
			}
			catch (jsonrpc::JsonRpcException)
			{
				cwarn << "Failed getting work!";
				disconnect();
			}

			// Submit current hashrate if needed
			if (!m_currentHashrateToSubmit.empty()) {
				try
				{
					p_client->wsh_submitHashrate(m_currentHashrateToSubmit, "0x" + m_client_id.hex());
				}
				catch (jsonrpc::JsonRpcException)
				{
					//cwarn << "Failed to submit hashrate.";
					//cwarn << boost::diagnostic_information(_e);
				}
				m_currentHashrateToSubmit = "";
			}
		}

		// Sleep
		this_thread::sleep_for(chrono::milliseconds(m_farmRecheckPeriod));
	}
}
