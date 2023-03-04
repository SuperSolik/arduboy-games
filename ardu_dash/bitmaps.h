#pragma once

const uint8_t PROGMEM player_sprite[] = {
    16, 16,
    0x00, 0x00, 0xfc, 0xfc, 0x0c, 0x0c, 0x0c, 0x8c, 0x8c, 0x0c, 0x0c, 0x0c, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x30, 0x30, 0x30, 0x31, 0x31, 0x30, 0x30, 0x30, 0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xfc, 0x1e, 0x06, 0x06, 0x86, 0x8c, 0x0c, 0x1c, 0x18, 0xb8, 0xf0, 0x70, 0x00, 0x04, 0x0f, 0x0f, 0x18, 0x18, 0x18, 0x30, 0x31, 0x31, 0x60, 0x70, 0x7e, 0x1f, 0x03, 0x00, 0x00, 0x00, 0x80, 0xe0, 0x78, 0x3c, 0x0f, 0x07, 0x86, 0x8e, 0x0c, 0x1c, 0x18, 0x30, 0xf0, 0xe0, 0x40, 0x06, 0x07, 0x0f, 0x0c, 0x18, 0x18, 0x30, 0x71, 0x61, 0xe0, 0x78, 0x1e, 0x0f, 0x03, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0x70, 0x38, 0x1c, 0x0e, 0x87, 0x87, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xc0, 0x80, 0x01, 0x03, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe1, 0xe1, 0x70, 0x38, 0x1c, 0x0e, 0x07, 0x03, 0x01, 0x60, 0xe0, 0xf0, 0x38, 0x18, 0x1c, 0x0c, 0x86, 0x86, 0x07, 0x0f, 0x3c, 0xf0, 0xc0, 0x80, 0x00, 0x00, 0x01, 0x07, 0x0f, 0x3c, 0x70, 0xe0, 0x61, 0x31, 0x38, 0x18, 0x1c, 0x0c, 0x07, 0x07, 0x02, 0x00, 0x78, 0xf8, 0x98, 0x18, 0x0c, 0x0c, 0x8c, 0x8e, 0x06, 0x0e, 0x3e, 0xfc, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x07, 0x7f, 0x78, 0x70, 0x30, 0x31, 0x31, 0x30, 0x18, 0x18, 0x1f, 0x1f, 0x0c, 0x00, 
};

const uint8_t PROGMEM player_death_sprite[] = {
    16, 16,
    0x00, 0x00, 0x00, 0xf0, 0xf8, 0x18, 0x18, 0x98, 0x98, 0x18, 0x18, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x18, 0x18, 0x18, 0x19, 0x18, 0x18, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x20, 0xa0, 0x20, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x04, 0x04, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xc0, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x08, 0x08, 0x09, 0x00, 0x00, 0x07, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x00,
};


const uint8_t PROGMEM obstacles_sprites[] = {
    16, 16,
    0x12, 0x6d, 0x6d, 0x33, 0x33, 0x2d, 0x2d, 0x73, 0x73, 0x2d, 0x2d, 0x33, 0x33, 0x6d, 0x6d, 0x12, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xfe, 0x03, 0x05, 0x09, 0x11, 0x21, 0x41, 0x81, 0x81, 0x41, 0x21, 0x11, 0x09, 0x05, 0x03, 0xfe, 
    0x7f, 0xc0, 0xa0, 0x90, 0x88, 0x84, 0x82, 0x81, 0x81, 0x82, 0x84, 0x88, 0x90, 0xa0, 0xc0, 0x7f, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x48, 0x93, 0xfc, 0x02, 0xfb, 0xfa, 0x02, 0xfd, 0x49, 0x90, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x92, 0x24, 0xfc, 0x03, 0xfa, 0xfa, 0x03, 0xfc, 0x24, 0x48, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x24, 0x48, 0xfd, 0x02, 0xfa, 0xfb, 0x02, 0xfc, 0x92, 0x24, 0x00, 0x00, 0x00, 
};


// ardu_dash_game_logo_77x32.png  width: 77 height: 32
const PROGMEM uint8_t compressed_logo[] = {
    0x4c,0x1f,0x71,0xe4,0x9c,0x73,0xce,0x39,0xe7,0x9c,0x73,0xce,0x39,0x27,0x9d,0xd3,
    0x5b,0x25,0xa5,0xe2,0x76,0xdb,0x74,0x5c,0xca,0x39,0xe7,0x9c,0x73,0xce,0x39,0xe7,
    0x9c,0x93,0x4e,0xa7,0xb7,0xce,0x71,0xbb,0x6d,0x3a,0xae,0xa5,0x92,0x4a,0x2a,0xa9,
    0xa4,0x92,0x4a,0x2a,0x7b,0xc9,0x4a,0xd6,0xce,0x4e,0x4d,0x9b,0xa6,0x4b,0xa7,0xd3,
    0xe9,0x74,0x44,0xb4,0x91,0xa5,0xd3,0xe9,0x92,0x95,0xac,0x64,0x25,0x2b,0x59,0xc9,
    0x4a,0x56,0xb2,0x92,0x95,0x3b,0xbd,0x5d,0x2a,0xc7,0x6d,0xd3,0xe9,0x74,0xb9,0xed,
    0x76,0xbb,0xdd,0x6e,0xb7,0xdb,0xb7,0xe9,0x74,0xba,0x78,0x53,0xff,0xe7,0x3a,0xd7,
    0xb9,0xcb,0x39,0xe7,0x9c,0x73,0xce,0x39,0xe7,0x9c,0x73,0x97,0x4e,0xa7,0x2b,0xc9,
    0x6c,0xd3,0xe9,0x74,0xca,0x2a,0xa9,0xa4,0x92,0x4a,0x2a,0xa9,0xe4,0xd6,0xa6,0xd3,
    0xe9,0x2a,0x9d,0x4e,0xa7,0xd3,0xe9,0x74,0x3a,0x9d,0x4e,0xa7,0x94,0xab,0xac,0xe2,
    0x72,0xae,0x92,0x17,0x97,0x73,0xb9,0x94,0x2a,0xdd,0xae,0x98,0x1c,0x97,0x73,0x95,
    0xad,0xb8,0x9c,0x4b,0xbb,0xd2,0xbd,0x4a,0xa5,0xda,0x21,0x4b,0xb9,0xea,0xae,0x4e,
    0xb7,0xab,0x73,0x29,0x55,0xba,0xeb,0xd2,0x9d,0x76,0x2e,0xba,0x9a,0xcb,0xb9,0xb4,
    0x2b,0xdd,0xce,0xe5,0x15,0x02,0xd3,0x61,0x39,0xba,0xe8,0xa2,0x8b,0x2e,0x1d,0x7d,
    0x3a,0xba,0x74,0xb4,0xe9,0x74,0x74,0xe9,0x74,0x68,0x4b,0x47,0x97,0x4e,0x47,0x17,
    0x6d,0xb4,0xd1,0xa5,0xd3,0xd1,0x45,0x17,0x7a,0x01
};
// bytes:250 ratio: 0.812


// press_a_to_play_new_58x8.png  width: 58 height: 8
const PROGMEM uint8_t press_a_to_play[] = {
    0x39,0x07,0x88,0xa4,0xaa,0xe4,0x6c,0x52,0x55,0xf2,0xe6,0x52,0xae,0x2a,0x55,0x15,
    0x57,0xa7,0xaa,0x52,0x1d,0x59,0xa7,0xaa,0x52,0x1d,0x8e,0xe4,0x9d,0xeb,0xa4,0x44,
    0x51,0x5a,0xaa,0xe8,0xa4,0x4a,0xa5,0x24,0xb5,0x54,0x95,0x9c,0x4d,0xea,0x74,0x5c,
    0x4e,0x55,0xd6,0x51,0x7d,0xae,0x53,0x92,0x09
};
// bytes:57 ratio: 0.983