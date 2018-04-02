#include <stdbool.h>
#include <stdio.h>
#include <string>
#include <sstream>


// Used to be a C library, give it a namespace for cleanliness
#define VIDEO_RESET  0x26
#define VIDEO_WRITE_DATA 0x31
#define VIDEO_TVOUT_CTRL 0x44

#define VIDEO_CAMERABUFFSIZ 100

#define VIDEO_COL_CTRL 0x3C
#define VIDEO_MIRROR_CTRL 0x3A

class Video{
public:
    bool reset(void);
    bool TVon(void);
    bool TVoff(void);
    bool imageSettings(int brightness, int contrast, int off_contrast, int hue, int saturation);
    bool colorControl(int mode);
    bool mirror_mode_on(void);

    std::string takeRawPicture();

    Video(const char* file);
private:
    int  serialNum;
	int  camerabuff[VIDEO_CAMERABUFFSIZ+1];
	int  bufferLen;
	int  frameptr;
	int video_uart;
    uint32_t * base;

    static constexpr unsigned width = 320;
    static constexpr unsigned height = 240;

	bool runCommand(int cmd, int args[], int argn, int resp, bool flushflag);
	void sendCommand(int cmd, int args[], int argn);
	int readResponse(int numbytes);
	bool verifyResponse(int command);
	void printBuff(void);
};
