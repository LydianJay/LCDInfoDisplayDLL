#pragma once
#include <Windows.h>
#include <string>
#include <sstream>
#include <thread>
#include <queue>
#include <iostream>
class WinSerial {


public:
	~WinSerial();
	bool serialInit(unsigned int comPortNum, unsigned int baudRate);
	unsigned int serialWrite(void* data, unsigned int dataSize);
	bool serialIsReadAvailable();
	std::string serialGetReadBuffer();
	void startRead();
private:
	static WinSerial* m_instance;
	bool m_threadFlag = false;
	bool m_init = false;
	OVERLAPPED m_evntOverlap = {0};
	std::thread m_readThread;
	HANDLE m_portHandle;
	std::queue<std::string> m_readBuffer;
private:
	
	static void serialRead();
};

