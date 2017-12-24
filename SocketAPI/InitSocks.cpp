#ifndef __linux__

#include "InitSocks.h"

#include <exception>
#include <string>

class Wsa_Except : public std::exception {
public:
	Wsa_Except(int errCode) : std::exception() {
		std::string sErrCode = std::to_string(errCode);
		mErrMsg = "SOCKET API INITIALIZATION ERROR #" + sErrCode;
	}
	virtual const char* what() const override {
		return mErrMsg.c_str();
	}
private:
	std::string mErrMsg;
};

WSA_INITIALIZER::WSA_INITIALIZER() {

	if (FAILED(WSAStartup(MAKEWORD(2, 2), &mWsaData))) {
		int errorCode = WSAGetLastError();
		throw Wsa_Except(errorCode);
	}
}

WSA_INITIALIZER::~WSA_INITIALIZER() {
	WSACleanup();
}

void WSA_INITIALIZER::Initialize() {
	static WSA_INITIALIZER someName;
}

#else 

#endif