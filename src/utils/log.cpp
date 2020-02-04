#include <utils/log.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

static const char* log_level_str[] = {
    "INFO",
    "SUCCESS",
    "WARNING",
    "ERROR"
};

static const char* log_level_color[] = {
    "\x1B[0m",
    "\x1B[32m",
    "\x1B[33m",
    "\x1B[31m"
};

void log(LogLevel level, const char* file, int line, const char *fmt, ...) {
    va_list args1;
    va_start(args1, fmt);
    va_list args2;
    va_copy(args2, args1);
    std::vector<char> buf(1+std::vsnprintf(nullptr, 0, fmt, args1));
    va_end(args1);
    std::vsnprintf(buf.data(), buf.size(), fmt, args2);
    va_end(args2);
    printf("%s[%s:%d] %s:\x1B[0m %s\n", log_level_color[level], file, line, log_level_str[level], buf.data());

    if (level == ERROR)
        exit(EXIT_FAILURE);
}