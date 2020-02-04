#pragma once


#define LOG(level, fmt, ...) log(level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define ASSERT(cond, fmt, ...)\
do {\
    if (!(cond)) LOG(ERROR, fmt, ##__VA_ARGS__);\
} while(0)

enum LogLevel
{
    INFO,
    SUCCESS,
    WARNING,
    ERROR
};

void log(LogLevel level, const char* file, int line, const char *fmt, ...);