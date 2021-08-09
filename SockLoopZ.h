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
	SockLoopZ();
	SockLoopZ(timeval timeout);
	~SockLoopZ();

	enum class tagStatus {
		None = 0,
		Read,
		Write,
		RandW
	};

	typedef struct tagvecSockZ {
		SocketZ m_sock;
		tagStatus m_status;
	}vecSockZ;

	void clearVec();
	bool pushZSock(bool isclear,int count, ...);
	bool runLoop(std::vector<vecSockZ> & vec, int sleepnum = 0);

public:
	void setTimeout(timeval timeout) {
		this->m_timeout = timeout;
	}

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

	void addToFdVec(int sock);
	void removeToFdVec(int sock);


private:
	std::vector<SocketZ> m_sockvec;
	timeval m_timeout;

	static fd_set m_vecset;
};

#endif
