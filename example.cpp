#include <iostream>
#include "SockLoopZ.h"
#include "SocketZ.h"
#include <thread>

void TcpconThread() {
	// block
	SocketZ socket;
	socket.create("127.0.0.1", 8888);
	socket.connect();
	socket.send("client:123");
	std::cout << "[*] client send " << std::endl;
	socket.close();
}

int TcpServer() {
	SocketZ socket;
	socket.createServer("127.0.0.1", 8888);
	socket.setBlock(false);
	SockLoopZ selectLoop;
	std::thread t1(TcpconThread);
	selectLoop.pushZSock(false, 1, socket);
	selectLoop.setTimeout(3, 3);
	std::vector<SockLoopZ::vecSockZ> sockvec;
	while (selectLoop.runLoop(sockvec)) {
		for (auto &i : sockvec) {
			if (i.m_status == SockLoopZ::tagStatus::Read || i.m_status == SockLoopZ::tagStatus::RandW) {
				SocketZ client = i.m_sock.accept();
				std::string buf;
				client.recv(buf);
				std::cout << "[+] server recv " << buf.c_str() << std::endl;
				selectLoop.popZSock(socket.getSocket());
				client.close();
				socket.close();
			}
		}
	}

	t1.join();
	return 0;
}

void UdpThread() {
	SocketZ socket;
	socket.create("127.0.0.1", 9999, SocketZ::ZsockType::udp);
	if (socket.udpSend("client:123", "127.0.0.1", 9999) == -1) {
		std::wcout << GetLastError() << std::endl;
	}
	std::cout << "[*] client send " << std::endl;
	socket.close();
}

int UdpServer() {
	SocketZ socket;
	socket.createServer("127.0.0.1", 9999,SocketZ::ZsockType::udp);
	socket.setBlock(false);
	SockLoopZ selectLoop;
	std::thread t1(UdpThread);
	selectLoop.pushZSock(false, 1, socket);
	selectLoop.setTimeout(3, 3);
	std::vector<SockLoopZ::vecSockZ> sockvec;
	while (selectLoop.runLoop(sockvec)) {
		for (auto &i : sockvec) {
			if (i.m_status == SockLoopZ::tagStatus::Read || i.m_status == SockLoopZ::tagStatus::RandW) {
				std::string buf;
				socket.udpRecv(buf);
				std::cout << "[+] server recv " << buf.c_str() << std::endl;
				selectLoop.popZSock(socket.getSocket());
				socket.close();
			}
		}
	}

	t1.join();
	return 0;
}

int main() {
	// Tcp
	// return TcpServer();
	// Udp
	return UdpServer();
}