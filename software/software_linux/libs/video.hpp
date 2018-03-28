#include <stdbool.h>
#include <stdio.h>
#include <string>
#include <sstream>


// Used to be a C library, give it a namespace for cleanliness
#define VIDEO_SYNC 0x56;

#define VIDEO_RESET  0x26
#define VIDEO_GEN_VERSION 0x11
#define VIDEO_SET_PORT 0x24
#define VIDEO_READ_FBUF 0x32
#define VIDEO_GET_FBUF_LEN 0x34
#define VIDEO_FBUF_CTRL 0x36
#define VIDEO_DOWNSIZE_CTRL 0x54
#define VIDEO_DOWNSIZE_STATUS 0x55
#define VIDEO_READ_DATA 0x30
#define VIDEO_WRITE_DATA 0x31
#define VIDEO_COMM_MOTION_CTRL 0x37
#define VIDEO_COMM_MOTION_STATUS 0x38
#define VIDEO_COMM_MOTION_DETECTED 0x39
#define VIDEO_MOTION_CTRL 0x42
#define VIDEO_MOTION_STATUS 0x43
#define VIDEO_TVOUT_CTRL 0x44
#define VIDEO_OSD_ADD_CHAR 0x45

#define VIDEO_STOPCURRENTFRAME 0x0
#define VIDEO_STOPNEXTFRAME 0x1
#define VIDEO_RESUMEFRAME 0x3
#define VIDEO_STEPFRAME 0x2

#define VIDEO_640x480 0x00
#define VIDEO_320x240 0x11
#define VIDEO_160x120 0x22

#define VIDEO_MOTIONCONTROL 0x0
#define VIDEO_UARTMOTION 0x01
#define VIDEO_ACTIVATEMOTION 0x01

#define VIDEO_SET_ZOOM 0x52
#define VIDEO_GET_ZOOM 0x53

#define VIDEO_CAMERABUFFSIZ 100
#define VIDEO_CAMERADELAY 10

#define VIDEO_COL_CTRL 0x3C
#define VIDEO_MIRROR_CTRL 0x3A

class Video{
public:
    bool initialize(const char* file);
    bool reset(void);
    bool TVon(void);
    bool TVoff(void);
    bool takePicture(void);
    int *readPicture(int n);
    bool resumeVideo(void);
    int frameLength(void);
    char *getVersion(void);
    int available();
    int getDownsize(void);
    bool setDownsize(int);
    int getImageSize();
    bool setImageSize(int);
    bool getMotionDetect();
    int getMotionStatus(int);
    bool motionDetected();
    bool setMotionDetect(bool f);
    bool setMotionStatus(int x, int d1, int d2);
    bool cameraFrameBuffCtrl(int command);
    int getCompression();
    bool setCompression(int c);
    bool imageSettings(int brightness, int contrast, int off_contrast, int hue, int saturation);
    bool colorControl(int mode);
    void close();
    bool mirror_mode_on(void);

    char* setBaud9600();
    char* setBaud19200();
    char* setBaud38400();
    char* setBaud57600();
    char* setBaud115200();

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
