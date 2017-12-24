#include "Socket.h"
#include <iostream>
#include <thread>

void Listener() {

}

int main() {
	Socket server;
	server.Bind("0.0.0.0", 4324);

	server.Listen(5);

	std::cout << "Listening on " << server.GetIP() << ":" << server.GetPort() << "\n";

	Socket client = server.Accept();

	std::cout << "Client " << client.GetIP() << ":" 
			  << client.GetPort() << " accepted\n";

	client.Send("Hello epta\n");
	auto data = client.Recv(1024);

	std::cout << "Received: " << data.size << "\n";
	client.Send(data.dataPtr.get(), data.size);

	std::cin.get();
}