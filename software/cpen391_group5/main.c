//
//  main.c
//  PixcelDetection
//
//  Created by Chi Liu on 2018/1/28.
//  Copyright © 2018年 Chi Liu. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "red_test.h"
#include "system.h"

#define MAX_X 320
#define MAX_Y 240
#define NUM_SLOTS 4
#define RANGE 50

struct point{
    unsigned x;
    unsigned y;
    unsigned count;
};

void algorithm(void);

int main(int argc, const char * argv[]) {
    
    algorithm();
    
    return 0;
}

void algorithm(void) {
    unsigned char *data = (unsigned char *) header_data;
    unsigned char pixel[3];
    
    struct point slots[NUM_SLOTS];
    
    for(int i = 0; i < NUM_SLOTS; i++){
        slots[i].count = 0;
    }
    
    
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++){
            
            HEADER_PIXEL(data, pixel);
            
            // relatively red
            if(pixel[0] >= 200 && pixel[1] < 50 && pixel[2] < 50){
                printf("Red Pixel at %u %u\n", x, y);
                int free_slot = -1;
                bool found_slot = false;
                for(int i = 0; i < NUM_SLOTS; i++){
                   
                    if(slots[i].count > 0 ){
                        // compare with specified range
                        double distance = absDistance(slots[i].x, x, slots[i].y, y);
                        
                        if(distance < RANGE){
                            
                            slots[i].x = average2Points(slots[i].x, x);
                            slots[i].y = average2Points(slots[i].y, y);
                            slots[i].count += 1;
                            //printf("%d\n",slots[i].count);
                            found_slot = true;
                        }
                        
                    } else{
                        free_slot = i;
                    }
                }
                
                // if the pixel doesnt get a slot
                if(!found_slot){
                    assert(free_slot != -1);
                    
                    slots[free_slot].x = x;
                    slots[free_slot].y = y;
                    slots[free_slot].count += 1;
                }
                
                // if
                
                
                
            }
        }
        
        
    }
    for(int i = 0; i < SLOT_SIZE; i++){
        
        printf("Slot %d: x = %u, y = %u, count= %d.\n", i, slots[i].x, slots[i].y, slots[i].count);
        
    }
    
    
    
}


