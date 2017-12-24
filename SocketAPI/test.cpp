#include "Socket.h"
#include "ThreadSafePrinter.h"

#include <iostream>
#include <thread>

using CONSOLE = ThreadSafePrinter;

void Listener() {
	Socket server;
	
	server.Bind("0.0.0.0", 4321);
	
	CONSOLE::Print("[SERVER INFO]: Listening on ", server.GetIP(), ":",
				    server.GetPort(), "...\n");

	server.Listen(1);

	Socket client = server.Accept();
	
	CONSOLE::Print("[SERVER INFO]: Accepted connection from ", client.GetIP(),
				   ":", client.GetPort(), "\n");

	while (true) {
		auto buffer = client.Recv(1024);
		
		if (buffer.size <= 0) {
			CONSOLE::Print("[SERVER INFO]: Connection dropped\n");
			break;
		}

		CONSOLE::Print("[SERVER INFO]: Accepted ", buffer.size, " bytes from client", "\n");

		for (int i = 0; i < buffer.size; ++i) {
			buffer.dataPtr[i] = toupper(buffer.dataPtr[i]);
		}

		client.Send(buffer.dataPtr.get(), buffer.size);
	}

}

int main() {
	std::thread t{ Listener };
	t.detach();

	Socket client;

	while (!client.Connect("127.0.0.1", 4321)) {
		CONSOLE::Print("Couldn't connect to the server\n");
	}

	std::string userInput;
	
	CONSOLE::Print("Press ENTER to start data transmission dialog\n");
	std::cin.get();
	
	while (true) {
		CONSOLE::Print(">");
		std::getline(std::cin, userInput);
		
		while (userInput.size() == 0) {
			std::getline(std::cin, userInput);
		}

		if (userInput == "END") {
			client.Close();
			break;
		}

		client.Send(userInput);

		auto response = client.Recv(1024);
		CONSOLE::Print(" ", response.dataPtr.get(), "\n");
	}

	CONSOLE::Print("Press ENTER to exit\n");
	std::cin.get();
}