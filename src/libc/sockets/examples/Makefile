all:
	gcc basic.c ../sockets.c -o basic.test
	gcc tcpclient.c ../sockets.c -o tcpclient.test
	gcc tcpserver.c ../sockets.c -o tcpserver.test

clean:
	rm basic.test
	rm tcpclient.test
	rm tcpserver.test