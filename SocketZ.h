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
		isNone,	// can create
		isCrea,	// can bind
		isBind,	// can listen
		isList,	// can accept
		isLoop, // can recv & send
	};

	enum class ZsockType { tcp = 1, udp = 2 };

	// windows well need init WinSocket first
	static bool isInit;

public:
	// default
	SocketZ();

	// default and Call create function
	SocketZ(std::string & ipaddr, short port, SocketZ::ZsockType type = ZsockType::tcp);
	~SocketZ();

	// copy construct
	SocketZ(const SocketZ & c);

	// target address , target port , use socket type - tcp or udp
	bool create(std::string && ipaddr, short port,SocketZ::ZsockType type = ZsockType::tcp);

	// bind target, need create first
	bool bind();

	// listen socket, need bind first
	bool listen(int maxlist);

	// sequence call create -> bind -> listen
	// target address , target port , use socket type - tcp or udp
	bool createServer(std::string && ipaddr, short port, SocketZ::ZsockType type = ZsockType::tcp);

	// accept and return new socket class , need listen first (well set status)
	SocketZ accept();

	// connect server , need create first ,
	bool connect();
	
	// recv tcp
	int recv(char * buf, int lens);

	// send tcp
	int send(const char * buf, int lens);

	// recv interface(auto
	int recv(std::string & buf);

	// send interface(auto
	int send(std::string && buf);

	// close socket and init class status to isNone
	bool close();

public:
	// value init
	void zeroInit();

	// set socket block status
	void setBlock(bool block);

	// test class socket status
	bool testFor(ZsockStatus src, ZsockStatus tar);

public:

	// get block status
	bool getBlock()const {
		return this->m_block;
	}

	// get socket FD
	int getSocket()const {
		return this->m_sock;
	}

private:
	int m_sock;						// socket fd
	bool m_block;					// block status| true = block | false = unblock
	short m_port;					// target port
	std::string m_ipaddr;			// target address
	SOCKADDR_IN m_sockaddr;			// socket addr
	SocketZ::ZsockType m_socktype;	// socket protocol - tcp or udp
	SocketZ::ZsockStatus m_status;	// class status , None Bind Listen Accept Loop
};



#endif

