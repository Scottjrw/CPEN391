//
//  main.c
//  PixcelDetection
//
//  Created by Chi Liu on 2018/1/28.
//  Copyright © 2018年 Chi Liu. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "Red Test.h"

#define MAX_X 320
#define MAX_Y 240
#define SLOT_SIZE 4
#define RANGE 50

double absDistance(double x1, double x2, double y1, double y2);
double average2Points(double p1, double p2);
void main_loop();

struct point{
    
    unsigned x;
    unsigned y;
    int count;
};

int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    main_loop();
    
    return 0;
}

void main_loop(){
    unsigned char *data = header_data;
    unsigned char pixel[3];
    
    struct point slots[SLOT_SIZE];
    
    for(int i = 0; i < SLOT_SIZE; i++){
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
                for(int i = 0; i < SLOT_SIZE; i++){
                   
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

//bool relativeRed(unsigned char pixel[3]){
    
    //return pixel[0] >= 200 && pixel[1] < 50 && pixel[2] < 50;
//}


double absDistance(double x1, double x2, double y1, double y2){
    double distance = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    return distance;
}

double average2Points(double p1, double p2){
    return (p1+p2)/2;
}
