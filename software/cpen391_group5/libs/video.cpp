/*
 */
#include "video.hpp"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>


Video::Video(const char* file) {
	video_uart = fopen(file, "r+");
	serialNum = 0x00;
	frameptr  = 0;
	bufferLen = 0;

	if (video_uart == NULL) {
		printf("Failed to open UART device\n");
		while(1);
	}
	else {
		printf("opened\n");
	}
}

void Video::close() {
	fclose(video_uart);
}

bool Video::imageSettings(int brightness, int contrast, int off_contrast, int hue, int saturation) {
	int args_brightness[] = {0x05, 0x01, 0x01, 0x1A, 0x10, brightness};
	int args_contrast[] = {0x05, 0x01, 0x01, 0x1A, 0x11, contrast};
	int args_saturation[] = {0x05, 0x01, 0x01, 0x1A, 0x15, saturation};
	int args_ctrl[] = {0x05, 0x01, 0x01, 0x1A, 0x00, 0x3E};

	runCommand(VIDEO_WRITE_DATA, args_ctrl, 6, 5, true);
	runCommand(VIDEO_WRITE_DATA, args_brightness, 6, 5, true);
	runCommand(VIDEO_WRITE_DATA, args_contrast, 6, 5, true);
	runCommand(VIDEO_WRITE_DATA, args_saturation, 6, 5, true);
	return true;
}

bool Video::colorControl(int mode) {
	int args[] = {0x02, 0x01, mode};

	if (mode == 0) {
		return runCommand(VIDEO_COL_CTRL, args, 3, 5, true);
	}
	else if (mode == 1) {
		return runCommand(VIDEO_COL_CTRL, args, 3, 5, true);
	}
	else {
		return runCommand(VIDEO_COL_CTRL, args, 3, 5, true);
	}
}

bool Video::mirror_mode_on(void) {
	int args[] = {0x02, 0x01, 0x01};

	return runCommand(VIDEO_MIRROR_CTRL, args, 3, 5, true);
}

bool Video::reset() {
	int args[] = {0x0};

	return runCommand(VIDEO_RESET, args, 1, 5, true);
}

bool Video::motionDetected() {
	if (readResponse(4) != 4) {
		return false;
	}
	if (! verifyResponse(VIDEO_COMM_MOTION_DETECTED))
		return false;

	return true;
}


bool Video::setMotionStatus(int x, int d1, int d2) {
	int args[] = {0x03, x, d1, d2};

	return runCommand(VIDEO_MOTION_CTRL, args, sizeof(args), 5, true);
}


int Video::getMotionStatus(int x) {
	int args[] = {0x01, x};

	return runCommand(VIDEO_MOTION_STATUS, args, sizeof(args), 5, true);
}


bool Video::setMotionDetect(bool flag) {
	if (! setMotionStatus(VIDEO_MOTIONCONTROL,
				VIDEO_UARTMOTION, VIDEO_ACTIVATEMOTION))
		return false;

	int args[] = {0x01, flag};

	return runCommand(VIDEO_COMM_MOTION_CTRL, args, sizeof(args), 5, true);
}



bool Video::getMotionDetect(void) {
	int args[] = {0x0};

	if (! runCommand(VIDEO_COMM_MOTION_STATUS, args, 1, 6, true))
		return false;

	return camerabuff[5];
}

int Video::getImageSize() {
	int args[] = {0x4, 0x4, 0x1, 0x00, 0x19};
	if (! runCommand(VIDEO_READ_DATA, args, sizeof(args), 6, true))
		return -1;

	return camerabuff[5];
}

bool Video::setImageSize(int x) {
	int args[] = {0x05, 0x04, 0x01, 0x00, 0x19, x};

	return runCommand(VIDEO_WRITE_DATA, args, sizeof(args), 5, true);
}

/****************** downsize image control */

int Video::getDownsize(void) {
	int args[] = {0x0};
	if (! runCommand(VIDEO_DOWNSIZE_STATUS, args, 1, 6, true))
		return -1;

	return camerabuff[5];
}

bool Video::setDownsize(int newsize) {
	int args[] = {0x01, newsize};

	return runCommand(VIDEO_DOWNSIZE_CTRL, args, 2, 5, true);
}

/***************** other high level commands */

char * Video::getVersion(void) {
	int args[] = {0x01};

	sendCommand(VIDEO_GEN_VERSION, args, 1);
	// get reply
	if (!readResponse(VIDEO_CAMERABUFFSIZ))
		return 0;
	camerabuff[bufferLen] = 0;  // end it!
	return (char *)camerabuff;  // return it!
}


/***************** baud rate commands */

char* Video::setBaud9600() {
	int args[] = {0x03, 0x01, 0xAE, 0xC8};

	sendCommand(VIDEO_SET_PORT, args, sizeof(args));
	// get reply
	if (!readResponse(VIDEO_CAMERABUFFSIZ))
		return 0;
	camerabuff[bufferLen] = 0;  // end it!
	return (char *)camerabuff;  // return it!

}

char* Video::setBaud19200() {
	int args[] = {0x03, 0x01, 0x56, 0xE4};

	sendCommand(VIDEO_SET_PORT, args, sizeof(args));
	// get reply
	if (!readResponse(VIDEO_CAMERABUFFSIZ))
		return 0;
	camerabuff[bufferLen] = 0;  // end it!
	return (char *)camerabuff;  // return it!
}

char* Video::setBaud38400() {
	int args[] = {0x03, 0x01, 0x2A, 0xF2};

	sendCommand(VIDEO_SET_PORT, args, sizeof(args));
	// get reply
	if (!readResponse(VIDEO_CAMERABUFFSIZ))
		return 0;
	camerabuff[bufferLen] = 0;  // end it!
	return (char *)camerabuff;  // return it!
}

char* Video::setBaud57600() {
	int args[] = {0x03, 0x01, 0x1C, 0x1C};

	sendCommand(VIDEO_SET_PORT, args, sizeof(args));
	// get reply
	if (!readResponse(VIDEO_CAMERABUFFSIZ))
		return 0;
	camerabuff[bufferLen] = 0;  // end it!
	return (char *)camerabuff;  // return it!
}

char* Video::setBaud115200() {
	int args[] = {0x03, 0x01, 0x0D, 0xA6};

	sendCommand(VIDEO_SET_PORT, args, sizeof(args));
	// get reply
	if (!readResponse(VIDEO_CAMERABUFFSIZ))
		return 0;
	camerabuff[bufferLen] = 0;  // end it!
	return (char *)camerabuff;  // return it!
}

/****************** high level photo comamnds */

bool Video::setCompression(int c) {
	int args[] = {0x5, 0x1, 0x1, 0x12, 0x04, c};
	return runCommand(VIDEO_WRITE_DATA, args, sizeof(args), 5, true);
}

int Video::getCompression(void) {
	int args[] = {0x4, 0x1, 0x1, 0x12, 0x04};
	runCommand(VIDEO_READ_DATA, args, sizeof(args), 6, true);
	printBuff();
	return camerabuff[5];
}

bool Video::takePicture() {
	frameptr = 0;
	return cameraFrameBuffCtrl(VIDEO_STOPCURRENTFRAME);
}

bool Video::resumeVideo() {
	return cameraFrameBuffCtrl(VIDEO_RESUMEFRAME);
}

bool Video::TVon() {
	int args[] = {0x1, 0x1};
	return runCommand(VIDEO_TVOUT_CTRL, args, 2, 5, true);
}
bool Video::TVoff() {
	int args[] = {0x1, 0x0};
	return runCommand(VIDEO_TVOUT_CTRL, args, 2, 5, true);
}

bool Video::cameraFrameBuffCtrl(int command) {
	int args[] = {0x1, command};
	return runCommand(VIDEO_FBUF_CTRL, args, sizeof(args), 5, true);
}

int Video::frameLength(void) {
	int args[] = {0x01, 0x00};

	if (!runCommand(VIDEO_GET_FBUF_LEN, args, sizeof(args), 9, true))
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


int Video::available(void) {
	return bufferLen;
}


int* Video::readPicture(int n) {
	int args[] = {0x0C, 0x0, 0x0A,
		0, 0, frameptr >> 8, frameptr & 0xFF,
		0, 0, 0, n,
		VIDEO_CAMERADELAY >> 8, VIDEO_CAMERADELAY & 0xFF};

	if (! runCommand(VIDEO_READ_FBUF, args, sizeof(args), 5, false))
		return 0;


	// read into the buffer PACKETLEN!
	if (readResponse(n+5) == 0)
		return 0;


	frameptr += n;

	return camerabuff;
}

uint32_t Video::readPixel(unsigned x, unsigned y){
	int addr = (y * 240 + x);//will fix this later

		// Ignore invalid writes
		if (addr < 320*240)
			return IORD_32DIRECT(addr, 0);

		return 0;
}

/**************** low level commands */


bool Video::runCommand(int cmd, int *args, int argn,
		int resplen, bool flushflag) {

	sendCommand(cmd, args, argn);
	if (readResponse(resplen) != resplen)
		return false;
	if (! verifyResponse(cmd))
		return false;
	return true;
}

void Video::sendCommand(int cmd, int args[], int argn) {
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

int Video::readResponse(int numbytes) {
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

bool Video::verifyResponse(int command) {
	if ((camerabuff[0] != 0x76) ||
			(camerabuff[1] != serialNum) ||
			(camerabuff[2] != command) ||
			(camerabuff[3] != 0x0))
		return false;
	return true;

}

void Video::printBuff() {
	int i;
	for (i = 0; i< bufferLen; i++) {
		printf(" 0x");
		printf("%x", camerabuff[i]);
	}
	printf("\n");
}

