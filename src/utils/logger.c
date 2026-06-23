#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "app.h"
#include "logger.h"

void log_info(char *format, ...)
{
    printf("[%s] ", APP_NAME);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void log_setup(char *step, char *status)
{
    printf("[%s] [SETUP] %-10s - %s\n", APP_NAME, step, status);
}

void log_to_file(FILE *file, char *format, ...)
{
    fprintf(file, "[%s] ", APP_NAME);
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
    fprintf(file, "\n");
}

void log_to_file_with_timestamp(FILE *file, char *format, ...)
{
    time_t now;
    struct tm *time_info;
    char buffer[20];

    time(&now);
    time_info = localtime(&now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time_info);

    fprintf(file, "[%s] [%s] ", APP_NAME, buffer);
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
    fprintf(file, "\n");
}