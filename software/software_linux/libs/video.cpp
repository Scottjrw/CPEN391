/*
 */
#include "video.hpp"
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <errno.h>
#include <TermiosUtil.hpp>
#include <iomanip>


Video::Video(const char* file) {
	video_uart = open(file, O_RDWR);
	serialNum = 0x00;
	frameptr  = 0;
	bufferLen = 0;

	if (video_uart < 0) {
		throw std::system_error(errno, std::system_category(), "fail to open uart");
	}

	TermiosUtil::SetSpeed(video_uart, B38K);//set baudrate

	const uint32_t hpsfpga_bridge = 0xC0000000;
    //const uint32_t lw_bridge = 0xFF200000;
    const uint32_t base_addr = hpsfpga_bridge;
	unsigned size = 320 * 240 * 4;

    int fd = open("/dev/mem", (O_RDWR | O_SYNC));
    if (fd == -1) {
        throw std::system_error(errno, std::system_category(), "fail to mmap video");
    }

    base = reinterpret_cast<uint32_t *> (mmap(NULL, size, (PROT_READ|PROT_WRITE), MAP_SHARED, fd, base_addr));
    if (base == MAP_FAILED) {
        throw "fail to mmap video";
    }
    printf("camera memory initialized\n");
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

/****************** high level photo comamnds */

bool Video::TVon() {
	int args[] = {0x1, 0x1};
	return runCommand(VIDEO_TVOUT_CTRL, args, 2, 5, true);
}
bool Video::TVoff() {
	int args[] = {0x1, 0x0};
	return runCommand(VIDEO_TVOUT_CTRL, args, 2, 5, true);
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

void Video::sendCommand(int cmd, int args[], int command_length) {
	int i;

	char string_out[50];
	string_out[0] = 0x56;
	string_out[1] = serialNum;
	string_out[2] = cmd;

	for (i = 0; i < command_length; i++) {
		string_out[3+i] = args[i];
	}

	if(write(video_uart, string_out, command_length+3)<0)
		printf("video write failed\n");
}

int Video::readResponse(int numbytes) {
	bufferLen = 0;
	int i;
	FILE * f = fdopen(video_uart, "r+");

	for(i = 0; i < numbytes; i++) {
		camerabuff[bufferLen++] = fgetc(f);
	}

	//  	for (i = 0; i < 5; i++) {
	//  		printf("%i ", camerabuff[i]);
	//  	}
	//  	printf("\n");

	fclose(f);
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

std::string Video::takeRawPicture(int startX, int startY){
	volatile uint32_t * data;
	std::stringstream ss;
	std::string output;

	// hard coded the location (upper middle) to capture
	for(int j=startY; j<startY+60; j++){
		for(int i=startX; i<startX+80; i++){
			data = base + (j * width + i);
			ss << std::setw(6) << std::setfill('0') << std::hex <<*data;
		}
	}
	output = ss.str();
	printf("string done, size is %u\n", output.length());
	return output;
}

