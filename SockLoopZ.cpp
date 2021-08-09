#include "SockLoopZ.h"
#include "SocketZ.h"

fd_set SockLoopZ::m_vecset;


SockLoopZ::SockLoopZ()
{
	static bool init = false;
	if (init == false) {
		FD_ZERO(&m_vecset);
		init = true;
	}

	setTimeout(-1, -1);
}

SockLoopZ::SockLoopZ(timeval timeout)
{
	SockLoopZ();
	setTimeout(timeout);
}


SockLoopZ::~SockLoopZ()
{
}

void SockLoopZ::clearVec()
{
	this->m_sockvec.clear();
}

bool SockLoopZ::pushZSock(bool isclear,int count, ...)
{
	std::vector<SocketZ> tempVec;

	va_list ap;
	va_start(ap, count);
	for (int i = 0; i < count; i++) {
		SocketZ & temp = va_arg(ap, SocketZ);
		if (temp.getBlock() == true) {
			tempVec.clear();
			return false;
		}
		tempVec.push_back(temp);
	}
	va_end(ap);

	if (tempVec.empty()) {
		return false;
	}

	if (isclear) {
		this->m_sockvec.clear();
	}

	for (auto i : tempVec) {
		m_sockvec.push_back(i);
	}

	return true;
}

bool SockLoopZ::runLoop(std::vector<vecSockZ> & vec, int sleepnum)
{
	if (sleepnum != 0) {
		::Sleep(sleepnum);
	}

	vec.clear();

	fd_set rset;
	fd_set wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	rset = m_vecset;
	wset = m_vecset;

	int ret = 0;


	if (TestTimeout(-1, -1)) {
		ret = select(0, &rset, &wset, NULL, NULL);
	}
	else {
		ret = select(0, &rset, &wset, NULL, &m_timeout);
	}

	if (ret <= 0) {
		return false;
	}

	SockLoopZ::vecSockZ value;

	for (auto index : m_sockvec) {
		value.m_status = tagStatus::None;

		int status = 0;
		int sock = index.getSocket();

		if (FD_ISSET(sock, &rset)) {
			status |= 1;
		}
		if (FD_ISSET(sock, &wset)) {
			status |= 2;
		}

		if ((tagStatus)status != tagStatus::None) {
			value.m_sock = index;
			value.m_status = (SockLoopZ::tagStatus)status;
		}
	}

	if (vec.empty()) {
		return false;
	}

	return true;
}

void SockLoopZ::addToFdVec(int sock)
{
	if (FD_ISSET(sock, &m_vecset)) {
		return;
	}

	FD_SET(sock, &m_vecset);
}

void SockLoopZ::removeToFdVec(int sock)
{
	if (!FD_ISSET(sock, &m_vecset)) {
		return;
	}

	FD_CLR(sock, &m_vecset);
}
