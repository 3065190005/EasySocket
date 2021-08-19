#include "FileSocket.h"
#include <sstream>

std::string FileSocket::m_workPath;

void FileSocket::setWorkPath(std::string && string)
{
	m_workPath = string;
}

bool FileSocket::buildLocal(std::string && ip, short port)
{
	if (m_status != NetWorkState::None) { return false; }
	if (!m_socket.createServer(ip.c_str(), port, SocketZ::ZsockType::tcp)) { goto end; }
	m_socket.setBlock(false);
	m_status = NetWorkState::Server;
	return true;

end:
	m_socket.close();
	return false;
}

bool FileSocket::joinServer(std::string && ip, short port)
{
	if (m_status != NetWorkState::None) { return false; }
	if (!m_sender.create(ip.c_str(), port)) { goto end; }
	m_sender.setBlock(false);
	m_status = NetWorkState::Client;
	return true;

end:
	m_socket.close();
	return false;
}

void FileSocket::destroy()
{
}

FileSocket::FileSocket()
{
	this->m_blockSize = BufSize::Default;
	this->m_buf = new char[10240]();
	this->m_brk = false;
	this->m_socket = SocketZ();
	this->m_status = NetWorkState::None;

	sendCallBack = nullptr;
	recvCallBack = nullptr;
	saveFileCall = nullptr;
}


FileSocket::~FileSocket()
{
	if (this->m_buf != nullptr) {
		delete[] this->m_buf;
		this->m_buf = nullptr;
	}
}

int FileSocket::sendFile(std::string && filePath, BufSize blocksize, bool clear)
{
	return false;
}

bool FileSocket::updateStatus()
{
	if (m_brk) {
		return false;
	}

	if (!networkUpdate()) { return false; }

	m_updateSock.pushZSock(false,1,m_sender);
	m_updateSock.runLoop(m_loopVec);
	for (auto &i : m_loopVec) {
		if (((int)(i.m_status)) & 0b01 == true) {
			// sock can read
			recvUpdate();
		}
		
		if (((int)(i.m_status)) & 0b10 == true) {
			// sock can write
			sendUpdate();
		}
	}

	bufToConfig();

	m_updateSock.clearVec(false);
}

bool FileSocket::networkUpdate()
{
	if (m_status == NetWorkState::Connect) { return true; }

	switch (m_status) {
	case NetWorkState::Client:
		if (!m_sender.connect()) { return false; }
		break;
	case NetWorkState::Server:
		m_sender = m_socket.accept();
		if (!m_sender.isLoopStatus()) { return false; }
		m_sender.setBlock(false);
		break;
	default:
		return false;
	}

	m_status = NetWorkState::Connect;
	return true;
}

bool FileSocket::sendUpdate()
{	
	return true;
}

bool FileSocket::recvUpdate()
{
	long long size = 0;
	int id = 0;
	int idoffset = 0;
	int socklen = 0;
	char sizebuf[21] = { 0 };
	char idbuf[4] = { 0 };
	char filesize[16] = { 0 };
	char totalfile[10240] = { 0 };

	socklen = m_sender.recv(sizebuf, 20);
	if (socklen != 20) { return false; }
	socklen = 0;
	strcpy_s(filesize, 16, sizebuf);
	strcpy_s(idbuf, 4, sizebuf+16);

	id = (int)idbuf;
	size = (long long)filesize;

	socklen = m_sender.recv(totalfile, size);
	if (socklen != size) { return false; }
	strcpy_s(idbuf, 4, totalfile);
	idoffset = (int)idbuf;
	if (idoffset != id) { return false; }
	m_cacheBufVec.push_back(totalfile);
	return true;
}

void FileSocket::bufToConfig()
{
}

void FileSocket::setBrk(bool brk)
{
	this->m_brk = brk;
}
