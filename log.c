/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D
    Standardized Logging

    Reference: http://www.gnu.org/software/libc/manual/html_node/Submitting-Syslog-Messages.html

*/

#include "headers.h"


/*
These are the levels defined in syslog and we are using these levels
inside our wrapper
LOG_EMERG
    A panic condition was reported to all processes.
LOG_ALERT
    A condition that should be corrected immediately.
LOG_CRIT
    A critical condition.
LOG_ERR
    An error message.
LOG_WARNING
    A warning message.
LOG_NOTICE
    A condition requiring special handling.
LOG_INFO
    A general information message.
LOG_DEBUG
    A message useful for debugging programs.

REFERNECE: http://pubs.opengroup.org/onlinepubs/007908775/xsh/syslog.h.html
*/


void logger(int logType, char * format, ...)
{
    va_list variableArguments;
    va_start(variableArguments, format);
    //Get Time
    time_t currentTime;
    time(&currentTime);
    char * time = ctime(&currentTime);
    removeline(time);
    switch(logType)
    {
        case LOG_INFO:
        {
            openlog("[OS-Project-2342][INFO]",0,LOG_USER);
            //Write to System Log
            vsyslog(LOG_INFO,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[INFO %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_NOTICE:
        {
            openlog("[OS-Project-2342][NOTICE]",0,LOG_USER);
            vsyslog(LOG_NOTICE,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[NOTICE %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_ALERT:
        {
            openlog("[OS-Project-2342][ALERT]",0,LOG_USER);
            vsyslog(LOG_ALERT,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[ALERT %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_WARNING:
        {
            openlog("[OS-Project-2342][WARNING]",0,LOG_USER);
            vsyslog(LOG_WARNING,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[WARNING %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_EMERG:
        {
            openlog("[OS-Project-2342][EMERGENCY]",0,LOG_USER);
            vsyslog(LOG_EMERG,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[EMERGENCY %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_CRIT:
        {
            openlog("[OS-Project-2342][CRITICAL]",0,LOG_USER);
            vsyslog(LOG_CRIT,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[CRITICAL %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_DEBUG:
        {
            openlog("[OS-Project-2342][DEBUG]",0,LOG_USER);
            vsyslog(LOG_DEBUG,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[DEBUG %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        case LOG_ERR:
        {
            openlog("[OS-Project-2342][ERROR]",0,LOG_USER);
            vsyslog(LOG_ERR,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[ERROR %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
        default:
        {
            //In case invalid log type is passed, we set it to LOG_INFO
            openlog("[OS-Project-2342][INFO]",0,LOG_USER);
            vsyslog(LOG_INFO,format,variableArguments);

            //Write to Console
            va_start(variableArguments, format);
            printf("[INFO %s]", time);
            vprintf(format, variableArguments);
            va_end(variableArguments);
            printf("\n");
            fflush(stdout);

            break;
        }
    }
}