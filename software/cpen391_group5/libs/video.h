#include <stdbool.h>
#include "system.h"
#include <stdio.h>


// Used to be a C library, give it a namespace for cleanliness
namespace Video {
#define VC0706_SYNC 0x56;

#define VC0706_RESET  0x26
#define VC0706_GEN_VERSION 0x11
#define VC0706_SET_PORT 0x24
#define VC0706_READ_FBUF 0x32
#define VC0706_GET_FBUF_LEN 0x34
#define VC0706_FBUF_CTRL 0x36
#define VC0706_DOWNSIZE_CTRL 0x54
#define VC0706_DOWNSIZE_STATUS 0x55
#define VC0706_READ_DATA 0x30
#define VC0706_WRITE_DATA 0x31
#define VC0706_COMM_MOTION_CTRL 0x37
#define VC0706_COMM_MOTION_STATUS 0x38
#define VC0706_COMM_MOTION_DETECTED 0x39
#define VC0706_MOTION_CTRL 0x42
#define VC0706_MOTION_STATUS 0x43
#define VC0706_TVOUT_CTRL 0x44
#define VC0706_OSD_ADD_CHAR 0x45

#define VC0706_STOPCURRENTFRAME 0x0
#define VC0706_STOPNEXTFRAME 0x1
#define VC0706_RESUMEFRAME 0x3
#define VC0706_STEPFRAME 0x2

#define VC0706_640x480 0x00
#define VC0706_320x240 0x11
#define VC0706_160x120 0x22

#define VC0706_MOTIONCONTROL 0x0
#define VC0706_UARTMOTION 0x01
#define VC0706_ACTIVATEMOTION 0x01

#define VC0706_SET_ZOOM 0x52
#define VC0706_GET_ZOOM 0x53

#define VC0706_CAMERABUFFSIZ 100
#define VC0706_CAMERADELAY 10

#define VC0706_COL_CTRL 0x3C
#define VC0706_MIRROR_CTRL 0x3A

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

    extern int  serialNum;
    extern int  camerabuff[VC0706_CAMERABUFFSIZ+1];
    extern int  bufferLen;
    extern int frameptr;
    extern FILE* video_uart;

    void common_init(void);
    bool runCommand(int cmd, int args[], int argn, int resp, bool flushflag);
    void sendCommand(int cmd, int args[], int argn);
    int readResponse(int numbytes);
    bool verifyResponse(int command);
    void printBuff(void);
};
