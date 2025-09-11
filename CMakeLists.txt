cmake_minimum_required(VERSION 3.16)

project(ShogiGame VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Set Qt6 properties
qt6_standard_project_setup()

# Include directories
include_directories(src)

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Core library - Board logic
add_library(ShogiCore STATIC
    src/core/board/board.cpp
    src/core/board/board.h
    src/core/board/piece.cpp
    src/core/board/piece.h
    src/core/board/move.cpp
    src/core/board/move.h
    src/core/komaochi/komaochi.cpp
    src/core/komaochi/komaochi.h
)

target_link_libraries(ShogiCore Qt6::Core)

# Engine library - USI integration
add_library(ShogiEngine STATIC
    src/engine/usi/usi_engine.cpp
    src/engine/usi/usi_engine.h
)

target_link_libraries(ShogiEngine Qt6::Core ShogiCore)

# AI library - Built-in AI
add_library(ShogiAI STATIC
    src/ai/basic/basic_ai.cpp
    src/ai/basic/basic_ai.h
)

target_link_libraries(ShogiAI Qt6::Core ShogiCore)

# Network library - TCP communication
add_library(ShogiNet STATIC
    src/net/basic/network_manager.cpp
    src/net/basic/network_manager.h
)

target_link_libraries(ShogiNet Qt6::Core Qt6::Network)

# UI library - Qt widgets
add_library(ShogiUI STATIC
    src/ui/board/board_widget.cpp
    src/ui/board/board_widget.h
    src/ui/board/main_window.cpp
    src/ui/board/main_window.h
)

target_link_libraries(ShogiUI Qt6::Core Qt6::Widgets ShogiCore)

# Main executable
add_executable(ShogiGame
    src/main.cpp
)

target_link_libraries(ShogiGame 
    ShogiCore 
    ShogiEngine 
    ShogiAI 
    ShogiNet 
    ShogiUI
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network
)

# Enable testing
enable_testing()

# Add test subdirectory
add_subdirectory(tests)

# Install target
install(TARGETS ShogiGame DESTINATION bin)