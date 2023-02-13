#!/usr/bin/python3

import os
import glob
import json
import datetime

import sys

from typing import List, Dict


DEBUG = False

def log(s: str):
    if DEBUG:
        print(s)

MAP_WIDTH = 20
MAP_HEIGHT = 4
NEXT_LEVEL_OPTIONS_CNT = 3

def parse_tiled_map_file(json_map_path: str, indent: int = 2) -> Dict:
    filename_with_ext = os.path.basename(json_map_path)
    log(f"Parsing {filename_with_ext}")
    filename, _ = filename_with_ext.split('.')

    comment_line = f"// {filename_with_ext}\n"

    with open(json_map_path, 'r') as f:
        map_data = json.load(f)
        assert map_data["width"] == MAP_WIDTH, f"map width != {MAP_WIDTH}"
        assert map_data["height"] == MAP_HEIGHT, f"map width != {MAP_HEIGHT}"

        # links are ;-separated level names, that current level can lead to
        links_to_next_lvls = [p["value"] for p in map_data["properties"] if p["name"] == "links"][0]
        obstacle_count = [p["value"] for p in map_data["properties"] if p["name"] == "obstacle_count"][0]
        parse_vertical = [p["value"] for p in map_data["properties"] if p["name"] == "parse_vertical"][0]
        next_map_names = [l for l in links_to_next_lvls.split(';') if l]
        
        lvl_data = map_data["layers"][0]["data"]
        char_lvl_data = ',\n'.join(
            ' '*indent + '{' + ','.join(map(str, lvl_data[i:i+MAP_WIDTH])) + '}'
            for i in range(0, MAP_HEIGHT*MAP_WIDTH, MAP_WIDTH) 
        )

        char_lvl_data = "{{\n{}\n}}".format(char_lvl_data)
        char_lvl_data = comment_line + char_lvl_data
        return {
            "name": filename,
            "char_data": char_lvl_data,
            "links": next_map_names,
            "obstacle_count": obstacle_count,
            "parse_vertical": parse_vertical,
        }

def parse_tiled_maps_to_c_maps_header_file(maps_folder_path: str) -> str:
    folder_abs_path = os.path.abspath(maps_folder_path)

    maps_files = sorted(glob.glob(folder_abs_path + "/**/*.json", recursive=True))

    levels_content = ""
    links_content = ""
    segments_metadata_content = ""
    raw_parsed_data = [parse_tiled_map_file(map_file) for map_file in maps_files]

    name_to_index = {}

    for i, parsed_data in enumerate(raw_parsed_data):
        levels_content += parsed_data["char_data"] + ",\n"
        segments_metadata_content += '''
{{
    .obstacle_cnt = {},
    .load_orientation = {},
}},'''.format(
    parsed_data["obstacle_count"], 
    "SegmentLoadOrientation::VERTICAL" if parsed_data["parse_vertical"] else "SegmentLoadOrientation::HORIZONTAL"
    ).strip() + "\n"

        parsed_data["index"] = i
        name_to_index[parsed_data["name"]] = i

    for parsed_data in raw_parsed_data:
        links = [name_to_index[l] for l in parsed_data["links"]]
        remaining_len = NEXT_LEVEL_OPTIONS_CNT - len(links)
        links = links + [links[-1] for _ in range(remaining_len)]

        links_content += "{{{}, {}, {}}},\n".format(links[0], links[1], links[2])

    maps_parsed = len(raw_parsed_data)

    constants_definition = ''.join([
        f"// auto-generated by {os.path.basename(__file__)} at {datetime.datetime.now()}\n\n",
        "#pragma once\n\n",
        f"constexpr uint8_t SEGMENT_CNT = {maps_parsed};\n",
        f"constexpr uint8_t SEGMENT_W = {MAP_WIDTH};\n",
        f"constexpr uint8_t SEGMENT_H = {MAP_HEIGHT};\n",
        f"constexpr uint8_t NEXT_LEVEL_OPTIONS_CNT = {NEXT_LEVEL_OPTIONS_CNT};\n\n",
        """
enum class SegmentLoadOrientation : uint8_t {
    HORIZONTAL,
    VERTICAL,
};
        """.strip() + "\n\n",
        """
struct Segment {
    uint8_t obstacle_cnt;
    SegmentLoadOrientation load_orientation;
};
        """.strip() + "\n\n"
    ])
    array_close_bracket = "};\n"
    level_data_array_declaration = "const uint8_t PROGMEM SEGMENT_MAPS[SEGMENT_CNT][SEGMENT_H][SEGMENT_W] = {\n"
    segments_link_array_declaration = "const uint8_t SEGMENT_LINKS[SEGMENT_CNT][NEXT_LEVEL_OPTIONS_CNT] = {\n"
    segments_metadata_array_declaration = "const Segment SEGMENTS_METADATA[SEGMENT_CNT] = {\n"

    file_content = ''.join(
        [
            constants_definition,
            level_data_array_declaration,
            levels_content,
            array_close_bracket,
            "\n",
            segments_metadata_array_declaration,
            segments_metadata_content,
            array_close_bracket,
            "\n",
            segments_link_array_declaration,
            links_content,
            array_close_bracket,
        ]
    )

    return file_content, raw_parsed_data



if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("[ERROR] Path to folder with csv maps is not provided")
        exit(1)

    path_to_folder = sys.argv[1]

    # TODO: use argparse
    try:
        debug_mode = sys.argv[2]
        if (debug_mode == "debug"):
            DEBUG = True
    except IndexError:
        pass

    copy_paste_ready_char_maps, raw_parase_data = parse_tiled_maps_to_c_maps_header_file(path_to_folder)
    log("\n----COPY PASTE START----\n")
    print(copy_paste_ready_char_maps)
    log("\n----COPY PASTE END------\n")