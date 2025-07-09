#pragma once
#include "Headers.h"

class PyMissionMap {
public:
    bool window_open = false;
    uint32_t frame_id = 0;
    float left = 0;
    float top = 0;
    float right = 0;
    float bottom = 0;
    float scale_x = 0;
    float scale_y = 0;
    float zoom = 0;
    float center_x = 0;
    float center_y = 0;
    float last_click_x = 0;
    float last_click_y = 0;
    float pan_offset_x = 0;
    float pan_offset_y = 0;
    float mission_map_screen_center_x = 0;
    float mission_map_screen_center_y = 0;



    PyMissionMap() { GetContext(); }
    void GetContext();


};



