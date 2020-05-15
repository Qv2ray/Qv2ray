find_package(Qt5Test REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
enable_testing(true)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
function(ADD_QV2RAY_TEST TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_include_directories(${TEST_NAME} 
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    )
    target_link_libraries(
            ${TEST_NAME}
            PRIVATE
            $<$<TARGET_EXISTS:${PROJECT_NAME}-lib>:${PROJECT_NAME}-lib>
            Qt5::Test
    )

add_test(NAME QV2RAY_TEST_${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
endfunction()

ADD_QV2RAY_TEST(parse_ss_url src/core/connection/TestParseSS.cpp)
