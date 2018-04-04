#include <string.h>
#include "bluetooth.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <err.h>


void Wand::LedOn(){
    char command[] = "1";
    write(bluetooth_uart, command, strlen(command));
}

void Wand::LedOff(){
    char command[] = "0";
    write(bluetooth_uart, command, strlen(command));
}

void Wand::enterCommandMode(){
    char command[] = "$$$";
    int val = write(bluetooth_uart, command, strlen(command));
    
    if (val != strlen(command)){
        err(1, "command failed");
    }
    
    waitForResponse("CMD\n");
}

void Wand::exitCommandMode() {
    char command[] = "---\n";
    int val = write(bluetooth_uart, command, strlen(command));
    if (val != strlen(command)){
        err(1, "command failed");
    }
    waitForResponse("END\n");
}

void Wand::setRemoteAddress(std::string address) {
    enterCommandMode();
    std::string s = "SR," + address + "\n";
    const char * command = s.c_str();
    int val = write(bluetooth_uart, command, strlen(command));
    if (val != strlen(command)){
        err(1, "command failed");
    }
    waitForResponse("AOK\n");
    exitCommandMode();
}

bool Wand::connectToAddress(std::string address) {
    enterCommandMode();
    std::string s = "C," + address + "\n";
    const char * command = s.c_str();
    int val = write(bluetooth_uart, command, strlen(command));
    if (val != strlen(command)){
        err(1, "command failed");
    }
    
    usleep(5000000);
    return checkConnection();
}

void Wand::killConnection() {
    bool connection = checkConnection();
    if (connection){
        enterCommandMode();
        char command[] = "K,\n";
        int val = write(bluetooth_uart, command, strlen(command));
        if (val != strlen(command)){
            err(1, "command failed");
        }
    }
    else {
        printf("\nno connection to end\n");
    }

}

void Wand::setMode(std::string mode) {
    exitCommandMode();
    std::string s = "SM," + mode + "\n";
    const char * command = s.c_str();
    int val = write(bluetooth_uart, command, strlen(command));
    if (val != strlen(command)){
        err(1, "command failed");
    }
    waitForResponse("AOK\n");
    exitCommandMode();
}

std::vector<std::vector<std::string>> Wand::getAvailableDevices(std::string time_length) {
    enterCommandMode();
    std::string s = "I," + time_length + "\n";
    const char * command = s.c_str();
    int val = write(bluetooth_uart, command, strlen(command));
    if (val != strlen(command)){
        err(1, "command failed");
    }
    std::vector<std::vector<std::string>> devices = waitForInquiry(time_length);
    exitCommandMode();
    return devices;
}

Wand::Wand(const char name[], Baudrate rate) {
    std::cout << "connecting" << std::endl;
    bluetooth_uart = open(name, O_RDWR | O_APPEND | O_NONBLOCK);
    
    assert(bluetooth_uart >= 0);
    
    TermiosUtil::SetSpeed(bluetooth_uart, rate);
    
}

void Wand::trypoll() {
    int num_rd;
    char buf[RECV_BYTES];
    
    while ((num_rd = read(bluetooth_uart, buf, RECV_BYTES)) > 0) {
        for (int i = 0 ; i < num_rd; i++)
            recv(buf[i]);
    }
}

void Wand::recv(char val) {
    if (val == '1'){
        printf("\nwand change\n");
        if (m_current_wand == wandStart){
            if (m_wandCommandCB != nullptr){
                m_wandCommandCB(wandStop);
            }
            m_current_wand = wandStop;
        }
        else if (m_current_wand == wandStop){
            if (m_wandCommandCB != nullptr){
                m_wandCommandCB(wandStart);
            }
            m_current_wand = wandStart;
        }
    }
    else if (val == '2'){
        printf("\nmode change\n");
        if (m_current_mode == gestureMode){
            if (m_modeCB != nullptr){
                m_modeCB(typingMode);
            }
            m_current_mode = typingMode;
        }
        else if (m_current_mode == typingMode){
            if (m_modeCB != nullptr){
                m_modeCB(cursorMode);
            }
            m_current_mode = cursorMode;
        }
        else if (m_current_mode == cursorMode){
            if (m_modeCB != nullptr){
                m_modeCB(gestureMode);
            }
            m_current_mode = gestureMode;
        }
    }
}

bool Wand::waitForResponse(std::string response) {
    int num_rd;
    char buf[RECV_BYTES];
    int response_length = (int)response.length();
    int characters_matched = 0;
    
    while(characters_matched != response_length){
        while ((num_rd = read(bluetooth_uart, buf, RECV_BYTES)) > 0) {
            for (int i = 0 ; i < num_rd; i++){
                printf("%c", buf[i]);
                if (buf[i] == response.at(characters_matched)){
                    characters_matched++;
                    if (characters_matched == response_length){
                        return true;
                    }
                }
            }
        }
    }
    return true;
}

std::vector<std::vector<std::string>> Wand::waitForInquiry(std::string time_length) {
    int num_rd;
    char buf[RECV_BYTES];
    std::string wait_string = "Inquiry,T=" + time_length + ",COD=0\n";
    std::string error_string = "No Devices Found\n";
    std::string found_string = "Found ";
    
    waitForResponse(wait_string);
    
    int error_response_length = (int)error_string.length();
    int found_response_length = (int)found_string.length();
    int characters_matched = 0;
    int response_length = error_response_length;
    std::string response = error_string;
    bool response_picked = false;
    
    std::string devices_found_string = "";
    char new_line = '\n';
    bool new_line_found = false;
    bool finding_device_num = false;
    
    while(!new_line_found){
        while ((num_rd = read(bluetooth_uart, buf, RECV_BYTES)) > 0) {
            for (int i = 0 ; i < num_rd; i++){
                printf("%c", buf[i]);
                if (!response_picked){
                    if (buf[i] == error_string.at(0)){
                        return std::vector<std::vector<std::string>>();
                    }
                    else if (buf[i] == found_string.at(0)){
                        response = found_string;
                        response_length = found_response_length;
                        response_picked = true;
                    }
                }
                if (finding_device_num) {
                    if (buf[i] == new_line){
                        printf("\nnew line found");
                        new_line_found = true;
                        goto devices_num_found;
                    }
                    else {
                        devices_found_string += buf[i];
                    }
                }
                else if (buf[i] == response.at(characters_matched)){
                    characters_matched++;
                    if (characters_matched == response_length){
                        finding_device_num = true;
                    }
                }
            }
        }
    }
    
devices_num_found:
    
    int num_devices_found = std::stoi(devices_found_string);
    printf("\nDevices Found: %i\n", num_devices_found);
    int new_lines_reached = 0;
    std::string seperator = ",";
    int commas_counted = 0;
    std::vector<std::vector<std::string>> devices(num_devices_found, std::vector<std::string>(2,""));
    bool devices_found = false;
    
    
    
    while(new_lines_reached != num_devices_found){
        while ((num_rd = read(bluetooth_uart, buf, RECV_BYTES)) > 0) {
            for (int i = 0 ; i < num_rd; i++){
                printf("%c", buf[i]);
                if (devices_found) {
                    if (buf[i] == new_line){
                        break;
                    }
                }
                else if (buf[i] == new_line){
                    new_lines_reached++;
                    commas_counted = 0;
                    if (new_lines_reached == num_devices_found){
                        devices_found = true;
                    }
                }
                else if (buf[i] == seperator.at(0)){
                    commas_counted++;
                }
                else {
                    if (commas_counted == 0){
                        devices[new_lines_reached][0] += buf[i];
                    }
                    else if (commas_counted == 1){
                        devices[new_lines_reached][1] += buf[i];
                    }
                }
            }
        }
    }

    return devices;
}

bool Wand::checkConnection() {
    enterCommandMode();
    std::string connection_status = "GK\n";
    const char * connection_command = connection_status.c_str();
    int val2 = write(bluetooth_uart, connection_command, strlen(connection_command));
    if (val2 != strlen(connection_command)){
        err(1, "command failed");
    }
    
    int num_rd;
    char buf[RECV_BYTES];
    std::string success = "1,0,0";
    std::string failure = "0,0,0";
    bool connection_found = false;
    
    while(!connection_found){
        while ((num_rd = read(bluetooth_uart, buf, RECV_BYTES)) > 0) {
            for (int i = 0 ; i < num_rd; i++){
                printf("%c", buf[i]);
                if (buf[i] == success.at(0)){
                    exitCommandMode();
                    return true;
                }
                else if (buf[i] == failure.at(0)){
                    exitCommandMode();
                    return false;
                }
            }
        }
    }
    exitCommandMode();
    return false;
}
