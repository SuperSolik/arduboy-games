#pragma once

constexpr uint8_t SEGMENT_H = 4;
constexpr uint8_t SEGMENT_W = 10;
constexpr uint8_t SEGMENT_CNT = 15;

enum class SegmentLoadOrientation : uint8_t {
    HORIZONTAL,
    VERTICAL,
};

// TODO: figure out how to pack segment obstacles more optimal
struct Segment {
    uint8_t start_height;
    uint8_t end_height;
    uint8_t obstacle_cnt;
    SegmentLoadOrientation load_orientation;

    const char map[SEGMENT_H][SEGMENT_W];
};

const Segment SEGMENTS[SEGMENT_CNT] {
    {
        .start_height = 0,
        .end_height = 0,
        .obstacle_cnt = 2,
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
        .obstacle_cnt = 6,
        .load_orientation = SegmentLoadOrientation::VERTICAL,
        .map =  {
            " x  x  x ",
            "         ",
            "         ",
            " x  x  x "
        }
    },
    {
        .start_height = 0,
        .end_height = 0,
        .obstacle_cnt = 5,
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
        .obstacle_cnt = 2,
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
        .obstacle_cnt = 3,
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
        .obstacle_cnt = 3,
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
        .obstacle_cnt = 4,
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
        .obstacle_cnt = 1,
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
        .obstacle_cnt = 5,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "        x",
            "  x   xx ",
            "xx  xxx  "
        }
    },
    {
        .start_height = 1,
        .end_height = 1,
        .obstacle_cnt = 3,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "         ",
            "  xxxxx  ",
            "xx     xx"
        }
    },
    {
        .start_height = 2,
        .end_height = 3,
        .obstacle_cnt = 4,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "       xx",
            "xx x xx  ",
            "         "
        }
    },
    {
        .start_height = 1,
        .end_height = 1,
        .obstacle_cnt = 2,
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
        .obstacle_cnt = 4,
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
        .obstacle_cnt = 4,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "xxxxxxxxx",
            " xx      ",
            "     xxxx",
            "xxxxxxxxx",
        }
    },
    {
        .start_height = 3,
        .end_height = 3,
        .obstacle_cnt = 5,
        .load_orientation = SegmentLoadOrientation::HORIZONTAL,
        .map =  {
            "         ",
            "x      xx",
            "x  xx    ",
            "x        ",
        }
    }
};


// TODO: create tree for segment indices
// what segments can be loaded after current segment
// uint8_t segments_load_path[SEGMENTS_CNT][3]; 
// 3 options where to go from current segment