#ifndef MAIN_H
#define MAIN_H

#include "SimpleGraphics.hpp"
#include "vision.hpp"

#define GRAPHICS_WIDTH 160
#define GRAPHICS_HEIGHT 120

// Shared graphics controller object
extern SimpleGraphics<uint16_t, GRAPHICS_WIDTH, GRAPHICS_HEIGHT, DOTS_MAX_X, DOTS_MAX_Y> graphics;

#endif
