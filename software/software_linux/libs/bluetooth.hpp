#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <stdio.h>
#include <string>
#include <termios.h>
#include <functional>
#include <vector>
#include "TermiosUtil.hpp"


class Wand {
public:
    
    enum Modes {
        gestureMode,
        typingMode,
        cursorMode
    };
    
    enum WandCommands {
        wandStart,
        wandStop
    };
    
    Wand(const char name[], Baudrate rate);
    
    typedef std::function<void(Modes m)> ModeCB;
    typedef std::function<void(WandCommands wc)> WandCommandCB;
    
    // Non-blocking receive, only possible with the unix-like interface
    void trypoll();
    
    
    void setValidModes(std::vector<Modes> modes);

    void setRemoteAddress(std::string address);
    bool connectToAddress(std::string address);
    void killConnection();
    void setMode(std::string mode);
    std::vector<std::pair<std::string, std::string>> getAvailableDevices(std::string time_length);
    
    void setMode(ModeCB cb){ m_modeCB = cb; };
    void setGesture(WandCommandCB cb){ m_wandCommandCB = cb; };
    
    Modes getMode() { return m_current_mode; }
    
    bool checkConnection();
    
private:
    int bluetooth_uart;
    unsigned m_recv_index;
    ModeCB m_modeCB;
    Modes m_current_mode;
    std::vector<Modes> m_validModes;
    bool m_connected;
    
    WandCommandCB m_wandCommandCB;
    WandCommands m_current_wand;
    
    // Number of bytes to read at once for trypoll
    static constexpr unsigned RECV_BYTES = 16;
    
    void recv(char val);
    bool waitForResponse(std::string response);
    std::vector<std::pair<std::string, std::string>> waitForInquiry(std::string time_length);
    
    void enterCommandMode();
    void exitCommandMode();
    
};


#endif /* BLUETOOTH_H_ */
