/*
    Operating System Project
    By: Saad Ahmed L1F12BSCS2342 Section D
    Standardized Logging
    I have simply built a wrapper on syslog so that logs can be sent to
    /var/log/syslog easily.

    Reference: http://www.gnu.org/software/libc/manual/html_node/Submitting-Syslog-Messages.html
*/


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


void logger(int logType, char * format, ...);
