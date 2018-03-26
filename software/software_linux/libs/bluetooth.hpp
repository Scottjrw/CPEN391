#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <stdio.h>
#include <string>
#include <termios.h>

class Bluetooth {
public:
    Bluetooth(const char name[]);
    
    void LedOn();
    void LedOff();
    std::string ReadResponse();
    int GetFd();
    int openFile();
    void closeFile();
    
private:
    int bluetooth_uart;
    FILE * F;
    
};


#endif /* BLUETOOTH_H_ */
