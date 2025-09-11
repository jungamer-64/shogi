cmake_minimum_required(VERSION 3.16)

# Find Qt6 Test module
find_package(Qt6 REQUIRED COMPONENTS Test)

# Include test directories
include_directories(../src)

# Test for core board functionality
add_executable(test_board
    core/test_board.cpp
)

target_link_libraries(test_board 
    ShogiCore
    Qt6::Test
    Qt6::Core
)

# Test for piece functionality
add_executable(test_piece
    core/test_piece.cpp
)

target_link_libraries(test_piece
    ShogiCore
    Qt6::Test
    Qt6::Core
)

# Test for komaochi functionality
add_executable(test_komaochi
    core/test_komaochi.cpp
)

target_link_libraries(test_komaochi
    ShogiCore
    Qt6::Test
    Qt6::Core
)

# Add tests to CTest
add_test(NAME BoardTest COMMAND test_board)
add_test(NAME PieceTest COMMAND test_piece)
add_test(NAME KomaochiTest COMMAND test_komaochi)

# Test runner that runs all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS test_board test_piece test_komaochi
)