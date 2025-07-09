#include "py_missionmap.h"


void PyMissionMap::GetContext() {
    const auto gameplay_context = GW::GetGameplayContext();
    const auto mission_map_context = GW::Map::GetMissionMapContext();
    const auto mission_map_frame = mission_map_context ? GW::UI::GetFrameById(mission_map_context->frame_id) : nullptr;
    if (!(mission_map_frame && mission_map_frame->IsVisible())) {
        window_open = false;
        return;
    }

    window_open = true;
    frame_id = mission_map_context->frame_id;

    const auto root = GW::UI::GetRootFrame();
    auto mission_map_top_left = mission_map_frame->position.GetContentTopLeft(root);
    left = mission_map_top_left.x;
    top = mission_map_top_left.y;
    auto mission_map_bottom_right = mission_map_frame->position.GetContentBottomRight(root);
    right = mission_map_bottom_right.x;
    bottom = mission_map_bottom_right.y;
    auto mission_map_scale = mission_map_frame->position.GetViewportScale(root);
    scale_x = mission_map_scale.x;
    scale_y = mission_map_scale.y;
    zoom = gameplay_context->mission_map_zoom;
    auto mission_map_center_pos = mission_map_context->player_mission_map_pos;
    center_x = mission_map_center_pos.x;
    center_y = mission_map_center_pos.y;
    auto mission_map_last_click_location = mission_map_context->last_mouse_location;
    last_click_x = mission_map_last_click_location.x;
    last_click_y = mission_map_last_click_location.y;
    auto current_pan_offset = mission_map_context->h003c->mission_map_pan_offset;
    pan_offset_x = current_pan_offset.x;
    pan_offset_y = current_pan_offset.y;
    //mission_map_screen_pos = GetMissionMapScreenCenterPos();
    mission_map_screen_center_x = mission_map_top_left.x + (mission_map_bottom_right.x - mission_map_top_left.x) / 2.0f;
    mission_map_screen_center_y = mission_map_top_left.y + (mission_map_bottom_right.y - mission_map_top_left.y) / 2.0f;
}



void bind_PyMissionMap(py::module_& m) {
    py::class_<PyMissionMap>(m, "PyMissionMap")
        .def(py::init<>())  // Default constructor
        .def("GetContext", &PyMissionMap::GetContext)  // Method to populate context
        .def_readonly("window_open", &PyMissionMap::window_open)  // Read-only property
        .def_readonly("frame_id", &PyMissionMap::frame_id)  // Read-only property
        .def_readonly("left", &PyMissionMap::left)  // Read-only property
        .def_readonly("top", &PyMissionMap::top)  // Read-only property
        .def_readonly("right", &PyMissionMap::right)  // Read-only property
        .def_readonly("bottom", &PyMissionMap::bottom)  // Read-only property
        .def_readonly("scale_x", &PyMissionMap::scale_x)  // Read-only property
        .def_readonly("scale_y", &PyMissionMap::scale_y)  // Read-only property
        .def_readonly("zoom", &PyMissionMap::zoom)  // Read-only property
        .def_readonly("center_x", &PyMissionMap::center_x)  // Read-only property
        .def_readonly("center_y", &PyMissionMap::center_y)  // Read-only property
        .def_readonly("last_click_x", &PyMissionMap::last_click_x)  // Read-only property
        .def_readonly("last_click_y", &PyMissionMap::last_click_y)  // Read-only property
        .def_readonly("pan_offset_x", &PyMissionMap::pan_offset_x)  // Read-only property
        .def_readonly("pan_offset_y", &PyMissionMap::pan_offset_y)  // Read-only property
        .def_readonly("mission_map_screen_center_x", &PyMissionMap::mission_map_screen_center_x)  // Read-only property
        .def_readonly("mission_map_screen_center_y", &PyMissionMap::mission_map_screen_center_y);  // Read-only property
}


PYBIND11_EMBEDDED_MODULE(PyMissionMap, m) {
    bind_PyMissionMap(m);
}