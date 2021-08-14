#include "FileSocket.h"
#include "SocketZ.h"


FileSocket::FileSocket(BufSize size)
{
	this->m_bufsize = size;
	this->m_buf = new char[10240]();
}


FileSocket::~FileSocket()
{
	delete []this->m_buf;
	this->m_buf = nullptr;
}

int FileSocket::SendFile(std::string && ip, short port)
{
	return false;
}

int FileSocket::RecvFile(std::string && downPath, bool clear)
{
	return 0;
}
