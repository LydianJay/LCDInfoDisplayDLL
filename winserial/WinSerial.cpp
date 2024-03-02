#include "WinSerial.h"
WinSerial* WinSerial::m_instance = nullptr;
WinSerial::~WinSerial() {
    if (m_readThread.joinable())m_readThread.join();
    CloseHandle(m_portHandle);
    CloseHandle(m_evntOverlap.hEvent);
}
bool WinSerial::serialInit(unsigned int comPortNum, unsigned int baudRate) {

    std::stringstream ss;
    ss << "COM" << comPortNum;

    m_portHandle = CreateFileA(ss.str().c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,      //  must be opened with exclusive-access
        NULL,   //  default security attributes
        OPEN_EXISTING, //  must use OPEN_EXISTING
        FILE_FLAG_OVERLAPPED,      //  not overlapped I/O
        NULL); //  hTemplate must be NULL for comm devices
    
    if (m_portHandle == INVALID_HANDLE_VALUE) return false;

    DCB dcb;
    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(m_portHandle, &dcb))return false;

    

    dcb.BaudRate = baudRate;     //  baud rate
    dcb.ByteSize = 8;             //  data size, xmit and rcv
    dcb.Parity = NOPARITY;      //  parity bit
    dcb.StopBits = ONESTOPBIT;    //  stop bit


    if (!SetCommState(m_portHandle, &dcb))return false;

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 2;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 5;
    if (SetCommTimeouts(m_portHandle, &timeouts) == FALSE)return false;

    if (SetCommMask(m_portHandle, EV_RXCHAR) ==FALSE)return false;
    if ((m_evntOverlap.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL)) == FALSE) return false;

    m_instance = this;
    m_init = true;
    return true;
}

unsigned int WinSerial::serialWrite(void* data, unsigned int dataSize) {
    DWORD nBytesToWrite = dataSize, nWritten = 0;
    
    return WriteFile(m_portHandle, data, nBytesToWrite, &nWritten, &m_evntOverlap);
}

bool WinSerial::serialIsReadAvailable() {
    return !m_readBuffer.empty();
}

std::string WinSerial::serialGetReadBuffer() {
    std::string copy = m_readBuffer.front();
    m_readBuffer.pop();
    return copy;
}

void WinSerial::startRead() {
    if (!m_init)return;

    if (m_readThread.joinable())m_readThread.join();

    m_threadFlag = true;
    m_readThread = std::thread(serialRead);
}


void WinSerial::serialRead() {


    while (m_instance->m_threadFlag) {
        

        DWORD dwEventMask = 0;
        if (!WaitCommEvent(m_instance->m_portHandle, &dwEventMask, &m_instance->m_evntOverlap)) {
            std::cout << "Wait Event error" << GetLastError() << '\n';
            return;
        }
        unsigned const int BYTE_SIZE = 64;
        unsigned int bytesToRead = BYTE_SIZE;

        DWORD readCount = 0;
        char buffer[BYTE_SIZE];
        if (ReadFile(m_instance->m_portHandle, buffer, bytesToRead, &readCount, &m_instance->m_evntOverlap)) {
            buffer[readCount] = 0;
            m_instance->m_readBuffer.push(std::string(buffer));
            
        }
        else if(GetLastError() != ERROR_IO_PENDING) {
            std::cout << "Error occured!: " << '\n';
        }


    }

   
}
