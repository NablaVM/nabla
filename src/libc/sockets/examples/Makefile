all:
	gcc basic.c ../sockets.c -o basic.test
	gcc tcpclient.c ../sockets.c -o tcpclient.test
	gcc tcpserver.c ../sockets.c -o tcpserver.test
	gcc udpclient.c ../sockets.c -o udpclient.test
	gcc udpserver.c ../sockets.c -o udpserver.test

clean:
	rm basic.test
	rm tcpclient.test
	rm tcpserver.test
	rm udpclient.test
	rm udpserver.test