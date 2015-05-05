#Multi Threaded Client Server Application in C

A simple multi threaded client server application written in C
using POSIX API's for Multi Threading and IPC (Sockets)

##Server Setup

Run make and then execute server like so ./server from terminal

Note, default server port is 2342 - It is defined as constant in constants.h, so you may change the server port and other configuration options from there. All events are logged to standard log (/var/log/syslog). There are various log levels available and defined 
in log.h that you can use

##Client Setup

Launch client like so: ./client serverip
so, if server is running on 192.168.1.1 you'd run it like so:
./client 192.168.1.1

##Client to Client Communication

Users can also establish direct connections with each their through server (Note that, server acts initially but later on client to client communication is totally independent of server )

## User Accounts

Account Info is stored users.txt file - File is protected using
mutex to ensure as it's used in multiple threads.

#Contribution

Feel free to fork or send a pull request.

Thank you!
