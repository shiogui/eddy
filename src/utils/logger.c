#include "logger.h"

#include <stdio.h>
#include <stdarg.h>

#include "app.h"

void log_info(char *format, ...)
{
    printf("[%s] ", APP_NAME);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}
