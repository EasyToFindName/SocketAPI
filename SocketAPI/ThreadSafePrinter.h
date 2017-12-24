#pragma once

#include <mutex>
#include <iostream>

//implements thread safe printing to stdout
//doesn't work if put mutex inside class :(
class ThreadSafePrinter {
public:
	template<typename... Args>
	static void Print(Args... args);
public:
	ThreadSafePrinter(const ThreadSafePrinter&) = delete;
	ThreadSafePrinter& operator=(const ThreadSafePrinter&) = delete;
	ThreadSafePrinter(ThreadSafePrinter&&) = delete;
protected:
	template<typename TFirst, typename... TRest>
	static void PrintHelper(TFirst current, TRest... rest);
	
	static void PrintHelper();
	ThreadSafePrinter() = default;
private:
};

std::mutex GLOBAL_CONSOLE_LOCK;

template<typename... Args>
void ThreadSafePrinter::Print(Args... args) {
	std::lock_guard<std::mutex> guard(GLOBAL_CONSOLE_LOCK);
	PrintHelper(args...);
}

template<typename TFirst, typename... TRest>
void ThreadSafePrinter::PrintHelper(TFirst cur, TRest... rest) {
	std::cout << cur;
	PrintHelper(rest...);
}

void ThreadSafePrinter::PrintHelper() {
	return;
}