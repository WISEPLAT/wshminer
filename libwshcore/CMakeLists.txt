set(SOURCES
	BlockHeader.h BlockHeader.cpp
	WshashAux.h WshashAux.cpp
	Exceptions.h
	Farm.h
	Miner.h Miner.cpp
)

include_directories(BEFORE ..)

add_library(wshcore ${SOURCES})
target_link_libraries(wshcore wshash devcore hwmon)

if(WSHASHCL)
	target_link_libraries(wshcore wshash-cl)
endif()
if(WSHASHCUDA)
	target_link_libraries(wshcore wshash-cuda)
endif()
