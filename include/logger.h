#ifndef LOGGER_H
#define LOGGER_H

void log_info(char *format, ...);
void log_setup(char *step, char *status);

/// @brief Logs the content to a desired file
/// @param file Pointer to an open and write enabled file
/// @param format Message/format to be written out
/// @param args Arguments to be interpolated into the format
void log_to_file(FILE *file, char *format, ...);
void log_to_file_with_timestamp(FILE *file, char *format, ...);

#endif