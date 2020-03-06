#pragma once
#ifdef __cplusplus
extern "C" {
#endif
    int ssr_start_helper(
        const char* localHost,
        int localPort,
        const char* remoteHost,
        int remotePort,
        const char* method,
        const char* password,
        const char* protocol,
        const char* protocol_param,
        const char* obfs,
        const char* obfs_param,
        int timeout
    );
    int stop_ss_local_server();
//    void set_qt_ui_log_ptr(void* ptr);
//    void reset_qt_ui_log_ptr();
    __declspec(dllexport)
    void set_qt_ui_log_ptr(void* ptr);
    __declspec(dllexport)
    void reset_qt_ui_log_ptr();


#ifdef __cplusplus
}
#endif
