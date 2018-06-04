set(SOURCES
	PoolURI.cpp PoolURI.h
	PoolClient.h
	PoolManager.h PoolManager.cpp
	testing/SimulateClient.h testing/SimulateClient.cpp
	stratum/WshStratumClient.h stratum/WshStratumClient.cpp
	getwork/WshGetworkClient.h getwork/WshGetworkClient.cpp getwork/jsonrpc_getwork.h
)

hunter_add_package(OpenSSL)
find_package(OpenSSL REQUIRED)

add_library(poolprotocols ${SOURCES})
target_link_libraries(poolprotocols PRIVATE devcore wshminer-buildinfo libjson-rpc-cpp::client Boost::system jsoncpp_lib_static OpenSSL::SSL OpenSSL::Crypto network-uri)
target_include_directories(poolprotocols PRIVATE ..)
