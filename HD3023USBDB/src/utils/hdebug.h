#ifndef HDEBUG_H
#define HDEBUG_H
#include<QDebug>

#define DbgLog QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug
#define InfoLog QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).info
#define WarningLog QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).warning
#define CriticalLog QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).critical
#define FatalLog QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).fatal

#if defined(QT_NO_DEBUG_OUTPUT)
#undef DbgPrint
#define DbgPrint QT_NO_QDEBUG_MACRO
#else
#define DEBUG_TEST  //调试测试宏
#endif



#endif // HDEBUG_H
