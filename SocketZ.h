#pragma once
#ifndef _SOCKETZ_H_
#define _SOCKETZ_H_

#include <iostream>

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#endif

class SocketZ
{
private:
	enum class ZsockStatus {
		isNone,
		isCrea,
		isBind,
		isList,
		isLoop,
	};

	enum class ZsockType { tcp = 1, udp = 2 };

	static bool isInit;
public:
	SocketZ();
	SocketZ(std::string & ipaddr, short port, SocketZ::ZsockType type = ZsockType::tcp);
	~SocketZ();
	SocketZ(const SocketZ & c);

	bool create(std::string && ipaddr, short port,SocketZ::ZsockType type = ZsockType::tcp);
	bool bind();
	bool listen(int maxlist);
	bool createSocket(std::string && ipaddr, short port, SocketZ::ZsockType type = ZsockType::tcp);
	SocketZ accept();
	bool connect();
	
	int recv(char * buf, int lens);
	int send(const char * buf, int lens);
	int recv(std::string & buf);
	int send(std::string && buf);
	bool close();

public:
	void zeroInit();
	void setBlock(bool block);
	bool testFor(ZsockStatus src, ZsockStatus tar);

public:
	bool getBlock()const {
		return this->m_block;
	}

	int getSocket()const {
		return this->m_sock;
	}

private:
	int m_sock;
	bool m_block;
	short m_port;
	std::string m_ipaddr;
	SOCKADDR_IN m_sockaddr;
	SocketZ::ZsockType m_socktype;
	SocketZ::ZsockStatus m_status;
};



#endif

