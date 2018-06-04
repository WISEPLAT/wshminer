# Set necessary compile and link flags

include(WshCheckCXXFlags)

# C++11 check and activation
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wextra -Wno-error=parentheses -pedantic")

    wsh_add_cxx_compiler_flag_if_supported(-ffunction-sections)
    wsh_add_cxx_compiler_flag_if_supported(-fdata-sections)
    wsh_add_cxx_linker_flag_if_supported(-Wl,--gc-sections)

elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wextra")

    wsh_add_cxx_compiler_flag_if_supported(-ffunction-sections)
    wsh_add_cxx_compiler_flag_if_supported(-fdata-sections)
    wsh_add_cxx_linker_flag_if_supported(-Wl,--gc-sections)

	if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -fcolor-diagnostics -Qunused-arguments")
	endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

	# enable parallel compilation
	# specify Exception Handling Model in msvc
	# enable LTCG for faster builds
	# disable unknown pragma warning (4068)
	# disable unsafe function warning (4996)
	# disable decorated name length exceeded, name was truncated (4503)
	# disable conversion from 'size_t' to 'type', possible loss of data (4267)
	# disable qualifier applied to function type has no meaning; ignored (4180)
	# disable C++ exception specification ignored except to indicate a function is not __declspec(nothrow) (4290)
	# disable conversion from 'type1' to 'type2', possible loss of data (4244)
	# disable forcing value to bool 'true' or 'false' (performance warning) (4800)
	# declare Windows XP requirement
	# undefine windows.h MAX && MIN macros cause it cause conflicts with std::min && std::max functions
	# define miniupnp static library
	add_compile_options(/MP /EHsc /GL /wd4068 /wd4996 /wd4503 /wd4267 /wd4180 /wd4290 /wd4244 /wd4800 -D_WIN32_WINNT=0x0501 /DNOMINMAX)
	# disable empty object file warning
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /LTCG /ignore:4221")
	# enable LTCG for faster builds
	# enable RELEASE so that the executable file has its checksum set
	# enable unused references removal
	# warning LNK4075: ignoring '/EDITANDCONTINUE' due to '/SAFESEH' specification
	# warning LNK4099: pdb was not found with lib
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /RELEASE /OPT:REF /OPT:ICF /ignore:4099,4075")
else ()
	message(WARNING "Your compiler is not tested, if you run into any issues, we'd welcome any patches.")
endif ()

set(SANITIZE NO CACHE STRING "Instrument build with provided sanitizer")
if(SANITIZE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=${SANITIZE}")
endif()

