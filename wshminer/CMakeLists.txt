cmake_policy(SET CMP0015 NEW)

aux_source_directory(. SRC_LIST)

include_directories(BEFORE ..)

set(EXECUTABLE wshminer)

file(GLOB HEADERS "*.h")

add_executable(${EXECUTABLE} ${SRC_LIST} ${HEADERS})

target_link_libraries(wshminer PRIVATE wshcore wshash poolprotocols devcore wshminer-buildinfo)

if(WSHDBUS)
	find_package(PkgConfig)
	set( ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/x86_64-linux-gnu/pkgconfig" )
	pkg_check_modules(DBUS dbus-1)
	include_directories(${DBUS_INCLUDE_DIRS})
	link_directories(${DBUS_LIBRARY_DIRS})
	target_link_libraries(${EXECUTABLE} ${DBUS_LIBRARIES})
endif()

if(APICORE)
	target_link_libraries(wshminer PRIVATE apicore)
endif()

include(GNUInstallDirs)
install(TARGETS wshminer DESTINATION ${CMAKE_INSTALL_BINDIR})
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:wshminer> DESTINATION ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()
