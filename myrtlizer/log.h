//log.h - the header file which defines Log(); and LogErr();

#define LOGFILE	"gl.log"     // all Log(); messages will be appended to this file
extern int LogCreated;      // keeps track whether the log file is created or not
void myLog (char *message);    // logs a message to LOGFILE
void myLogErr (char *message); // logs a message; execution is interrupted
