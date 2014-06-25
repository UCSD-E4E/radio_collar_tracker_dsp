// log.cpp;

#include <stdlib.h>
#include <stdio.h>
//#include "system.h"        // SysShutdown();
#include "log.h"

int LogCreated = 0;

void myLog (char *message)
{
    FILE *file;

    if (!LogCreated) {
        file = fopen(LOGFILE, "w");
        LogCreated = 1;
    }
    else
        file = fopen(LOGFILE, "a");

    if (file == NULL) {
        if (LogCreated)
            LogCreated = 0;
        return;
    }
    else
    {
        fputs(message, file);
        fclose(file);
    }

//    if (file)
//        fclose(file);
}

void myLogErr (char *message)
{
    myLog(message);
    myLog("\n");
}
