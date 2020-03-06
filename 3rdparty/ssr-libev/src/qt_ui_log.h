#ifndef QT_UI_LOG_H
#define QT_UI_LOG_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
void qt_ui_log_info(const char* format,...);
void qt_ui_log_error(const char* format,...);
void send_traffic_stat(uint64_t,uint64_t);
#ifdef __cplusplus
}
#endif
#endif // QT_UI_LOG_H
