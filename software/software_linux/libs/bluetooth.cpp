#include <string.h>
#include "bluetooth.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void Bluetooth::LedOn(){
    char command[] = "1";
    write(bluetooth_uart, command, strlen(command));
}

void Bluetooth::LedOff(){
    char command[] = "0";
    write(bluetooth_uart, command, strlen(command));
}

std::string Bluetooth::ReadResponse(){
    //not sure working correctly
    std::string response;

    while(1){
        for(int i=0; i<1; i++){
            response = fgetc(F);
        }
        break;
    }
    return response;
}

int Bluetooth::openFile(){
    F = fdopen(bluetooth_uart, "r+");
    if(F == 0) {
        return 0;
    }
    return 1;
}

void Bluetooth::closeFile(){
    fclose(F);
}

int Bluetooth::GetFd(){
    int i = bluetooth_uart;
    return i;
}

Bluetooth::Bluetooth(const char name[]) {
    bluetooth_uart = open(name, O_RDWR);
    
    if (bluetooth_uart == -1) {
        printf("Failed to open UART device\n");
        while (1)
            ;
    }
    else {
        printf("Connection successful\n");
    }
    
}
