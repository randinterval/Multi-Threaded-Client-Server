all:
	gcc -c helpers.c -Wall -Werror
	gcc -c log.c -Wall -Werror
	gcc -c userlist.c -Wall -Werror -w -lpthread -lrt
	gcc server.c helpers.o userlist.o log.o -Wall -Werror -o server -lpthread
	gcc client.c helpers.o userlist.o log.o -Wall -Werror -o client -lpthread
	gcc clientclient.c helpers.o userlist.o log.o -Wall -Werror -o clientclient -lpthread
	rm helpers.o
	rm log.o
	rm userlist.o
clean:
	rm -rf *~
	rm server
	rm client
	rm clientclient
