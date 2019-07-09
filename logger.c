#include <stdio.h>
#include "logger.h"

static char dir[255];
static int loggerDirSet = 0;


void setLoggerDirectory(char *directory)
{
    sprintf(dir,"%s",directory);
    loggerDirSet = 1;
}

void deleteLog(char *filename)
{
    int ret=0;
    char dirAndFilename[255];

    if(loggerDirSet){
        sprintf(dirAndFilename,"%s/%s",dir,filename);
        ret = remove(dirAndFilename);
    }
    else{
        ret = remove(filename);
    }

   if(ret == 0) {
      fprintf(stdout,"Log file:%s deleted successfully!\n",filename);
   } else {
      fprintf(stderr,"Error in deleteLog(...): unable to delete the log file:%s\n",filename);
   }
}

void writeToLog(char *message,char *filename)
{
    FILE *out;
    char dirAndFilename[255];
    if(loggerDirSet){
        sprintf(dirAndFilename,"%s/%s",dir,filename);
        out = fopen(dirAndFilename,"a+");
    }
    else{
        out = fopen(filename,"a+");
    }

    if(out!=NULL){
        fprintf(out,"%s\n",message);
        fclose(out);
    }
}
void writeSeparatorToLog(char *filename)
{
    FILE *out;
    char dirAndFilename[255];
    if(loggerDirSet){
        sprintf(dirAndFilename,"%s/%s",dir,filename);
        out = fopen(dirAndFilename,"a+");
    }
    else{
        out = fopen(filename,"a+");
    }

    if(out!=NULL){
        fprintf(out,"%s\n","-------------------------------------------------------------------------------------------------------------------------");
        fclose(out);
    }
}


