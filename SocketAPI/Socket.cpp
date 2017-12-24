#include "Socket.h"

const char* Socket::True = "\x01";
const char* Socket::False = "\x00";

#include <stdexcept>

Socket::Socket(int family, int socktype, int protocol) : m_socket(0) {
	SOCKET_API_INIT

	memset(&m_addrInfo, 0, sizeof(sockaddr_in));
	m_socket = socket(family, socktype, protocol);

	if (m_socket == INVALID_SOCKET) {
		throw std::runtime_error("Couldn't create socket");
	}

	m_addrInfo.sin_family = family;

	m_sockData.socktype = socktype;
	m_sockData.protocol = protocol;
}

Socket::Socket(int fd, const sockaddr_in& addr) : m_socket(fd), m_addrInfo(addr), m_sockData{ 0 } {

}

Socket::~Socket() {
	Close();
}

void Socket::Bind(const std::string& host, int port) {
	if (!inet_pton(m_addrInfo.sin_family, host.c_str(), &m_addrInfo.sin_addr.s_addr)) {
		throw std::runtime_error("Invalid ip address");
	}

	m_addrInfo.sin_port = htons(port);
	
	if (bind(m_socket, (sockaddr*)&m_addrInfo, sizeof(sockaddr)) < 0) {
		throw std::runtime_error("Couldn't bind on specified ip:port");
	}
}

void Socket::Bind(const std::string& host, const std::string& port) {
	int iPort = atoi(port.c_str());
	Bind(host, iPort);
}

bool Socket::Connect(const std::string& host, int port) {
	char portStr[5];
	snprintf(portStr, 5, "%d", port);
	return Connect(host, portStr);
}

bool Socket::Connect(const std::string& host, const std::string& port) {
	addrinfo hints;
	addrinfo* pAddr;
	
	memset(&hints, 0, sizeof(addrinfo));

	hints.ai_family = m_addrInfo.sin_family;
	hints.ai_socktype = m_sockData.socktype;
	hints.ai_protocol = m_sockData.protocol;

	getaddrinfo(host.c_str(), port.c_str(), &hints, &pAddr);

	m_addrInfo = *(sockaddr_in*)pAddr->ai_addr;

	int status = connect(m_socket, pAddr->ai_addr, pAddr->ai_addrlen);
	freeaddrinfo(pAddr);

	return status >= 0;
}

void Socket::Listen(int backlog) {
	listen(m_socket, backlog);
}

Socket Socket::Accept() {
	sockaddr_in clientAddr;
	int size = sizeof(sockaddr_in);

	SOCKET clientFd = accept(m_socket, (sockaddr*)&clientAddr, &size);

	return Socket(clientFd, clientAddr);
}

bool Socket::Send(const std::string& text, int flags) {
	return (send(m_socket, text.c_str(), text.size(), flags) >= 0);
}

void Socket::Close() {
	if (IsValid()) {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

int Socket::GetPort() const {
	return ntohs(m_addrInfo.sin_port);
}

std::string Socket::GetIP() const {
	
}

bool Socket::IsValid() const {
	return m_socket != INVALID_SOCKET;
}