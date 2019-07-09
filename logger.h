#ifndef __LOGGER_H_
#define __LOGGER_H_

void setLoggerDirectory(char *directory);
void deleteLog(char *filename);
void writeToLog(char *message,char *filename);
void writeSeparatorToLog(char *filename);

#endif // __LOGGER_H_
