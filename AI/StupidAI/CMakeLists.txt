set(stupidAI_SRCS
		StdInc.cpp

		main.cpp
		StupidAI.cpp
)

set(stupidAI_HEADERS
		StdInc.h

		StupidAI.h
)

assign_source_group(${stupidAI_SRCS} ${stupidAI_HEADERS})

if(EMSCRIPTEN)
	add_library(StupidAI STATIC ${stupidAI_SRCS} ${stupidAI_HEADERS})
	return()
endif()

add_library(StupidAI SHARED ${stupidAI_SRCS} ${stupidAI_HEADERS})
target_link_libraries(StupidAI PRIVATE vcmi)
target_include_directories(StupidAI PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

vcmi_set_output_dir(StupidAI "AI")

set_target_properties(StupidAI PROPERTIES ${PCH_PROPERTIES})
cotire(StupidAI)

install(TARGETS StupidAI RUNTIME DESTINATION ${AI_LIB_DIR} LIBRARY DESTINATION ${AI_LIB_DIR})
