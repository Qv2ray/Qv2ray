function(ADD_QV2RAY_TEST TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} catch.hpp)
    target_include_directories(${TEST_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )
    target_link_libraries(
        ${TEST_NAME}
        PRIVATE
        $<$<TARGET_EXISTS:qv2ray_baselib>:qv2ray_baselib>
        )

    add_test(NAME QV2RAY_TEST_${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
endfunction()

ADD_QV2RAY_TEST(parse_ss_url src/core/connection/TestParseSS.cpp)
ADD_QV2RAY_TEST(parse_vmess_url src/core/connection/TestParseVmess.cpp)
ADD_QV2RAY_TEST(parse_vless_url src/core/connection/TestParseVLESS.cpp)
ADD_QV2RAY_TEST(generation src/core/connection/TestGeneration.cpp)
ADD_QV2RAY_TEST(qjsonio libs/QJsonStruct/QJsonIO.cpp)
ADD_QV2RAY_TEST(realping src/components/latency/TestRealPing.cpp)
