#pragma once

#ifndef __linux__
#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

//singleton that initializes win sock api
//throws runtime error if API couldn't be initialized
class WSA_INITIALIZER {
public:
	WSA_INITIALIZER(WSA_INITIALIZER&) = delete;
	WSA_INITIALIZER& operator=(WSA_INITIALIZER&) = delete;
	~WSA_INITIALIZER();

	static void Initialize();

private:
	WSA_INITIALIZER();
private: //data
	WSADATA mWsaData;
};

#define SOCKET_API_INIT WSA_INITIALIZER::Initialize();

#else

#define SOCKET_API_INIT

#endif