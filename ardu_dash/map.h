#pragma once

#include "obstacles.h"

#define SEGMENT_H 4
#define SEGMENT_W 10


struct Segment {
    uint8_t start_height;
    uint8_t end_height;

    const char map[SEGMENT_H][SEGMENT_W];
};

Segment segments[10] {
    {
        .start_height = 0,
        .end_height = 0,
        .map =  {
            "         ",
            "         ",
            "         ",
            "    x    "
        }
    },
    {
        .start_height = 0,
        .end_height = 0,
        .map =  {
            "         ",
            "         ",
            "    xx   ",
            "  x      "
        }
    },
    {
        .start_height = 0,
        .end_height = 3,
        .map =  {
            "         ",
            "      xxx",
            "   xxxx  ",
            " x       "
        }
    },
    {
        .start_height = 3,
        .end_height = 0,
        .map =  {
            "         ",
            "x  xx    ",
            "x  xx  x ",
            "x  xx    "
        }
    },
    {
        .start_height = 3,
        .end_height = 1,
        .map =  {
            "         ",
            "x  xxx   ",
            "     xxx ",
            "       xx"
        }
    }
};

constexpr uint8_t SEGMENTS_BY_LEVEL[3][10] {
    {0},
    {0},
    {0}
};

int8_t parse_segment_map_to_obstacles(Obstacle obstacles[SEGMENT_W], const char segment_map[SEGMENT_H][SEGMENT_W]) {
    int8_t init_offset = 0; 
    int8_t  start_y = 0;
    int8_t height;
    uint8_t created_cnt = 0;

    for (uint8_t x = 0; x < SEGMENT_W; x++) {
        init_offset += 1;
        start_y = -1;
        height = 0;
        
        for (int8_t y = SEGMENT_H - 1; y >= 0; y--) {
            if (segment_map[y][x] == 'x') {
                if (start_y == -1) {
                    // start of the obstacle
                    start_y = y;
                    ++height;
                } else {
                    // inside obstacle
                    ++height;
                }
            } else {
                if (start_y != -1) {
                    if (created_cnt >= SEGMENT_W) return -1; // SHOULD NEVER HAPPEN, number of obstacles bigger than we can create
                    
                    // create obstacle
                    // TODO: replace 16 with object size
                    obstacles[created_cnt].bounds.width = 1 * 16;
                    obstacles[created_cnt].bounds.height = height * 16;
                    obstacles[created_cnt].bounds.x = x * 16;
                    obstacles[created_cnt].bounds.y =  (-y + SEGMENT_H - 1) * 16; 

                    created_cnt++;

                    // reset
                    start_y = -1;
                    height = 0;
                }
            }
        }
    }

    return created_cnt;
}