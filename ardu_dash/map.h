#pragma once

#include "obstacles.h"
#include "game_global_constants.h"

#define SEGMENT_H 4
#define SEGMENT_W 10
#define SEGMENT_CNT 15

enum class SegmentLoadOrientation : uint8_t {
    HORIZONTAL,
    VERTICAL,
};

struct Segment {
    uint8_t start_height;
    uint8_t end_height;
    SegmentLoadOrientation load_orientation;

    const char map[SEGMENT_H][SEGMENT_W];
};

Segment segments[SEGMENT_CNT] {
    {
        .start_height = 0,
        .end_height = 0,
        .load_orientation = SegmentLoadOrientation::VERTICAL,
        .map =  {
            "         ",
            "         ",
            "         ",
            "  x   x  "
        }
    },
    {
        .start_height = 0,
        .end_height = 0,
        .load_orientation = SegmentLoadOrientation::VERTICAL,
        .map =  {
            "         ",
            " x  x  x ",
            "         ",
            " x  x  x "
        }
    },
    {
        .start_height = 0,
        .end_height = 0,
        .load_orientation = SegmentLoadOrientation::VERTICAL,
        .map =  {
            "         ",
            "  x   x  ",
            "         ",
            " x  x  x "
        }
    },
    {
        .start_height = 0,
        .end_height = 0,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
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
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
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
        .load_orientation = SegmentLoadOrientation::VERTICAL,
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
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "x  xxx   ",
            "     xxx ",
            "       xx"
        }
    },
    {
        .start_height = 2,
        .end_height = 2,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "         ",
            "xxxxxxxxx",
            "         "
        }
    },
    {
        .start_height = 1,
        .end_height = 3,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "        x",
            " xx   xx ",
            "x  xxx   "
        }
    },
    {
        .start_height = 1,
        .end_height = 1,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "    xx   ",
            "  xx  xx ",
            "xx      x"
        }
    },
    {
        .start_height = 2,
        .end_height = 3,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "     x  x",
            "xx x  xx ",
            "         "
        }
    },
    {
        .start_height = 1,
        .end_height = 1,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "xxxxxxxxx",
            "         ",
            "         ",
            "xxxxxxxxx",
        }
    },
    {
        .start_height = 1,
        .end_height = 1,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "xxxxxxxxx",
            "      x  ",
            "   x     ",
            "xxxxxxxxx",
        }
    },
    {
        .start_height = 1,
        .end_height = 2,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "xxxxxxxxx",
            "xxx      ",
            "     xxxx",
            "xxxxxxxxx",
        }
    },
    {
        .start_height = 3,
        .end_height = 3,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "x      xx",
            "x  xx    ",
            "x        ",
        }
    }
};

// constexpr uint8_t SEGMENTS_BY_LEVEL[3][10] {
//     {0},
//     {0},
//     {0}
// };


// TODO take into account load orientation
int8_t parse_vertical_segment_map_to_obstacles(Obstacle obstacles[SEGMENT_W], const char segment_map[SEGMENT_H][SEGMENT_W]) {
    int8_t start = 0;
    int8_t height;
    uint8_t created_cnt = 0;

    for (uint8_t x = 0; x < SEGMENT_W; x++) {
        start = -1;
        height = 0;
        
        for (int8_t y = SEGMENT_H - 1; y >= 0; y--) {
            if (segment_map[y][x] == 'x') {
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
                    // TODO: replace 16 with object size
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
            if (segment_map[y][x] == 'x') {
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
                    // TODO: replace 16 with object size
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
        return parse_horizontal_segment_map_to_obstacles(obstacles, seg.map);
    } else {
        return parse_vertical_segment_map_to_obstacles(obstacles, seg.map);
    }
}