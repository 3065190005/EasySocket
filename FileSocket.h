#pragma once
#ifndef _FILE_SOCKET_H_
#define _FILE_SOCKET_H_

#include <iostream>
#include <functional>
#include <vector>
#include "SockLoopZ.h"
#include "SocketZ.h"

class FileSocket
{
public:
	// block size
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

	// socket status
	enum class NetWorkState {
		None = 0,
		Client = 1,
		Server = 2,
		Connect = 3,
	};
	
	// call back type
	typedef struct tagCallBackEvent {
		bool isSuccess = false;
		long long totalSize = 0;
		long long fileMaxPage = 0;
		const char * filePath = nullptr;
		long long blockSize = 0;
		long long blockPage = 0;
		const char * blockBuf = nullptr;
	} CallEvent;

	FileSocket();
	~FileSocket();

	// config cache save path
	void static setWorkPath(std::string &&string);

	// create socket with server , accept
	bool buildLocal(std::string && ip, short port);

	// create socket with client ,connect
	bool joinServer(std::string && ip, short port);

	// close socket
	void destroy();

	// sendFile first
	int sendFile(std::string && filePath, BufSize blocksize = BufSize::Default, bool clear = false);

	// main loop
	bool updateStatus();

	// test for network is connect
	bool networkUpdate();

	// update send block from target
	bool sendUpdate();

	// update recv block from target
	bool recvUpdate();

	// make block to buffer
	void bufToConfig();

	// callback interface
	std::function<void(CallEvent & event)> sendCallBack;
	std::function<void(CallEvent & event)> recvCallBack;
	std::function<void(CallEvent & event)> saveFileCall;

	void setBrk(bool brk);

private:
	static std::string m_workPath;	// config cache path
	NetWorkState m_status;			// is connect , client or server
	BufSize m_blockSize;			// recv||send one buffer size
	class SocketZ m_socket;			// create socket
	class SocketZ m_sender;			// connect return socket
	char * m_buf;					// recv||send buffer
	bool m_brk;						// if brk == true updateStatus well be break

	class SockLoopZ m_updateSock;				// select socket
	std::vector<SockLoopZ::vecSockZ> m_loopVec;	// select socket return vec

	// callback return value
	CallEvent m_sendEvent;
	CallEvent m_recvEvent;
	CallEvent m_saveEvent;

	std::vector<int> m_randomNumVec;		// all id save
	std::vector<std::string> m_cacheBufVec;	// all buffer save
};

#endif
