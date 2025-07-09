#pragma once
#include "Headers.h"

class PyCamera {
public:
    PyCamera();
    /* +h0000 */ uint32_t look_at_agent_id;
    /* +h0004 */ uint32_t h0004;
    /* +h0008 */ float h0008;
    /* +h000C */ float h000C;
    /* +h0010 */ float max_distance;
    /* +h0014 */ float h0014;
    /* +h0018 */ float yaw; // left/right camera angle, radians w/ origin @ east
    float current_yaw;
    /* +h001C */ float pitch; // up/down camera angle, range of [-1,1]
    /* +h0020 */ float camera_zoom; // current distance from players head.
    /* +h0024 */ std::vector<uint32_t> h0024;
    /* +h0034 */ float yaw_right_click;
    /* +h0038 */ float yaw_right_click2;
    /* +h003C */ float pitch_right_click;
    /* +h0040 */ float distance2;
    /* +h0044 */ float acceleration_constant;
    /* +h0048 */ float time_since_last_keyboard_rotation; // In seconds it seems.
    /* +h004C */ float time_since_last_mouse_rotation;
    /* +h0050 */ float time_since_last_mouse_move;
    /* +h0054 */ float time_since_last_agent_selection;
    /* +h0058 */ float time_in_the_map;
    /* +h005C */ float time_in_the_district;
    /* +h0060 */ float yaw_to_go;
    /* +h0064 */ float pitch_to_go;
    /* +h0068 */ float dist_to_go;
    /* +h006C */ float max_distance2;
    /* +h0070 */ std::vector<float> h0070;
    /* +h0078 */ Point3D position;
    /* +h0084 */ Point3D camera_pos_to_go;
    /* +h0090 */ Point3D cam_pos_inverted;
    /* +h009C */ Point3D cam_pos_inverted_to_go;
    /* +h00A8 */ Point3D look_at_target;
    /* +h00B4 */ Point3D look_at_to_go;
    /* +h00C0 */ float field_of_view;
    /* +h00C4 */ float field_of_view2;

    void GetContext();
    void ResetContext();
    void SetYaw(float _yaw);
    void SetPitch(float _pitch);
    void SetCameraPos(float x, float y, float z);
    void SetLookAtTarget(float x, float y, float z);

    void SetMaxDist(float dist);
    void SetFieldOfView(float fov);
    void UnlockCam(bool flag);
    bool GetCameraUnlock();
    void SetFog(bool flag);
    void ForwardMovement(float amount, bool true_forward);
    void VerticalMovement(float amount);
    void SideMovement(float amount);
    void RotateMovement(float angle);
    Point3D ComputeCameraPos();
    void UpdateCameraPos();

};
