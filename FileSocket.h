#pragma once
#ifndef _FILE_SOCKET_H_
#define _FILE_SOCKET_H_

#include <iostream>

class SocketZ;
class FileSocket
{
public:
	enum class BufSize {
		Default = 0,
		Lv1 = 128,
		Lv2 = 256,
		Lv3 = 512,
		Lv4 = 1024,
		Lv5 = 2048,
		Lv6 = 4096,
		Lv7 = 8192,
	};

	FileSocket(BufSize size = BufSize::Default);
	~FileSocket();

	int SendFile(std::string && ip, short port);
	int RecvFile(std::string && downPath, bool clear);

private:
	BufSize m_bufsize = BufSize::Default;
	char * m_buf = nullptr;
};

#endif
