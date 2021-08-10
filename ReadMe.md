# EasySocketZ

## example
```C++
#include <iostream>
#include "SockLoopZ.h"
#include "SocketZ.h"
#include <thread>

void conThread() {
	// block
	SocketZ socket;
	socket.create("127.0.0.1", 8888);
	socket.connect();
	socket.send("client:123");
	std::cout << "[*] client send " << std::endl;
	socket.close();
}

int main() {
	// unblock
	SocketZ socket;
	socket.createServer("127.0.0.1", 8888);
	socket.setBlock(false);
	SockLoopZ selectLoop;
	std::thread t1(conThread);
	selectLoop.pushZSock(false, 1, socket);
	selectLoop.setTimeout(3, 3);
	std::vector<SockLoopZ::vecSockZ> sockvec;
	while (selectLoop.runLoop(sockvec)) {
		for (auto &i : sockvec) {
			if (i.m_status == SockLoopZ::tagStatus::Read) {
				SocketZ client = i.m_sock.accept();
				std::string buf;
				client.recv(buf);
				std::cout << "[+] server recv " << buf.c_str() << std::endl;
				client.close();
			}
		}
	}

	t1.join();
	return 0;
}
```

## 实现原理
简单的socket封装，windows实测没问题，目前支持tcp和udp(未实现)  
简单的处理了粘包以及阻塞和非阻塞的实现
非阻塞采用select，全局fd_set，最大数量默认4096连接数