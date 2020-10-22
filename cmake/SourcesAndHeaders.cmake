file(GLOB_RECURSE sources
	CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp"
)

file(GLOB_RECURSE headers
	CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.hpp"
)

set(exe_sources
    src/main.cpp
    ${sources}
)

file(GLOB_RECURSE public_headers
	CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)
