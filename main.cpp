#include "coretemp/sharedmem/GetCoreTempInfo.h"
#include "sysinfo/include/SystemInformation.h" // https://github.com/SaulBerrenson/sys_info/tree/main
#include "Struct.h"
#include "winserial/WinSerial.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>


bool isLoggin = true;


SendInfo getInfo(CORE_TEMP_SHARED_DATA& sData, SystemInformation& pInfo) {

	SendInfo sInfo = {};
	float sumTemp = 0;
	uint32_t sumLoad = 0;
	for (size_t i = 0; i < sData.uiCoreCnt; i++) {
		sumTemp += sData.fTemp[i];
		sumLoad += sData.uiLoad[i];
	}

	sInfo.cpuTemp = sumTemp / sData.uiCoreCnt;
	sInfo.cpuUsage = sumLoad / sData.uiCoreCnt;
	
	
	sInfo.memUsage = ((float)pInfo.GetTotalUsageMemory() / pInfo.GetTotalMemory()) * 100.0f;
	return sInfo;
}


void procInput() {

	while (true) {

		if (GetAsyncKeyState(VK_F1) & 1) {
			ShowWindow(GetConsoleWindow(), SW_HIDE);
			isLoggin = false;
		}
		else if (GetAsyncKeyState(VK_F2) & 1) {
			ShowWindow(GetConsoleWindow(), SW_SHOW);
			isLoggin = true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}


int main() {

	
	
	WinSerial serialCom;
	if (!initCoreTemp()) {
		std::cout << "[ERROR] Misssing GetCoreTempInfo.dll \n";
		std::cout << "[INFO] Exiting with error....\n";
		std::cin.get();
		return -1;
	}

	std::cout << "[INFO] If you want to change the port number please edit the comport.ini file\n";

	uint8_t comPort = 3;

	std::ifstream file("comport.ini");
	
	if (file.is_open()) {

		std::string content;
		file >> content;

		comPort = atoi(content.c_str());
		std::cout << "\n[INFO] Default Setting Loaded\n\n";
	}
	else {
		std::ofstream def("comport.ini");
		if (def.is_open()) {
			def << 3;
		}
		def.close();
	}


	file.close();

	CORE_TEMP_SHARED_DATA sData = {};
	if (!serialCom.serialInit(comPort, 9600)) {

		std::cout << "\n[ERROR] The comport is not available\n";
		std::cout << "[INFO] Please change the comport.ini with the port with Arduino Nano is set and restart the program\n";
		std::cout << "[INFO] Exiting with error....\n\n";
		std::cin.get();
		return -1;
	}


	std::thread inputThread(procInput);
	inputThread.detach();


	SystemInformation pInfo;
	

	
	std::cout << "\n[INFO] Attach Success!\n";
	std::cout << "[INFO] Press 'F1' To hide the console\n";
	std::cout << "[INFO] Press 'F2' To show the console\n\n\n";


	while (true) {
		fnGetCoreTempInfoAlt(&sData);
		SendInfo sInfo = getInfo(sData, pInfo);
		serialCom.serialWrite(&sInfo, sizeof(SendInfo));
		if(isLoggin)
			std::cout << "[INFO] Sending data to COM" << (int)comPort << "\n";
		
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}


	std::cin.get();
	return 0;
}