#pragma once

//implements python-like socket interface

#include "InitSocks.h"
#include <string>
#include <memory>
#include <vector>
#include <cstdlib>

class Socket {
public: 
	// Bool values for SetOption
	static const char* True;
	static const char* False;
	
	//buffer that holds recieved data and its size in bytes
	//Returned by Recv function
	template<typename T = char>
	struct RecvBuffer {
		std::unique_ptr<T[]> dataPtr;
		int size;
	};

public:
	// Creates the socket. Throws std::runtime_error if socket couldn't be initialized
	explicit Socket(int family = AF_INET, int socktype = SOCK_STREAM, int protocol = IPPROTO_TCP);
	
	//Move constructor, required for Accept()
	Socket(Socket&&) = default;
	// Unusable, so deleted
	Socket(const Socket&) = delete;
	Socket& operator=(const Socket&) = delete;

	~Socket();

	// binds socket to specified interface and port
	// Throws std::runtime_error on failure
	void Bind(const std::string& host, const std::string& port);
	void Bind(const std::string& host, int port);

	// Listening on specified by Bind() host, port
	// size - size of the backlog queue
	void Listen(int size);
	
	// Accepting incoming connection
	// Blocking call
	// Returns (by moving) client socket to communicate with.
	Socket Accept();
	
	//Connects to the specified host:port
	//Returns true on success
	//Otherwise returns false
	bool Connect(const std::string& host, const std::string& port);
	bool Connect(const std::string& host, int port);
	// Sends text data. If some error occured return false
	// else return true
	bool Send(const std::string& text, int flags = 0);
	
	// Sends raw data of specified type
	// amount stands for data amount of specific type
	// Example - sending 4 ints : Send<int>(bufPtr, 4);
	// If some error occured return false
	// else return true
	template<typename T = char>
	bool Send(const T* data, int amount, int flags = 0);

	//Syntactic sugar for sending data stored in std::vector
	template<typename T>
	bool Send(const std::vector<T>& data, int flags = 0);

	// Receives specified data in RecvBuffer
	// If connection dropped or some error occured size will be < 1
	template<typename T = char>
	RecvBuffer<T> Recv(int amount, int flags = 0);
	
	// Close socket
	void Close();

	// Checks if socket initialized
	// Should be used to validate sockets returned by Accept()
	bool IsValid() const;

	// returns socket's port
	// 0 if port isn't initialized
	int GetPort() const;

	// returns socket's host in numeric representation
	// if ip is INADDR_ANY performs host lookup and
	// returns address of first avaliable external interface
	std::string GetIP() const;

protected:
	// Constructing client socket
	// Required for Accept() function
	Socket(int fd, const sockaddr_in& addr);

	// transforms s_addr to string
	// helper for GetIP function
	std::string GetIPHelper(const sockaddr_in& addr) const;
private: //members
	SOCKET m_socket;
	sockaddr_in m_addrInfo;

	// store this for addrinfo hints
	struct SocketMetadata {
		int socktype;
		int protocol;
	} m_sockData;
};



// ================================= TEMPLATE FUNCTION DEFINITIONS ==================================

template<typename T>
bool Socket::Send(const T* data, int amount, int flags) {
	int dataLen = sizeof(T) * amount;
	return (send(m_socket, (char*)data, dataLen, flags) >= 0);
}

template<typename T>
bool Socket::Send(const std::vector<T>& data, int flags) {
	return Send<T>(data.data(), data.size(), flags);
}

template<typename T>
Socket::RecvBuffer<T> Socket::Recv(int amount, int flags) {
	std::unique_ptr<T[]> ptr{ new T[amount+1] };

	int dataLen = sizeof(T) * amount;

	memset(ptr.get(), 0, sizeof(T) * amount);

	int size = recv(m_socket, (char*)ptr.get(), dataLen, flags);

	return RecvBuffer<T>{ std::move(ptr), size };
}