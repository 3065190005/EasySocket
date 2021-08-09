#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "SocketZ.h"
#include "SockLoopZ.h"

bool SocketZ::isInit = false;

SocketZ::SocketZ()
{
	zeroInit();

#ifdef _WIN32
	if (!isInit) {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		isInit = true;
	}
#endif
}

SocketZ::SocketZ(std::string & ipaddr, short port, SocketZ::ZsockType type)
{
	SocketZ();
	this->create(ipaddr.c_str(), port,type);
}


SocketZ::~SocketZ()
{
}

SocketZ::SocketZ(const SocketZ & c)
{
	zeroInit();
	this->m_sock = c.m_sock;
	this->m_block = c.m_block;
	this->m_port = c.m_port;
	this->m_ipaddr = c.m_ipaddr;
	this->m_sockaddr = c.m_sockaddr;
	this->m_socktype = c.m_socktype;
	this->m_status = c.m_status;
}

bool SocketZ::create(std::string && ipaddr, short port, SocketZ::ZsockType type)
{
	if (!testFor(ZsockStatus::isNone, this->m_status) || port <= 0) {
		goto end;
	}

	m_sock = ::socket(AF_INET, (int)type, 0);

	if (m_sock <= 0) {
		goto end;
	}

	m_port = port;
	m_ipaddr.append(ipaddr);
	m_socktype = type;
	m_status = ZsockStatus::isCrea;
	return true;

end:
	this->close();
	return false;
}

bool SocketZ::bind()
{
	if (!testFor(ZsockStatus::isCrea, this->m_status)) {
		goto end;
	}

	int ref = -1;

	m_sockaddr.sin_family = AF_INET; 
	m_sockaddr.sin_port = htons(m_port);
	m_sockaddr.sin_addr.s_addr = inet_addr(m_ipaddr.c_str());
	ref = ::bind(m_sock, (SOCKADDR*)&m_sockaddr, sizeof(SOCKADDR));

	if (ref != 0) {
		goto end;
	}

	m_status = ZsockStatus::isBind;
	return true;

end:
	return false;
}

bool SocketZ::listen(int maxlist)
{
	if (!testFor(ZsockStatus::isBind, this->m_status)) {
		goto end;
	}

	int ref = -1;
	ref = ::listen(this->m_sock, maxlist);

	if (ref != 0) {
		goto end;
	}

	m_status = ZsockStatus::isList;
	return true;

end:
	return false;
}

bool SocketZ::createSocket(std::string && ipaddr, short port, SocketZ::ZsockType type)
{
	if (!testFor(ZsockStatus::isNone, this->m_status) || port <= 0) {
		goto end;
	}

	if (!this->create(ipaddr.c_str(), port, type)) {
		goto end;
	}

	if (!this->bind()) {
		goto end;
	}

	if (!this->listen(20)) {
		goto end;
	}

	return true;

end:
	this->close();
	return false;
}

SocketZ SocketZ::accept()
{
	SocketZ clisock;
	char * cli_ip = nullptr;

	if (!testFor(ZsockStatus::isList, this->m_status)) {
		goto end;
	}

	SOCKADDR_IN clntAddr;
	int nSize = sizeof(SOCKADDR_IN);
	int clntSock = ::accept(this->m_sock, (SOCKADDR*)&clntAddr, &nSize);
	if (clntSock <= 0) {
		goto end;
	}
	cli_ip = ::inet_ntoa(clntAddr.sin_addr);

	clisock.zeroInit();
	clisock.m_sock = clntSock;
	clisock.m_port = ::ntohs(clntAddr.sin_port);
	clisock.m_ipaddr.append(cli_ip);
	clisock.m_sockaddr = clntAddr;
	clisock.m_socktype = this->m_socktype;
	clisock.m_status = ZsockStatus::isLoop;
	return clisock;

end:
	return SocketZ();
}

bool SocketZ::connect()
{
	if (!testFor(ZsockStatus::isCrea, this->m_status)) {
		goto end;
	}

	int ret = -1;
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons(m_port);
	m_sockaddr.sin_addr.s_addr = inet_addr(m_ipaddr.c_str());

	ret = ::connect(this->m_sock, (sockaddr*)&m_sockaddr, sizeof(m_sockaddr));
	if (ret != 0) {
		goto end;
	}

	m_status = ZsockStatus::isLoop;
	return true;

end:
	return false;
}

int SocketZ::recv(char * buf, int lens)
{
	int recvLen = (lens == -1 ? sizeof(buf) : lens);
	int bufref = 0;
	int buflen = 0;
	int offset = 0;
	int rs = 1;
	while (rs) {
		buflen = ::recv(m_sock, buf + offset, recvLen, 0);
		if (buflen < 0) {
			if (errno == EAGAIN || errno == EINTR) {
				break;
			}
			else {
				return -1;
			}
		}
		else if (buflen == 0) {
			return -2;
		}

		bufref += buflen;

		if (bufref == sizeof(buf)) {
			offset = sizeof(buf);
			char * tchar = new char[sizeof(buf)];
			strcpy(tchar, buf);
			delete buf;
			buf = new char[sizeof(tchar) + recvLen];
			memset(buf, 0, sizeof(buf));
			strcpy(buf, tchar);
			delete tchar;
			tchar = nullptr;
			rs = 1;
		}
		else {
			rs = 0;
		}

	}
	return bufref;
}

int SocketZ::send(const char * buf, int lens)
{
	int tmp;
	size_t total = lens;
	const char *p = buf;
	while (1) {
		tmp = ::send(m_sock, p, total, 0);
		if (tmp < 0) {
			if (errno == EINTR)
				return -1;
			if (errno == EAGAIN) {
				::Sleep(1);
				continue;
			}
			return -1;
		}
		if (tmp == total) {
			return lens;
		}
		total -= tmp;
		p += tmp;
	}
	return tmp;
}

int SocketZ::recv(std::string & buf)
{
	char * sbuf = new char[2048];
	int len = SocketZ::recv(sbuf, -1);
	buf.clear();
	buf.append(sbuf);
	return len;
}

int SocketZ::send(std::string & buf)
{
	return SocketZ::send(buf.c_str(), buf.length());
}

bool SocketZ::close()
{
	if (this->m_sock > 0) {closesocket(this->m_sock);}
	zeroInit();
	return true;
}

void SocketZ::setBlock(bool block)
{
	if (m_sock <= 0) {
		return;
	}

	if (this->m_block == block) {
		return;
	}

	this->m_block = block;

#ifdef _WIN32
	unsigned long cm = m_block == true ? 0 : 1;
	::ioctlsocket(this->m_sock, FIONBIO, &cm);
#else
	int flags = fcntl(sock, f_GETFL, 0);
	if(this->m_block)
		flags = flags &~O_NONBLOCK;
	else
		flags = flags | O_NONBLOCK;
#endif

	SockLoopZ sockSelect;

	if (m_block == true) {
		sockSelect.removeToFdVec(this->m_sock);
	}
	else {
		sockSelect.addToFdVec(this->m_sock);
	}
}

bool SocketZ::testFor(ZsockStatus src, ZsockStatus tar)
{
	if (src == tar)
		return true;
	return false;
}

void SocketZ::zeroInit()
{
	this->m_port = 0;
	this->m_sock = 0;
	this->m_block = true;
	this->m_ipaddr.clear();
	this->m_status = ZsockStatus::isNone;
	this->m_socktype = SocketZ::ZsockType::tcp;
	memset(&this->m_sockaddr, 0, sizeof(SOCKADDR_IN));
	setBlock(true);
}
