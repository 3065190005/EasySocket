#pragma once
#ifndef _ZSOCKLOOP_H_
#define _ZSOCKLOOP_H_

#include <iostream>
#include <vector>
#include "SocketZ.h"

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

class SocketZ;
class SockLoopZ
{
public:
	// default 
	SockLoopZ();

	// default and Call setTimeout
	SockLoopZ(timeval timeout);
	~SockLoopZ();

	// socket status
	enum class tagStatus {
		None = 0,	// can not read and write
		Read,		// can read
		Write,		// can write
		RandW		// can read and write
	};

	// runLoop block type
	typedef struct tagvecSockZ {
		SocketZ m_sock;
		tagStatus m_status;
	}vecSockZ;

	// clear select vector target
	// sockclose| Call close
	void clearVec(bool sockclose = true);

	// isclear|call clear before the push_back
	// push_back to SocketZ vector from arg
	bool pushZSock(bool isclear,int count, ...);
	// push_back to SocketZ vector from vec
	bool pushZSock(bool isclear, std::vector<SocketZ> vec);

	// pop SocketZ from m_sockvec ,remove = remove from m_sockvec
	SocketZ popZSock(int sockfd, bool remove = true);
	void popZSock(SocketZ & sock, bool remove = true);

	// select loop vector and return ret when target socket is not None
	bool runLoop(std::vector<vecSockZ> & ret, int sleepnum = 0);

public:

	// set select Timeout from struct timeval
	void setTimeout(timeval timeout) {
		this->m_timeout = timeout;
	}

	// set select Timeout from two value
	// -1,-1 == block another is Unblock
	void setTimeout(int sec, int nsec) {
		timeval timeout;
		timeout.tv_sec = sec;
		timeout.tv_usec = nsec;

		this->m_timeout = timeout;
	}

	bool TestTimeout(int sec, int nsec) {
		int rsec, rnesc;
		rsec = m_timeout.tv_sec;
		rnesc = m_timeout.tv_usec;

		if (sec == rsec && nsec == rnesc) {
			return true;
		}
		return false;
	}

	// add to m_vecset when SocketZ call the setBlock(true)
	void addToFdVec(int sock);

	// remote from m_vecset when SocketZ call the setBlock(false)
	void removeToFdVec(int sock);


private:
	std::vector<SocketZ> m_sockvec;	// select target
	timeval m_timeout;		// select timeout

	static fd_set m_vecset;	// single fd vector
};

#endif
