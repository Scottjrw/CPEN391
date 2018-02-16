/*
 * vision_updated.hpp
 *
 *  Created on: Feb 13, 2018
 *      Author: Daniel
 */

#ifndef VISION_UPDATED_HPP_
#define VISION_UPDATED_HPP_

#include <functional>
#include <string>
#include <vector>
#include <exception>
#include <stdexcept>
#include <utility>
#include <array>
#include "cursor.hpp"


struct rgba {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} __attribute__((packed));

struct point {
	unsigned x;
	unsigned y;
	unsigned count;
};

typedef enum {
	NO_MOVE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	UP_LEFT,
	UP_RIGHT,
	DOWN_LEFT,
	DOWN_RIGHT
} Direction;

class Gesture_Recognizer {
public:

	typedef std::function<void()> GestureCB;

	Gesture_Recognizer(void);

	void change_gesture_map(Direction dir, GestureCB callback);


	/*
	 * Store Dot from hardware
	 */
	void update_dots(point red_dot);

private:
    static constexpr unsigned DOTS_MAX_X = 320;
    static constexpr unsigned DOTS_MAX_Y = 240;
    static constexpr unsigned DOTS_NUM_SLOTS = 4;
    static constexpr unsigned DOTS_RANGE = 50;
    static constexpr unsigned DOTS_MARKER_SIZE = 5;
    static constexpr unsigned GESTURE_JITTER_RANGE = 20;
    static constexpr unsigned GESTURE_STOP_MS = 1000;
    static constexpr unsigned GESTURE_START_MAX = 0;
    static constexpr unsigned GESTURE_ERR_MAX = 10;
    static constexpr unsigned GESTURE_MAX_MOVE = 50;
    static constexpr unsigned MAX_GESTURES = 8;

	std::array<Direction, MAX_GESTURES> gestures;
	std::array<GestureCB, MAX_GESTURES> callbacks;
	point current_point;
	point previous_point;

	Direction last_dir = NO_MOVE;
	Direction cur_gesture = NO_MOVE;
	unsigned start_count = 0;
	unsigned err_count = 0;
	unsigned stop_begin_ticks = 0;

	enum {
			WAIT_START,
			GESTURE,
			WAIT_STOP
	} state = WAIT_START;

	struct point slots[DOTS_NUM_SLOTS];
	struct point prev_slots[DOTS_NUM_SLOTS];



    inline unsigned absDistanceSq(unsigned x1, unsigned x2, unsigned y1, unsigned y2) {
        return (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
    }

    inline unsigned average2Points(unsigned p1, unsigned p2) {
        return (p1+p2) / 2;
    }

	/*
	 * Gesture Detector
	 */
	void gesture_detect();

	/*
	 * Print a Gesture
	 */
	void print_gesture(Direction g);

	/*
	 * Get Direction
	 */
	Direction pointDirection(struct point *prev, struct point *next);



};




#endif /* VISION_UPDATED_HPP_ */
