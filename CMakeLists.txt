cmake_minimum_required(VERSION 3.3)

include(cmake/cable/bootstrap.cmake)

include(CableBuildInfo)
include(CableBuildType)
include(CableToolchains)

cable_configure_toolchain(DEFAULT cxx11)

set(HUNTER_CONFIGURATION_TYPES Release)
set(HUNTER_CACHE_SERVERS "https://github.com/wiseplat/hunter-cache")
include(HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.213.tar.gz"
    SHA1 "59b2b880178bdc1e9cce83f279f1d88feefa07a2"
    LOCAL
)

if(HUNTER_ENABLED)
	find_package(PythonInterp 3)
	if(PYTHONINTERP_FOUND)
		hunter_gate_self(
			"${HUNTER_CACHED_ROOT}"
			"${HUNTER_VERSION}"
			"${HUNTER_SHA1}"
			hunter_dir
		)
		set(hunter_upload_script "${hunter_dir}/maintenance/upload-cache-to-github.py")
		set(hunter_cache_dir "${HUNTER_GATE_ROOT}/_Base/Cache")
		set(hunter_tmp_dir "${HUNTER_GATE_ROOT}/tmp")
		add_custom_target(
			hunter_upload_cache
			${PYTHON_EXECUTABLE} ${hunter_upload_script}
			--username hunter-cache-bot
			--repo-owner wiseplat
			--repo hunter-cache
			--cache-dir ${hunter_cache_dir}
			--temp-dir ${hunter_tmp_dir}
		)
	endif()
endif()


project(wshminer)
set(PROJECT_VERSION 0.15.0.dev0)

cable_set_build_type(DEFAULT Release CONFIGURATION_TYPES Release RelWithDebInfo Debug)

# link_directories interprate relative paths with respect to CMAKE_CURRENT_SOURCE_DIR
cmake_policy(SET CMP0015 NEW)

# let cmake autolink dependencies on windows
# it's specified globally, cause qt libraries requires that on windows and they are also found globally
cmake_policy(SET CMP0020 NEW)

# 3.1 and above
cmake_policy(SET CMP0054 NEW)

######################################################################################################

option(WSHASHCL "Build with OpenCL mining" ON)
option(WSHASHCUDA "Build with CUDA mining" OFF)
option(WSHDBUS "Build with D-Bus support" OFF)
option(APICORE "Build with API Server support" ON)

# propagates CMake configuration options to the compiler
function(configureProject)
	if (WSHASHCL)
		add_definitions(-DWSH_WSHASHCL)
	endif()
	if (WSHASHCUDA)
		add_definitions(-DWSH_WSHASHCUDA)
	endif()
	if (WSHDBUS)
		add_definitions(-DWSH_DBUS)
	endif()
	if (APICORE)
		add_definitions(-DAPI_CORE)
	endif()
endfunction()

hunter_add_package(Boost COMPONENTS system)
find_package(Boost CONFIG REQUIRED COMPONENTS system)

hunter_add_package(jsoncpp)
find_package(jsoncpp CONFIG REQUIRED)

hunter_add_package(libjson-rpc-cpp)
find_package(libjson-rpc-cpp CONFIG REQUIRED)

hunter_add_package(CppNetlibUri)
find_package(CppNetlibUri CONFIG REQUIRED)

configureProject()

message("------------------------------------------------------------------------")
message("-- CMake ${CMAKE_VERSION}")
message("-- Build ${CMAKE_BUILD_TYPE} / ${CMAKE_SYSTEM_NAME}")
message("------------------------------------------------------------- components")
message("-- WSHASHCL         Build OpenCL components                  ${WSHASHCL}")
message("-- WSHASHCUDA       Build CUDA components                    ${WSHASHCUDA}")
message("-- WSHDBUS          Build D-Bus components                   ${WSHDBUS}")
message("-- APICORE          Build API Server components              ${APICORE}")
message("------------------------------------------------------------------------")
message("")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(WshCompilerSettings)

cable_add_buildinfo_library(PREFIX wshminer)

add_subdirectory(libdevcore)
add_subdirectory(libwshash)
add_subdirectory(libwshcore)
add_subdirectory(libhwmon)
add_subdirectory(libpoolprotocols)

if (WSHASHCL)
	add_subdirectory(libwshash-cl)
endif ()
if (WSHASHCUDA)
	add_subdirectory(libwshash-cuda)
endif ()
if (APICORE)
	add_subdirectory(libapicore)
endif()

add_subdirectory(wshminer)


if(WIN32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_GENERATOR TGZ)
endif()
set(CPACK_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME})
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY FALSE)
include(CPack)


# Export some variables for CIs
if(PROJECT_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
	set(PROJECT_VERSION_IS_PRERELEASE false)
else()
	set(PROJECT_VERSION_IS_PRERELEASE true)
endif()
configure_file(cmake/variables.sh.in variables.sh)
configure_file(cmake/variables.ps1.in variables.ps1)
