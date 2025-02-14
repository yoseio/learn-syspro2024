all: 1-1.out 1-2.out 1-3-tcp.out 1-3-udp.out 1-tcp.out 1-udp.out monitor.out send.out server.out

# ---

send.out: 2-send.o
	$(CXX) -o send.out 2-send.o

2-send.o: 2-send.cpp
	$(CXX) -c 2-send.cpp

monitor.out: 2-monitor.o
	$(CXX) -o monitor.out 2-monitor.o

2-monitor.o: 2-monitor.cpp
	$(CXX) -c 2-monitor.cpp

server.out: 2-server.o
	$(CXX) -o server.out 2-server.o

2-server.o: 2-server.cpp
	$(CXX) -c 2-server.cpp

# ---

1-2.out: 1-2.o TCPEchoClient.o
	$(CXX) -o 1-2.out 1-2.o TCPEchoClient.o

1-2.o: 1-2.cpp TCPEchoClient.h
	$(CXX) -c 1-2.cpp

1-3-tcp.out: 1-3-tcp.o TCPEchoClient.o
	$(CXX) -o 1-3-tcp.out 1-3-tcp.o TCPEchoClient.o

1-3-tcp.o: 1-3-tcp.cpp TCPEchoClient.h
	$(CXX) -c 1-3-tcp.cpp

1-tcp.out: 1-tcp.o TCPEchoServer.o
	$(CXX) -o 1-tcp.out 1-tcp.o TCPEchoServer.o

1-tcp.o: 1-tcp.cpp TCPEchoServer.h
	$(CXX) -c 1-tcp.cpp

TCPEchoClient.o: TCPEchoClient.cpp TCPEchoClient.h
	$(CXX) -c TCPEchoClient.cpp

TCPEchoServer.o: TCPEchoServer.cpp TCPEchoServer.h
	$(CXX) -c TCPEchoServer.cpp

# ---

1-1.out: 1-1.o UDPEchoClient.o
	$(CXX) -o 1-1.out 1-1.o UDPEchoClient.o

1-1.o: 1-1.cpp UDPEchoClient.h
	$(CXX) -c 1-1.cpp

1-3-udp.out: 1-3-udp.o UDPEchoClient.o
	$(CXX) -o 1-3-udp.out 1-3-udp.o UDPEchoClient.o

1-3-udp.o: 1-3-udp.cpp UDPEchoClient.h
	$(CXX) -c 1-3-udp.cpp

1-udp.out: 1-udp.o UDPEchoServer.o
	$(CXX) -o 1-udp.out 1-udp.o UDPEchoServer.o

1-udp.o: 1-udp.cpp UDPEchoServer.h
	$(CXX) -c 1-udp.cpp

UDPEchoClient.o: UDPEchoClient.cpp UDPEchoClient.h
	$(CXX) -c UDPEchoClient.cpp

UDPEchoServer.o: UDPEchoServer.cpp UDPEchoServer.h
	$(CXX) -c UDPEchoServer.cpp

# ---

clean:
	rm -f *.o *.out
