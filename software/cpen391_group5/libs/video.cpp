/*
 */
#include "video.hpp"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

namespace Video {
    int  serialNum;
    int  camerabuff[VC0706_CAMERABUFFSIZ+1];
    int  bufferLen;
    int frameptr;
    FILE* video_uart;

    bool initialize(const char* file) {
        video_uart = fopen(file, "r+");
        serialNum = 0x00;
        frameptr  = 0;
        bufferLen = 0;

        if (video_uart == NULL) {
            printf("Failed to open UART device\n");
            return false;
        }
        else {
            printf("opened\n");
            return true;
        }
    }

    void close() {
        fclose(video_uart);
    }

    bool imageSettings(int brightness, int contrast, int off_contrast, int hue, int saturation) {
        int args_brightness[] = {0x05, 0x01, 0x01, 0x1A, 0x10, brightness};
        int args_contrast[] = {0x05, 0x01, 0x01, 0x1A, 0x11, contrast};
        int args_saturation[] = {0x05, 0x01, 0x01, 0x1A, 0x15, saturation};
        int args_ctrl[] = {0x05, 0x01, 0x01, 0x1A, 0x00, 0x3E};

        runCommand(VC0706_WRITE_DATA, args_ctrl, 6, 5, true);
        runCommand(VC0706_WRITE_DATA, args_brightness, 6, 5, true);
        runCommand(VC0706_WRITE_DATA, args_contrast, 6, 5, true);
        runCommand(VC0706_WRITE_DATA, args_saturation, 6, 5, true);
        return true;
    }

    bool colorControl(int mode) {
        int args[] = {0x02, 0x01, mode};

        if (mode == 0) {
            return runCommand(VC0706_COL_CTRL, args, 3, 5, true);
        }
        else if (mode == 1) {
            return runCommand(VC0706_COL_CTRL, args, 3, 5, true);
        }
        else {
            return runCommand(VC0706_COL_CTRL, args, 3, 5, true);
        }
    }

    bool mirror_mode_on(void) {
        int args[] = {0x02, 0x01, 0x01};

        return runCommand(VC0706_MIRROR_CTRL, args, 3, 5, true);
    }

    bool reset() {
        int args[] = {0x0};

        return runCommand(VC0706_RESET, args, 1, 5, true);
    }

    bool motionDetected() {
        if (readResponse(4) != 4) {
            return false;
        }
        if (! verifyResponse(VC0706_COMM_MOTION_DETECTED))
            return false;

        return true;
    }


    bool setMotionStatus(int x, int d1, int d2) {
        int args[] = {0x03, x, d1, d2};

        return runCommand(VC0706_MOTION_CTRL, args, sizeof(args), 5, true);
    }


    int getMotionStatus(int x) {
        int args[] = {0x01, x};

        return runCommand(VC0706_MOTION_STATUS, args, sizeof(args), 5, true);
    }


    bool setMotionDetect(bool flag) {
        if (! setMotionStatus(VC0706_MOTIONCONTROL,
                    VC0706_UARTMOTION, VC0706_ACTIVATEMOTION))
            return false;

        int args[] = {0x01, flag};

        return runCommand(VC0706_COMM_MOTION_CTRL, args, sizeof(args), 5, true);
    }



    bool getMotionDetect(void) {
        int args[] = {0x0};

        if (! runCommand(VC0706_COMM_MOTION_STATUS, args, 1, 6, true))
            return false;

        return camerabuff[5];
    }

    int getImageSize() {
        int args[] = {0x4, 0x4, 0x1, 0x00, 0x19};
        if (! runCommand(VC0706_READ_DATA, args, sizeof(args), 6, true))
            return -1;

        return camerabuff[5];
    }

    bool setImageSize(int x) {
        int args[] = {0x05, 0x04, 0x01, 0x00, 0x19, x};

        return runCommand(VC0706_WRITE_DATA, args, sizeof(args), 5, true);
    }

    /****************** downsize image control */

    int getDownsize(void) {
        int args[] = {0x0};
        if (! runCommand(VC0706_DOWNSIZE_STATUS, args, 1, 6, true))
            return -1;

        return camerabuff[5];
    }

    bool setDownsize(int newsize) {
        int args[] = {0x01, newsize};

        return runCommand(VC0706_DOWNSIZE_CTRL, args, 2, 5, true);
    }

    /***************** other high level commands */

    char * getVersion(void) {
        int args[] = {0x01};

        sendCommand(VC0706_GEN_VERSION, args, 1);
        // get reply
        if (!readResponse(VC0706_CAMERABUFFSIZ))
            return 0;
        camerabuff[bufferLen] = 0;  // end it!
        return (char *)camerabuff;  // return it!
    }


    /***************** baud rate commands */

    char* setBaud9600() {
        int args[] = {0x03, 0x01, 0xAE, 0xC8};

        sendCommand(VC0706_SET_PORT, args, sizeof(args));
        // get reply
        if (!readResponse(VC0706_CAMERABUFFSIZ))
            return 0;
        camerabuff[bufferLen] = 0;  // end it!
        return (char *)camerabuff;  // return it!

    }

    char* setBaud19200() {
        int args[] = {0x03, 0x01, 0x56, 0xE4};

        sendCommand(VC0706_SET_PORT, args, sizeof(args));
        // get reply
        if (!readResponse(VC0706_CAMERABUFFSIZ))
            return 0;
        camerabuff[bufferLen] = 0;  // end it!
        return (char *)camerabuff;  // return it!
    }

    char* setBaud38400() {
        int args[] = {0x03, 0x01, 0x2A, 0xF2};

        sendCommand(VC0706_SET_PORT, args, sizeof(args));
        // get reply
        if (!readResponse(VC0706_CAMERABUFFSIZ))
            return 0;
        camerabuff[bufferLen] = 0;  // end it!
        return (char *)camerabuff;  // return it!
    }

    char* setBaud57600() {
        int args[] = {0x03, 0x01, 0x1C, 0x1C};

        sendCommand(VC0706_SET_PORT, args, sizeof(args));
        // get reply
        if (!readResponse(VC0706_CAMERABUFFSIZ))
            return 0;
        camerabuff[bufferLen] = 0;  // end it!
        return (char *)camerabuff;  // return it!
    }

    char* setBaud115200() {
        int args[] = {0x03, 0x01, 0x0D, 0xA6};

        sendCommand(VC0706_SET_PORT, args, sizeof(args));
        // get reply
        if (!readResponse(VC0706_CAMERABUFFSIZ))
            return 0;
        camerabuff[bufferLen] = 0;  // end it!
        return (char *)camerabuff;  // return it!
    }

    /****************** high level photo comamnds */

    void OSD(int x, int y, char *str) {
        if (strlen(str) > 14) { str[13] = 0; }

        int args[17] = {(int)strlen(str), (int)strlen(str)-1, (y & 0xF) | ((x & 0x3) << 4)};
        for (int i=0; i<(int)strlen(str); i++) {
            char c = str[i];
            if ((c >= '0') && (c <= '9')) {
                str[i] -= '0';
            } else if ((c >= 'A') && (c <= 'Z')) {
                str[i] -= 'A';
                str[i] += 10;
            } else if ((c >= 'a') && (c <= 'z')) {
                str[i] -= 'a';
                str[i] += 36;
            }

            args[3+i] = str[i];
        }

        runCommand(VC0706_OSD_ADD_CHAR, args, strlen(str)+3, 5, true);
        printBuff();
    }

    bool setCompression(int c) {
        int args[] = {0x5, 0x1, 0x1, 0x12, 0x04, c};
        return runCommand(VC0706_WRITE_DATA, args, sizeof(args), 5, true);
    }

    int getCompression(void) {
        int args[] = {0x4, 0x1, 0x1, 0x12, 0x04};
        runCommand(VC0706_READ_DATA, args, sizeof(args), 6, true);
        printBuff();
        return camerabuff[5];
    }

    bool takePicture() {
        frameptr = 0;
        return cameraFrameBuffCtrl(VC0706_STOPCURRENTFRAME);
    }

    bool resumeVideo() {
        return cameraFrameBuffCtrl(VC0706_RESUMEFRAME);
    }

    bool TVon() {
        int args[] = {0x1, 0x1};
        return runCommand(VC0706_TVOUT_CTRL, args, 2, 5, true);
    }
    bool TVoff() {
        int args[] = {0x1, 0x0};
        return runCommand(VC0706_TVOUT_CTRL, args, 2, 5, true);
    }

    bool cameraFrameBuffCtrl(int command) {
        int args[] = {0x1, command};
        return runCommand(VC0706_FBUF_CTRL, args, sizeof(args), 5, true);
    }

    int frameLength(void) {
        int args[] = {0x01, 0x00};

        if (!runCommand(VC0706_GET_FBUF_LEN, args, sizeof(args), 9, true))
            return 0;

        int len;
        len = camerabuff[5];
        len <<= 8;
        len |= camerabuff[6];
        len <<= 8;
        len |= camerabuff[7];
        len <<= 8;
        len |= camerabuff[8];

        return len;
    }


    int available(void) {
        return bufferLen;
    }


    int* readPicture(int n) {
        int args[] = {0x0C, 0x0, 0x0A,
            0, 0, frameptr >> 8, frameptr & 0xFF,
            0, 0, 0, n,
            VC0706_CAMERADELAY >> 8, VC0706_CAMERADELAY & 0xFF};

        if (! runCommand(VC0706_READ_FBUF, args, sizeof(args), 5, false))
            return 0;


        // read into the buffer PACKETLEN!
        if (readResponse(n+5) == 0)
            return 0;


        frameptr += n;

        return camerabuff;
    }

    /**************** low level commands */


    bool runCommand(int cmd, int *args, int argn,
            int resplen, bool flushflag) {

        sendCommand(cmd, args, argn);
        if (readResponse(resplen) != resplen)
            return false;
        if (! verifyResponse(cmd))
            return false;
        return true;
    }

    void sendCommand(int cmd, int args[], int argn) {
        int i;

        char string_out[50];
        string_out[0] = 0x56;
        string_out[1] = serialNum;
        string_out[2] = cmd;

        for (i = 0; i < argn; i++) {
            string_out[3+i] = args[i];
        }



        fwrite(string_out, argn + 3, 1, video_uart);
    }

    int readResponse(int numbytes) {
        bufferLen = 0;
        int i;

        for(i = 0; i < numbytes; i++) {
            camerabuff[bufferLen++] = fgetc(video_uart);
        }

        //  	for (i = 0; i < 5; i++) {
        //  		printf("%i ", camerabuff[i]);
        //  	}
        //  	printf("\n");

        return bufferLen;
    }

    bool verifyResponse(int command) {
        if ((camerabuff[0] != 0x76) ||
                (camerabuff[1] != serialNum) ||
                (camerabuff[2] != command) ||
                (camerabuff[3] != 0x0))
            return false;
        return true;

    }

    void printBuff() {
        int i;
        for (i = 0; i< bufferLen; i++) {
            printf(" 0x");
            printf("%x", camerabuff[i]);
        }
        printf("\n");
    }
}
