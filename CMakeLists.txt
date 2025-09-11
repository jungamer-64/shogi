cmake_minimum_required(VERSION 3.25)
project(ShogiGame VERSION 1.0.0 LANGUAGES CXX)

# Set C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(wxWidgets REQUIRED COMPONENTS core base)
find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(LIBRSVG librsvg-2.0)
endif()

# Include wxWidgets
include(${wxWidgets_USE_FILE})

# Set compiler flags for better error reporting and optimization
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/app.cpp
    src/main_frame.cpp
    src/board_panel.cpp
    src/game_logic.cpp
    src/piece.cpp
    src/board_state.cpp
    src/svg_renderer.cpp
)

set(HEADERS
    include/app.h
    include/main_frame.h
    include/board_panel.h
    include/game_logic.h
    include/piece.h
    include/board_state.h
    include/svg_renderer.h
    include/common.h
)

# Include directories
include_directories(include)

# Create executable
add_executable(shogi ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(shogi ${wxWidgets_LIBRARIES})

if(LIBRSVG_FOUND)
    target_include_directories(shogi PRIVATE ${LIBRSVG_INCLUDE_DIRS})
    target_link_libraries(shogi ${LIBRSVG_LIBRARIES})
    target_compile_definitions(shogi PRIVATE HAVE_LIBRSVG)
endif()

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Install target
install(TARGETS shogi DESTINATION bin)
install(DIRECTORY assets DESTINATION share/shogi)