#pragma once

#include "obstacles.h"
#include "game_global_constants.h"
#include "map_segments.h"


int8_t parse_vertical_segment_map_to_obstacles(Obstacle obstacles[SEGMENT_W], const char segment_map[SEGMENT_H][SEGMENT_W]) {
    int8_t start = 0;
    int8_t height;
    uint8_t created_cnt = 0;

    for (uint8_t x = 0; x < SEGMENT_W; x++) {
        start = -1;
        height = 0;
        
        for (int8_t y = SEGMENT_H - 1; y >= 0; y--) {
            if ((char)pgm_read_byte(&segment_map[y][x]) == 'x') {
                if (start == -1) {
                    // start of the obstacle
                    start = y;
                    height++;
                } else {
                    // inside obstacle
                    height++;
                }
            } else {
                if (start != -1) {
                    if (created_cnt >= SEGMENT_W) return -1; // SHOULD NEVER HAPPEN, number of obstacles bigger than we can create
                    
                    // create obstacle
                    // TODO: refactor in a separate function?
                    obstacles[created_cnt].bounds.width = 1 * OBJECT_SIZE;
                    obstacles[created_cnt].bounds.height = height * OBJECT_SIZE;
                    obstacles[created_cnt].bounds.x = x * OBJECT_SIZE;
                    obstacles[created_cnt].bounds.y =  (-y + SEGMENT_H - 1) * OBJECT_SIZE; 

                    created_cnt++;

                    // reset
                    start = -1;
                    height = 0;
                }
            }
        }
    }

    return created_cnt;
}

int8_t parse_horizontal_segment_map_to_obstacles(Obstacle obstacles[SEGMENT_W], const char segment_map[SEGMENT_H][SEGMENT_W]) {
    int8_t start = 0;
    int8_t width;
    uint8_t created_cnt = 0;

    for (int8_t y = SEGMENT_H - 1; y >= 0; y--) {
        start = -1;
        width = 0;
        
        for (uint8_t x = 0; x < SEGMENT_W; x++) {
            if ((char)pgm_read_byte(&segment_map[y][x]) == 'x') {
                if (start == -1) {
                    // start of the obstacle
                    start = x;
                    width++;
                } else {
                    // inside obstacle
                    width++;
                }
            } else {
                if (start != -1) {
                    if (created_cnt >= SEGMENT_W) return -1; // SHOULD NEVER HAPPEN, number of obstacles bigger than we can create
                    
                    // create obstacle
                    obstacles[created_cnt].bounds.width = width * OBJECT_SIZE;
                    obstacles[created_cnt].bounds.height = 1 * OBJECT_SIZE;
                    obstacles[created_cnt].bounds.x = start * OBJECT_SIZE;
                    obstacles[created_cnt].bounds.y =  (-y + SEGMENT_H) * OBJECT_SIZE; 

                    created_cnt++;

                    // reset
                    start = -1;
                    width = 0;
                }
            }
        }
    }

    return created_cnt;
}

int8_t parse_segment_to_obstacles(Obstacle obstacles[SEGMENT_W], const Segment& seg) {
    if (seg.load_orientation == SegmentLoadOrientation::HORIZONTAL){
        return parse_horizontal_segment_map_to_obstacles(obstacles, SEGMENT_MAPS[seg.map_idx]);
    } else {
        return parse_vertical_segment_map_to_obstacles(obstacles, SEGMENT_MAPS[seg.map_idx]);
    }
}

uint8_t segment_from_heigth(uint8_t height) {
    uint8_t picked_idx = random(0, SEGMENT_CNT);
    return picked_idx;
}