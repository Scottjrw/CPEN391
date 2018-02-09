#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "io.h"
#include "SimpleGraphics.hpp"

#define BUTTON_WIDTH	40
#define BUTTON_HEIGHT	15
#define FONT_WIDTH		5
#define FONT_HEIGHT		7
#define FONT_SPACE		1


class UI_Manager
{
public:
	UI_Manager();
	~UI_Manager();
	
};


class Buttons
{

public:

	Buttons(char header[]){
		_header = header;
		BUTTON_COLOR	= rgba(255, 255, 255, 255);
		FONT_COLOR		= rgba(0, 0, 0, 255);
	}

	~Buttons();

	void draw_Button(unsigned x1, unsigned y1, char header[], rgba_t fontcolor, rgba_t backgroundcolor){

		draw_rect(backgroundcolor, x1, y1, BUTTON_WIDTH, BUTTON_HEIGHT);

		int header_Length = FONT_WIDTH * strlen(header) + FONT_SPACE * (strlen(header) - 1);

		for(int i = 0; header[i] != '\0'; ++i.){

			display_text( x1 + 10 + , y1 + 4, fontcolor, backgroundcolor, header[i], 0);
		}
	}


private:
	char _header[];
	rgba_t BUTTON_COLOR;
	rgba_t FONT_COLOR;
};

class Panel:
{
public:
	Panel();
	~Panel();
	
};

class Dropdown_Menu:
{
public:
	Dropdown_Menu();
	~Dropdown_Menu();
	
};

class Text_Field
{
public:
	Text_Field();
	~Text_Field();
	
};