#pragma once

constexpr uint8_t SEGMENT_H = 4;
constexpr uint8_t SEGMENT_W = 10;
constexpr uint8_t SEGMENT_CNT = 4;

enum class SegmentLoadOrientation : uint8_t {
    HORIZONTAL,
    VERTICAL,
};

const char PROGMEM SEGMENT_MAPS[SEGMENT_CNT][SEGMENT_H][SEGMENT_W] = {
    {
        "        ",
        "        ",
        "    x  x",
        "  x x^^x "
    },
    {
        "         ",
        "      -- ",
        "    x    ",
        " xxxxxxx "
    },
    {
        " xxxxxxx ",
        "         ",
        "         ",
        " xxx^^xx "
    },
    {
        "         ",
        "     --  ",
        "   x     ",
        " xx    xx"
    },
};

struct Segment {
    uint8_t start_height;
    uint8_t end_height;
    uint8_t obstacle_cnt;
    uint8_t map_idx;
    SegmentLoadOrientation load_orientation;
};

const Segment SEGMENTS[SEGMENT_CNT] {
    {
        .start_height = 0,
        .end_height = 0,
        .obstacle_cnt = 5,
        .map_idx = 0,
        .load_orientation = SegmentLoadOrientation::VERTICAL,
    },
    {
        .start_height = 0,
        .end_height = 0,
        .obstacle_cnt = 3,
        .map_idx = 1,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
    },
    {
        .start_height = 0,
        .end_height = 0,
        .obstacle_cnt = 4,
        .map_idx = 2,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
    },
    {
        .start_height = 0,
        .end_height = 1,
        .obstacle_cnt = 4,
        .map_idx = 3,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
    }
};


// TODO: create tree for segment indices
// what segments can be loaded after current segment
// uint8_t segments_load_path[SEGMENTS_CNT][3]; 
// 3 options where to go from current segment