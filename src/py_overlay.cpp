#include "py_overlay.h"

GW::Vec3f MouseWorldPos;
GW::Vec2f MouseScreenPos;

void GlobalMouseClass::SetMouseWorldPos(float x, float y, float z) {
    MouseWorldPos.x = x;
    MouseWorldPos.y = y;
    MouseWorldPos.z = z;
}

GW::Vec3f GlobalMouseClass::GetMouseWorldPos() {
    return MouseWorldPos;
}

void GlobalMouseClass::SetMouseCoords(float x, float y) {
    MouseScreenPos.x = x;
    MouseScreenPos.y = y;
}

GW::Vec2f GlobalMouseClass::GetMouseCoords() {
    return MouseScreenPos;
}

// Overlay methods
Overlay::Overlay() {
    //drawList = ImGui::GetWindowDrawList();
}

void Overlay::RefreshDrawList() {
	drawList = ImGui::GetWindowDrawList();
}
Point2D Overlay::GetMouseCoords() {
    ImVec2 mouse_pos = ImGui::GetIO().MousePos;
    return Point2D(mouse_pos.x, mouse_pos.y);
}


XMMATRIX Overlay::CreateViewMatrix(const XMFLOAT3& eye_pos, const XMFLOAT3& look_at_pos, const XMFLOAT3& up_direction) {
    XMMATRIX viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&eye_pos), XMLoadFloat3(&look_at_pos), XMLoadFloat3(&up_direction));
    return viewMatrix;
}

XMMATRIX Overlay::CreateProjectionMatrix(float fov, float aspect_ratio, float near_plane, float far_plane) {
    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspect_ratio, near_plane, far_plane);
    return projectionMatrix;
}

// Function to convert world coordinates to screen coordinates
GW::Vec3f Overlay::GetWorldToScreen(const GW::Vec3f& world_position, const XMMATRIX& mat_view, const XMMATRIX& mat_proj, float viewport_width, float viewport_height) {
    GW::Vec3f res;

    // Transform the point into camera space
    XMVECTOR pos = XMVectorSet(world_position.x, world_position.y, world_position.z, 1.0f);
    pos = XMVector3TransformCoord(pos, mat_view);

    // Transform the point into projection space
    pos = XMVector3TransformCoord(pos, mat_proj);

    // Perform perspective division
    XMFLOAT4 projected;
    XMStoreFloat4(&projected, pos);
    if (projected.w < 0.1f) return res; // Point is behind the camera

    projected.x /= projected.w;
    projected.y /= projected.w;
    projected.z /= projected.w;

    // Transform to screen space
    res.x = ((projected.x + 1.0f) / 2.0f) * viewport_width;
    res.y = ((-projected.y + 1.0f) / 2.0f) * viewport_height;
    res.z = projected.z;

    return res;
}

void Overlay::GetScreenToWorld() {

    GW::GameThread::Enqueue([]() {


        GlobalMouseClass setmousepos;
        GW::Vec2f* screen_coord = 0;
        uintptr_t address = GW::Scanner::Find("\x8B\xE5\x5D\xC3\x8B\x4F\x10\x83\xC7\x08", "xxxxxxxxxx", 0xC);
        //uintptr_t address = ptrGetter.GetNdcScreenCoordsPtr();
        if (Verify(address))
        {
            screen_coord = *reinterpret_cast<GW::Vec2f**>(address);
        }
        else { setmousepos.SetMouseWorldPos(0, 0, 0); return; }

        address = GW::Scanner::Find("\xD9\x5D\x14\xD9\x45\x14\x83\xEC\x0C", "xxxxxxxx", -0x2F);
        if (address)
        {
            ScreenToWorldPoint_Func = (ScreenToWorldPoint_pt)address;
        }
        else { setmousepos.SetMouseWorldPos(0, 0, 0); return; }

        address = GW::Scanner::Find("\xff\x75\x08\xd9\x5d\xfc\xff\x77\x7c", "xxxxxxxxx", -0x27);
        if (address) {
            MapIntersect_Func = (MapIntersect_pt)address;
        }
        else { setmousepos.SetMouseWorldPos(0, 0, 0); return; }

        GW::Vec3f origin;
        GW::Vec3f p2;
        float unk1 = ScreenToWorldPoint_Func(&origin, screen_coord->x, screen_coord->y, 0);
        float unk2 = ScreenToWorldPoint_Func(&p2, screen_coord->x, screen_coord->y, 1);
        GW::Vec3f dir = p2 - origin;
        GW::Vec3f ray_dir = GW::Normalize(dir);
        GW::Vec3f hit_point;
        int layer = 0;
        uint32_t ret = MapIntersect_Func(&origin, &ray_dir, &hit_point, &layer); //needs to run in game thread

        if (ret) { setmousepos.SetMouseWorldPos(hit_point.x, hit_point.y, hit_point.z); }
        else { setmousepos.SetMouseWorldPos(0, 0, 0); }

        });

}

float Overlay::findZ(float x, float y, float pz) {

    float altitude = ALTITUDE_UNKNOWN;
    unsigned int cur_altitude = 0u;
    float z = pz - (pz * 0.05);// adds an offset more of z to look for unleveled planes

    // in order to properly query altitudes, we have to use the pathing map
    // to determine the number of Z planes in the current map.
    const GW::PathingMapArray* pathing_map = GW::Map::GetPathingMap();
    if (pathing_map != nullptr) {
        const size_t pmap_size = pathing_map->size();
        if (pmap_size > 0) {

            GW::Map::QueryAltitude({ x, y, pmap_size - 1 }, 5.f, altitude);
            if (altitude < z) {
                // recall that the Up camera component is inverted
                z = altitude;
            }

        }
    }
    return z;
}

Point2D Overlay::WorldToScreen(float x, float y, float z) {
    GW::Vec3f world_position = { x, y, z };

    static auto cam = GW::CameraMgr::GetCamera();
    float camX = GW::CameraMgr::GetCamera()->position.x;
    float camY = GW::CameraMgr::GetCamera()->position.y;
    float camZ = GW::CameraMgr::GetCamera()->position.z;


    XMFLOAT3 eyePos = { camX, camY, camZ };
    XMFLOAT3 lookAtPos = { cam->look_at_target.x, cam->look_at_target.y, cam->look_at_target.z };
    XMFLOAT3 upDir = { 0.0f, 0.0f, -1.0f };  // Up Vector

    // Create view matrix
    XMMATRIX viewMatrix = CreateViewMatrix(eyePos, lookAtPos, upDir);

    // Define projection parameters
    float fov = GW::Render::GetFieldOfView();
    float aspectRatio = static_cast<float>(GW::Render::GetViewportWidth()) / static_cast<float>(GW::Render::GetViewportHeight());
    float nearPlane = 0.1f;
    float farPlane = 100000.0f;

    // Create projection matrix
    XMMATRIX projectionMatrix = CreateProjectionMatrix(fov, aspectRatio, nearPlane, farPlane);
    GW::Vec3f res = GetWorldToScreen(world_position, viewMatrix, projectionMatrix, static_cast<float>(GW::Render::GetViewportWidth()), static_cast<float>(GW::Render::GetViewportHeight()));
    Point2D ret = Point2D(res.x, res.y);
    return ret;

}

Point3D Overlay::GetMouseWorldPos() {
    GetScreenToWorld();
    GlobalMouseClass mousepos;
    GW::Vec3f ret_mouse_coords = mousepos.GetMouseWorldPos();
    Point3D ret = Point3D(ret_mouse_coords.x, ret_mouse_coords.y, ret_mouse_coords.z);
    return ret;
}

struct ImRect {
    ImVec2 Min;
    ImVec2 Max;

    ImRect() : Min(), Max() {}
    ImRect(const ImVec2& min, const ImVec2& max) : Min(min), Max(max) {}
    ImRect(float x1, float y1, float x2, float y2) : Min(x1, y1), Max(x2, y2) {}

    ImVec2 GetCenter() const { return ImVec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
    ImVec2 GetSize()   const { return ImVec2(Max.x - Min.x, Max.y - Min.y); }
    bool Contains(const ImVec2& p) const { return p.x >= Min.x && p.y >= Min.y && p.x < Max.x && p.y < Max.y; }
};

bool GetMapWorldMapBounds(GW::AreaInfo* map, ImRect* out) {
    if (!map) return false;
    auto bounds = &map->icon_start_x;
    if (*bounds == 0)
        bounds = &map->icon_start_x_dupe;

    // NB: Even though area info holds map bounds as uints, the world map uses signed floats anyway - a cast should be fine here.
    *out = {
        { static_cast<float> (bounds[0]), static_cast<float>(bounds[1]) },
        { static_cast<float> (bounds[2]), static_cast<float>(bounds[3]) }
    };
    return true;
}
GW::Array<GW::MapProp*>* GetMapProps() {
    const auto m = GW::GetMapContext();
    const auto p = m ? m->props : nullptr;
    return p ? &p->propArray : nullptr;
}

Point2D Overlay::GamePosToWorldMap(float x, float y) {
    ImRect map_bounds;
    if (!GetMapWorldMapBounds(GW::Map::GetMapInfo(), &map_bounds))
        return Point2D(0, 0);
    const auto current_map_context = GW::GetMapContext();
    if (!current_map_context)
        return Point2D(0, 0);

    const auto game_map_rect = ImRect({
        current_map_context->map_boundaries[1], current_map_context->map_boundaries[2],
        current_map_context->map_boundaries[3], current_map_context->map_boundaries[4],
        });

    // NB: World map is 96 gwinches per unit, this is hard coded in the GW source
    const auto gwinches_per_unit = 96.f;
    GW::Vec2f map_mid_world_point = {
        map_bounds.Min.x + (abs(game_map_rect.Min.x) / gwinches_per_unit),
        map_bounds.Min.y + (abs(game_map_rect.Max.y) / gwinches_per_unit),
    };

    float result_x = 0.f;
    float result_y = 0.f;
    // Convert the game coordinates to world map coordinates

    result_x = (x / gwinches_per_unit) + map_mid_world_point.x;
    result_y = ((y * -1.f) / gwinches_per_unit) + map_mid_world_point.y; // Inverted Y Axis
    return Point2D(result_x, result_y);

}

Point2D Overlay::WorlMapToGamePos(float x, float y) {
    ImRect map_bounds;
    if (!GetMapWorldMapBounds(GW::Map::GetMapInfo(), &map_bounds))
        return Point2D(0, 0);
    if (!map_bounds.Contains({ x, y }))
        return Point2D(0, 0); // Current map doesn't contain these coords; we can't plot a position

    const auto current_map_context = GW::GetMapContext();
    if (!current_map_context)
        return Point2D(0, 0);

    const auto game_map_rect = ImRect({
        current_map_context->map_boundaries[1], current_map_context->map_boundaries[2],
        current_map_context->map_boundaries[3], current_map_context->map_boundaries[4],
        });

    // NB: World map is 96 gwinches per unit, this is hard coded in the GW source
    constexpr auto gwinches_per_unit = 96.f;
    GW::Vec2f map_mid_world_point = {
        map_bounds.Min.x + (abs(game_map_rect.Min.x) / gwinches_per_unit),
        map_bounds.Min.y + (abs(game_map_rect.Max.y) / gwinches_per_unit),
    };

    float result_x = 0;
    float result_y = 0;
    result_x = (x - map_mid_world_point.x) * gwinches_per_unit;
    result_y = ((y - map_mid_world_point.y) * gwinches_per_unit) * -1.f; // Inverted Y Axis
    return Point2D(result_x, result_y);
}

Point2D Overlay::WorldMapToScreen(float x, float y) {
    GW::Vec2f position(x, y);
    const auto gameplay_context = GW::GetGameplayContext();
    const auto mission_map_context = GW::Map::GetMissionMapContext();
    const auto mission_map_frame = mission_map_context ? GW::UI::GetFrameById(mission_map_context->frame_id) : nullptr;
    if (!(mission_map_frame && mission_map_frame->IsVisible())) return Point2D(0, 0);

    const auto root = GW::UI::GetRootFrame();
    auto mission_map_top_left = mission_map_frame->position.GetContentTopLeft(root);
    auto mission_map_bottom_right = mission_map_frame->position.GetContentBottomRight(root);
    auto mission_map_scale = mission_map_frame->position.GetViewportScale(root);
    auto mission_map_zoom = gameplay_context->mission_map_zoom;
    auto mission_map_center_pos = mission_map_context->player_mission_map_pos;
    auto mission_map_last_click_location = mission_map_context->last_mouse_location;
    auto current_pan_offset = mission_map_context->h003c->mission_map_pan_offset;
    auto mission_map_screen_pos = mission_map_top_left + (mission_map_bottom_right - mission_map_top_left) / 2;

    const auto offset = (position - current_pan_offset);
    const auto scaled_offset = GW::Vec2f(offset.x * mission_map_scale.x, offset.y * mission_map_scale.y);
    auto result(scaled_offset * mission_map_zoom + mission_map_screen_pos);
    return Point2D(result.x, result.y);
}

Point2D Overlay::ScreenToWorldMap(float screen_x, float screen_y) {
    const auto gameplay = GW::GetGameplayContext();
    const auto mission_map_ctx = GW::Map::GetMissionMapContext();
    const auto frame = mission_map_ctx ? GW::UI::GetFrameById(mission_map_ctx->frame_id) : nullptr;
    if (!(frame && frame->IsVisible())) return Point2D(0, 0);

    const auto root = GW::UI::GetRootFrame();
    auto top_left = frame->position.GetContentTopLeft(root);
    auto bottom_right = frame->position.GetContentBottomRight(root);
    auto center = top_left + (bottom_right - top_left) / 2;
    auto scale = frame->position.GetViewportScale(root);
    auto zoom = gameplay->mission_map_zoom;
    auto pan_offset = mission_map_ctx->h003c->mission_map_pan_offset;

    GW::Vec2f offset = {
        (screen_x - center.x) / (zoom * scale.x),
        (screen_y - center.y) / (zoom * scale.y)
    };

    auto result = pan_offset + offset;
    return Point2D(result.x, result.y);
}

Point2D Overlay::GameMapToScreen(float x, float y)
{
    Point2D world_map_pos = GamePosToWorldMap(x, y);
    return WorldMapToScreen(world_map_pos.x, world_map_pos.y);
}

Point2D Overlay::ScreenToGameMapPos(float screen_x, float screen_y) {
    Point2D world = ScreenToWorldMap(screen_x, screen_y);
    return WorlMapToGamePos(world.x, world.y);
}

Point2D Overlay::NormalizedScreenToScreen(float norm_x, float norm_y)
{
    const auto mission_map_context = GW::Map::GetMissionMapContext();
    const auto mission_map_frame = mission_map_context ? GW::UI::GetFrameById(mission_map_context->frame_id) : nullptr;
    if (!(mission_map_frame && mission_map_frame->IsVisible()))
        return Point2D(0.f, 0.f);

    const auto root = GW::UI::GetRootFrame();

    GW::Vec2f top_left = mission_map_frame->position.GetContentTopLeft(root);
    GW::Vec2f bottom_right = mission_map_frame->position.GetContentBottomRight(root);
    GW::Vec2f size = bottom_right - top_left;

    // Convert from [-1, 1] to [0, 1], and invert Y
    float adjusted_x = (norm_x + 1.0f) / 2.0f;
    float adjusted_y = (1.0f - norm_y) / 2.0f;

    float screen_x = top_left.x + adjusted_x * size.x;
    float screen_y = top_left.y + adjusted_y * size.y;

    return Point2D(screen_x, screen_y);
}



Point2D Overlay::ScreenToNormalizedScreen(float screen_x, float screen_y)
{
    const auto mission_map_context = GW::Map::GetMissionMapContext();
    const auto mission_map_frame = mission_map_context ? GW::UI::GetFrameById(mission_map_context->frame_id) : nullptr;
    if (!(mission_map_frame && mission_map_frame->IsVisible()))
        return Point2D(0.f, 0.f);

    const auto root = GW::UI::GetRootFrame();

    GW::Vec2f top_left = mission_map_frame->position.GetContentTopLeft(root);
    GW::Vec2f bottom_right = mission_map_frame->position.GetContentBottomRight(root);
    GW::Vec2f size = bottom_right - top_left;

    float rel_x = (screen_x - top_left.x) / size.x;
    float rel_y = (screen_y - top_left.y) / size.y;

    // Convert to normalized range [-1, 1], with Y inverted
    float norm_x = rel_x * 2.0f - 1.0f;
    float norm_y = (1.0f - rel_y) * 2.0f - 1.0f;

    return Point2D(norm_x, norm_y);
}


Point2D Overlay::NormalizedScreenToWorldMap(float norm_x, float norm_y) {
    Point2D screen = NormalizedScreenToScreen(norm_x, norm_y);
    return ScreenToWorldMap(screen.x, screen.y);
}


Point2D Overlay::NormalizedScreenToGameMap(float norm_x, float norm_y) {
    Point2D world = NormalizedScreenToWorldMap(norm_x, norm_y);
    return WorlMapToGamePos(world.x, world.y);
}


Point2D Overlay::GamePosToNormalizedScreen(float x, float y) {
    Point2D screen = GameMapToScreen(x, y);
    return Point2D(
        ScreenToNormalizedScreen(screen.x, screen.y)
    );
}

void Overlay::BeginDraw() {

    ImGuiIO& io = ImGui::GetIO();

    // Make the panel cover the whole screen
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // Create a transparent and click-through panel
    ImGui::Begin("HeroOverlay", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoInputs);
}

void Overlay::BeginDraw(std::string name) {

    ImGuiIO& io = ImGui::GetIO();

    // Make the panel cover the whole screen
    ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    // Create a transparent and click-through panel
    std::string internal_name = "##" + name;
    ImGui::Begin(internal_name.c_str(), nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoInputs);
}

void Overlay::BeginDraw(std::string name, float x, float y, float width, float height) {
	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(x, y));
	// Create a transparent and click-through panel
    std::string internal_name = "##" + name;
	ImGui::Begin(internal_name.c_str(), nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoInputs);
}

void Overlay::EndDraw() {
    ImGui::End();
}


void Overlay::DrawLine(Point2D from, Point2D to, ImU32 color = 0xFF00FF00, float thickness = 1.0) {
    drawList = ImGui::GetWindowDrawList();
    ImVec2 from_imvec = ImVec2(from.x, from.y);
    ImVec2 to_imvec = ImVec2(to.x, to.y);
    drawList->AddLine(from_imvec, to_imvec, color, thickness);
}

void Overlay::DrawLine3D(Point3D from, Point3D to, ImU32 color = 0xFF00FF00, float thickness = 1.0) {
    drawList = ImGui::GetWindowDrawList();
    Point2D from3D = WorldToScreen(from.x, from.y, from.z);
    Point2D to3D = WorldToScreen(to.x, to.y, to.z);

    ImVec2 fromProjected;
    ImVec2 toProjected;

    fromProjected.x = from3D.x;
    fromProjected.y = from3D.y;

    toProjected.x = to3D.x;
    toProjected.y = to3D.y;

    drawList->AddLine(fromProjected, toProjected, color, thickness);
}

void Overlay::DrawTriangle(Point2D p1, Point2D p2, Point2D p3, ImU32 color, float thickness) {
    drawList = ImGui::GetWindowDrawList();
    ImVec2 v1(p1.x, p1.y);
    ImVec2 v2(p2.x, p2.y);
    ImVec2 v3(p3.x, p3.y);
    drawList->AddTriangle(v1, v2, v3, color, thickness);
}


void Overlay::DrawTriangle3D(Point3D p1, Point3D p2, Point3D p3, ImU32 color, float thickness) {
    drawList = ImGui::GetWindowDrawList();
    Point2D a = WorldToScreen(p1.x, p1.y, p1.z);
    Point2D b = WorldToScreen(p2.x, p2.y, p2.z);
    Point2D c = WorldToScreen(p3.x, p3.y, p3.z);
    drawList->AddTriangle(ImVec2(a.x, a.y), ImVec2(b.x, b.y), ImVec2(c.x, c.y), color, thickness);
}

void Overlay::DrawTriangleFilled(Point2D p1, Point2D p2, Point2D p3, ImU32 color) {
    drawList = ImGui::GetWindowDrawList();
    drawList->AddTriangleFilled(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), ImVec2(p3.x, p3.y), color);
}

void Overlay::DrawTriangleFilled3D(Point3D p1, Point3D p2, Point3D p3, ImU32 color) {
    drawList = ImGui::GetWindowDrawList();
    Point2D p1Screen = WorldToScreen(p1.x, p1.y, p1.z);
    Point2D p2Screen = WorldToScreen(p2.x, p2.y, p2.z);
    Point2D p3Screen = WorldToScreen(p3.x, p3.y, p3.z);
    ImVec2 p1ImVec(p1Screen.x, p1Screen.y);
    ImVec2 p2ImVec(p2Screen.x, p2Screen.y);
    ImVec2 p3ImVec(p3Screen.x, p3Screen.y);
    drawList->AddTriangleFilled(p1ImVec, p2ImVec, p3ImVec, color);
}

void Overlay::DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4, ImU32 color, float thickness) {
    drawList = ImGui::GetWindowDrawList();
    ImVec2 points[4] = {
        ImVec2(p1.x, p1.y),
        ImVec2(p2.x, p2.y),
        ImVec2(p3.x, p3.y),
        ImVec2(p4.x, p4.y)
    };
    drawList->AddPolyline(points, 4, color, true /* closed */, thickness);
}


void Overlay::DrawQuad3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, ImU32 color, float thickness) {
    drawList = ImGui::GetWindowDrawList();

    Point2D s1 = WorldToScreen(p1.x, p1.y, p1.z);
    Point2D s2 = WorldToScreen(p2.x, p2.y, p2.z);
    Point2D s3 = WorldToScreen(p3.x, p3.y, p3.z);
    Point2D s4 = WorldToScreen(p4.x, p4.y, p4.z);

    ImVec2 screenPoints[4] = {
        ImVec2(s1.x, s1.y),
        ImVec2(s2.x, s2.y),
        ImVec2(s3.x, s3.y),
        ImVec2(s4.x, s4.y)
    };

    drawList->AddPolyline(screenPoints, 4, color, true /* closed */, thickness);
}


void Overlay::DrawQuadFilled(Point2D p1, Point2D p2, Point2D p3, Point2D p4, ImU32 color) {
    drawList = ImGui::GetWindowDrawList();
    ImVec2 points[4] = {
        ImVec2(p1.x, p1.y),
        ImVec2(p2.x, p2.y),
        ImVec2(p3.x, p3.y),
        ImVec2(p4.x, p4.y)
    };
    drawList->AddConvexPolyFilled(points, 4, color);
}


void Overlay::DrawQuadFilled3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, ImU32 color) {
    drawList = ImGui::GetWindowDrawList();

    Point2D s1 = WorldToScreen(p1.x, p1.y, p1.z);
    Point2D s2 = WorldToScreen(p2.x, p2.y, p2.z);
    Point2D s3 = WorldToScreen(p3.x, p3.y, p3.z);
    Point2D s4 = WorldToScreen(p4.x, p4.y, p4.z);

    ImVec2 screenPoints[4] = {
        ImVec2(s1.x, s1.y),
        ImVec2(s2.x, s2.y),
        ImVec2(s3.x, s3.y),
        ImVec2(s4.x, s4.y)
    };

    drawList->AddConvexPolyFilled(screenPoints, 4, color);
}


void Overlay::DrawPoly(Point2D center, float radius, ImU32 color, int numSegments, float thickness) {
    drawList = ImGui::GetWindowDrawList();
    std::vector<ImVec2> points;
    points.reserve(numSegments);

    const float segmentRadian = 2.0f * 3.14159265358979323846f / numSegments;

    for (int i = 0; i < numSegments; ++i) {
        float angle = segmentRadian * i;
        float x = center.x + cos(angle) * radius;
        float y = center.y + sin(angle) * radius;
        points.emplace_back(ImVec2(x, y));
    }

    drawList->AddPolyline(points.data(), points.size(), color, true /* closed */, thickness);
}


void Overlay::DrawPolyFilled(Point2D center, float radius, ImU32 color, int numSegments) {
    drawList = ImGui::GetWindowDrawList();
    std::vector<ImVec2> points;
    points.reserve(numSegments);

    const float segmentRadian = 2.0f * 3.14159265358979323846f / numSegments;

    for (int i = 0; i < numSegments; ++i) {
        float angle = segmentRadian * i;
        float x = center.x + cos(angle) * radius;
        float y = center.y + sin(angle) * radius;
        points.emplace_back(ImVec2(x, y));
    }

    drawList->AddConvexPolyFilled(points.data(), points.size(), color);
}



void Overlay::DrawPoly3D(Point3D center, float radius, ImU32 color, int numSegments, float thickness, bool autoZ) {
    drawList = ImGui::GetWindowDrawList();
    const float segmentRadian = 2.0f * 3.14159265358979323846f / numSegments;

    std::vector<ImVec2> points;
    points.reserve(numSegments);

    for (int i = 0; i < numSegments; ++i) {
        float angle = segmentRadian * i;
        float x = center.x + radius * std::cos(angle);
        float y = center.y + radius * std::sin(angle);
        float z = autoZ ? findZ(x, y, center.z) : center.z;

        Point2D screen = WorldToScreen(x, y, z);
        points.emplace_back(ImVec2(screen.x, screen.y));
    }

    drawList->AddPolyline(points.data(), points.size(), color, true /* closed */, thickness);
}



void Overlay::DrawPolyFilled3D(Point3D center, float radius, ImU32 color, int numSegments, bool autoZ) {
    drawList = ImGui::GetWindowDrawList();
    std::vector<ImVec2> points;
    points.reserve(numSegments);

    const float segmentRadian = 2.0f * 3.14159265358979323846f / numSegments;

    for (int i = 0; i < numSegments; ++i) {
        float angle = segmentRadian * i;
        float x = center.x + cos(angle) * radius;
        float y = center.y + sin(angle) * radius;
        float z = autoZ ? findZ(x, y, center.z) : center.z;

        Point2D screen = WorldToScreen(x, y, z);
        points.emplace_back(ImVec2(screen.x, screen.y));
    }

    drawList->AddConvexPolyFilled(points.data(), points.size(), color);
}


void Overlay::DrawCubeOutline(Point3D center, float size, ImU32 color, float thickness) {
    drawList = ImGui::GetWindowDrawList();
    float h = size / 2.0f;

    Point3D corners[8] = {
        {center.x - h, center.y - h, center.z - h},
        {center.x + h, center.y - h, center.z - h},
        {center.x + h, center.y + h, center.z - h},
        {center.x - h, center.y + h, center.z - h},
        {center.x - h, center.y - h, center.z + h},
        {center.x + h, center.y - h, center.z + h},
        {center.x + h, center.y + h, center.z + h},
        {center.x - h, center.y + h, center.z + h},
    };

    int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    for (auto& edge : edges) {
        Point2D p1 = WorldToScreen(corners[edge[0]].x, corners[edge[0]].y, corners[edge[0]].z);
        Point2D p2 = WorldToScreen(corners[edge[1]].x, corners[edge[1]].y, corners[edge[1]].z);
        drawList->AddLine(ImVec2(p1.x, p1.y), ImVec2(p2.x, p2.y), color, thickness);
    }
}


void Overlay::DrawCubeFilled(Point3D center, float size, ImU32 color) {
    drawList = ImGui::GetWindowDrawList();
    float h = size / 2.0f;

    Point3D corners[8] = {
        {center.x - h, center.y - h, center.z - h},
        {center.x + h, center.y - h, center.z - h},
        {center.x + h, center.y + h, center.z - h},
        {center.x - h, center.y + h, center.z - h},
        {center.x - h, center.y - h, center.z + h},
        {center.x + h, center.y - h, center.z + h},
        {center.x + h, center.y + h, center.z + h},
        {center.x - h, center.y + h, center.z + h},
    };

    int faces[6][4] = {
        {0,1,2,3}, {4,5,6,7}, {0,1,5,4},
        {2,3,7,6}, {0,3,7,4}, {1,2,6,5}
    };

    for (auto& face : faces) {
        ImVec2 screenPts[4];
        for (int i = 0; i < 4; ++i) {
            Point2D screen = WorldToScreen(corners[face[i]].x, corners[face[i]].y, corners[face[i]].z);
            screenPts[i] = ImVec2(screen.x, screen.y);
        }
        drawList->AddConvexPolyFilled(screenPts, 4, color);
    }
}



void Overlay::DrawText2D(Point2D position, std::string text, ImU32 color, bool centered, float scale) {
    drawList = ImGui::GetWindowDrawList();
    ImVec2 screenPos(position.x, position.y);
    ImFont* currentFont = ImGui::GetFont();   // Get the current font
    float originalFontSize = currentFont->Scale;  // Save the original font scale
    currentFont->Scale = scale;  // Set the new scale for the font

    ImGui::PushFont(currentFont);  // Apply the new font scale

    // Check if the text should be centered
    if (centered) {
        // Get the size of the text with the current font scale
        ImVec2 textSizeVec = ImGui::CalcTextSize(text.c_str());

        // Adjust the x coordinate to center the text
        screenPos.x -= textSizeVec.x / 2;
    }

    // Convert std::string to const char* for ImGui::AddText
    drawList->AddText(screenPos, color, text.c_str());

    // Restore the original font size
    currentFont->Scale = originalFontSize;  // Restore original font scale
    ImGui::PopFont();  // Pop the font after rendering
}

void Overlay::DrawText3D(Point3D position3D, std::string text, ImU32 color, bool autoZ, bool centered, float scale) {
    drawList = ImGui::GetWindowDrawList();
    // Project the 3D position to 2D screen coordinates
    Point2D screenPosition = { 0, 0 };

    if (autoZ) {
        // Optionally adjust Z if autoZ is true
        float z = findZ(position3D.x, position3D.y, position3D.z);
        screenPosition = WorldToScreen(position3D.x, position3D.y, z);
    }
    else {
        screenPosition = WorldToScreen(position3D.x, position3D.y, position3D.z);
    }

    // Convert the screen position to ImVec2 for ImGui
    ImVec2 screenPos(screenPosition.x, screenPosition.y);

    // Save the current font size and set the new one
    ImFont* currentFont = ImGui::GetFont();   // Get the current font
    float originalFontSize = currentFont->Scale;  // Save the original font scale
    currentFont->Scale = scale;  // Set the new scale for the font

    ImGui::PushFont(currentFont);  // Apply the new font scale

    // Check if the text should be centered
    if (centered) {
        // Get the size of the text with the current font scale
        ImVec2 textSizeVec = ImGui::CalcTextSize(text.c_str());

        // Adjust the x coordinate to center the text
        screenPos.x -= textSizeVec.x / 2;
    }

    // Convert std::string to const char* for ImGui::AddText
    drawList->AddText(screenPos, color, text.c_str());

    // Restore the original font size
    currentFont->Scale = originalFontSize;  // Restore original font scale
    ImGui::PopFont();  // Pop the font after rendering
}





Point2D Overlay::GetDisplaySize() {
    drawList = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();
    return Point2D(io.DisplaySize.x, io.DisplaySize.y);
}

bool Overlay::IsMouseClicked(int button) {
    drawList = ImGui::GetWindowDrawList();
    ImGuiIO& io = ImGui::GetIO();
	return io.MouseDown[button];
}

void Overlay::PushClipRect(float x, float y, float x2, float y2) {
	drawList = ImGui::GetWindowDrawList();
	ImVec2 min(x, y);
	ImVec2 max(x2, y2);
	drawList->PushClipRect(min, max, true);
}

std::vector<float> GetMapBoundaries() {
    GW::MapContext* m = GW::GetMapContext();
    std::vector<float> boundaries;
    for (int i = 0; i < 5; i++) {
        boundaries.push_back(m->map_boundaries[i]);
    }
    return boundaries;
};

struct PathingTrapezoid {
    uint32_t id;
    std::vector<uint32_t> neighbor_ids; // Indices of adjacent trapezoids
    uint16_t portal_left;
    uint16_t portal_right;
    float XTL;
    float XTR;
    float YT;
    float XBL;
    float XBR;
    float YB;

    PathingTrapezoid()
        : id(0), portal_left(0xFFFF), portal_right(0xFFFF),
        XTL(0), XTR(0), YT(0), XBL(0), XBR(0), YB(0) {}
};

struct Node {
    uint32_t type; // XNode = 0, YNode = 1, SinkNode = 2
    uint32_t id;

    Node() : type(0), id(0) {}
};

struct XNode : Node {
    std::pair<float, float> pos; // Position (x, y)
    std::pair<float, float> dir; // Direction vector (x, y)
    uint32_t left_id;  // Left node ID
    uint32_t right_id; // Right node ID

    XNode() : pos({ 0.0f, 0.0f }), dir({ 0.0f, 0.0f }), left_id(UINT32_MAX), right_id(UINT32_MAX) {
        type = 0; // Set type to XNode
    }
};

struct YNode : Node {
    std::pair<float, float> pos; // Position (x, y)
    uint32_t left_id;  // Left node ID
    uint32_t right_id; // Right node ID

    YNode() : pos({ 0.0f, 0.0f }), left_id(UINT32_MAX), right_id(UINT32_MAX) {
        type = 1; // Set type to YNode
    }
};

struct SinkNode : Node {
    std::vector<uint32_t> trapezoid_ids; // IDs of associated trapezoids

    SinkNode() {
        type = 2; // Set type to SinkNode
    }
};

struct Portal {
    uint16_t left_layer_id;
    uint16_t right_layer_id;
    uint32_t h0004;
    uint32_t pair_index;  // Index of the paired portal
    std::vector<uint32_t> trapezoid_indices; // IDs of associated trapezoids

    Portal() : left_layer_id(0xFFFF), right_layer_id(0xFFFF), h0004(0), pair_index(UINT32_MAX) {}
};

struct PathingMap {
    uint32_t zplane;
    uint32_t h0004;
    uint32_t h0008;
    uint32_t h000C;
    uint32_t h0010;
    std::vector<PathingTrapezoid> trapezoids;
    std::vector<SinkNode> sink_nodes;
    std::vector<XNode> x_nodes;
    std::vector<YNode> y_nodes;
    uint32_t h0034;
    uint32_t h0038;
    std::vector<Portal> portals;
    uint32_t root_node_id;

    PathingMap()
        : zplane(UINT32_MAX), h0004(0), h0008(0), h000C(0), h0010(0), h0034(0), h0038(0), root_node_id(UINT32_MAX) {}
};

PathingTrapezoid ConvertTrapezoid(const GW::PathingTrapezoid& original) {
    PathingTrapezoid trapezoid;
    trapezoid.id = original.id;
    trapezoid.XTL = original.XTL;
    trapezoid.XTR = original.XTR;
    trapezoid.YT = original.YT;
    trapezoid.XBL = original.XBL;
    trapezoid.XBR = original.XBR;
    trapezoid.YB = original.YB;
    trapezoid.portal_left = original.portal_left;
    trapezoid.portal_right = original.portal_right;

    // Map adjacent trapezoids by their IDs
    for (int i = 0; i < 4; ++i) {
        if (original.adjacent[i]) {
            trapezoid.neighbor_ids.push_back(original.adjacent[i]->id);
        }
    }

    return trapezoid;
}

PathingMap ConvertPathingMap(const GW::PathingMap& original) {
    PathingMap pathing_map;
    pathing_map.zplane = original.zplane;

    // Convert trapezoids
    for (uint32_t i = 0; i < original.trapezoid_count; ++i) {
        pathing_map.trapezoids.push_back(ConvertTrapezoid(original.trapezoids[i]));
    }

    // Convert SinkNodes
    /*
    for (uint32_t i = 0; i < original.sink_node_count; ++i) {
        SinkNode sink_node;
        sink_node.id = original.sink_nodes[i].id;

        // Safely iterate through the null-terminated trapezoid list
        if (original.sink_nodes[i].trapezoid) { // Ensure the trapezoid list exists
            const GW::PathingTrapezoid* const* trapezoid_ptr = original.sink_nodes[i].trapezoid;

            // Iterate until the null-terminator is reached
            while (*trapezoid_ptr != nullptr) {
                sink_node.trapezoid_ids.push_back((*trapezoid_ptr)->id);
                ++trapezoid_ptr; // Move to the next pointer in the array
            }
        }

        // Add the converted SinkNode to the destination map
        pathing_map.sink_nodes.push_back(sink_node);
    }
    */

    // Convert XNodes
    for (uint32_t i = 0; i < original.x_node_count; ++i) {
        XNode x_node;
        x_node.id = original.x_nodes[i].id;
        x_node.pos = { original.x_nodes[i].pos.x, original.x_nodes[i].pos.y };
        x_node.dir = { original.x_nodes[i].dir.x, original.x_nodes[i].dir.y };
        if (original.x_nodes[i].left) x_node.left_id = original.x_nodes[i].left->id;
        if (original.x_nodes[i].right) x_node.right_id = original.x_nodes[i].right->id;

        pathing_map.x_nodes.push_back(x_node);
    }

    // Convert YNodes
    for (uint32_t i = 0; i < original.y_node_count; ++i) {
        YNode y_node;
        y_node.id = original.y_nodes[i].id;
        y_node.pos = { original.y_nodes[i].pos.x, original.y_nodes[i].pos.y };
        if (original.y_nodes[i].left) y_node.left_id = original.y_nodes[i].left->id;
        if (original.y_nodes[i].right) y_node.right_id = original.y_nodes[i].right->id;

        pathing_map.y_nodes.push_back(y_node);
    }

    // Convert Portals
    for (uint32_t i = 0; i < original.portal_count; ++i) {
        Portal portal;
        portal.left_layer_id = original.portals[i].left_layer_id;
        portal.right_layer_id = original.portals[i].right_layer_id;
        portal.h0004 = original.portals[i].h0004;

        // Map trapezoids connected by the portal
        for (uint32_t j = 0; j < original.portals[i].count; ++j) {
            portal.trapezoid_indices.push_back(original.portals[i].trapezoids[j]->id);
        }

        // Determine the pair index manually
        portal.pair_index = UINT32_MAX; // Default to invalid
        if (original.portals[i].pair) {
            for (uint32_t k = 0; k < original.portal_count; ++k) {
                if (&original.portals[k] == original.portals[i].pair) {
                    portal.pair_index = k;
                    break;
                }
            }
        }

        pathing_map.portals.push_back(portal);
    }

    return pathing_map;
}



std::vector<PathingMap> GetPathingMaps() {
    const auto* pathing_map_array = GW::Map::GetPathingMap();

    std::vector<PathingMap> converted_maps;

    if (pathing_map_array) {
        for (size_t i = 0; i < pathing_map_array->size(); ++i) {
            converted_maps.push_back(ConvertPathingMap((*pathing_map_array)[i]));
        }
    }

    return converted_maps;
}



namespace py = pybind11;

void bind_Point2D(py::module_& m) {
    py::class_<Point2D>(m, "Point2D")
        .def(py::init<int, int>()) // Bind the constructor
        .def_readonly("x", &Point2D::x, "X coordinate")  // Expose x as a property
        .def_readonly("y", &Point2D::y, "Y coordinate");  // Expose y as a property
}

void bind_Point3D(py::module_& m) {
    py::class_<Point3D>(m, "Point3D")
        .def(py::init<float, float, float>()) // Bind the constructor
        .def_readonly("x", &Point3D::x, "X coordinate")  // Expose x as a property
        .def_readonly("y", &Point3D::y, "Y coordinate")  // Expose y as a property
        .def_readonly("z", &Point3D::z, "Z coordinate");  // Expose z as a property
}

void bind_overlay(py::module_& m) {
    py::class_<Overlay>(m, "Overlay")
        .def(py::init<>())  // Constructor binding
        .def("RefreshDrawList", &Overlay::RefreshDrawList)  // Expose RefreshDrawList method
        .def("GetMouseCoords", &Overlay::GetMouseCoords)  // Expose GetMouseCoords method
        .def("FindZ", &Overlay::findZ, py::arg("x"), py::arg("y"), py::arg("pz"))  // Expose findZ method
        .def("WorldToScreen", &Overlay::WorldToScreen, py::arg("x"), py::arg("y"), py::arg("z"))  // Expose WorldToScreen method
        .def("GetMouseWorldPos", &Overlay::GetMouseWorldPos)  // Expose GetMouseWorldPos method
        // Game <-> World
        .def("GamePosToWorldMap", &Overlay::GamePosToWorldMap, py::arg("x"), py::arg("y"), "Convert game position to world map coordinates")
        .def("WorldMapToGamePos", &Overlay::WorlMapToGamePos, py::arg("x"), py::arg("y"), "Convert world map position to game coordinates")
        // World <-> Screen
        .def("WorldMapToScreen", &Overlay::WorldMapToScreen, py::arg("x"), py::arg("y"), "Convert world map position to screen coordinates")
		.def("ScreenToWorldMap", &Overlay::ScreenToWorldMap, py::arg("x"), py::arg("y"), "Convert screen position to world map coordinates")
        // Game <-> Screen (combined)
        .def("GameMapToScreen", &Overlay::GameMapToScreen, py::arg("x"), py::arg("y"), "Convert game map position to screen coordinates")
		.def("ScreenToGameMapPos", &Overlay::ScreenToGameMapPos, py::arg("x"), py::arg("y"), "Convert screen position to game map coordinates")
        //NormalizedScreen <-> Screen
		.def("NormalizedScreenToScreen", &Overlay::NormalizedScreenToScreen, py::arg("norm_x"), py::arg("norm_y"), "Convert normalized screen coordinates to screen coordinates")
		.def("ScreenToNormalizedScreen", &Overlay::ScreenToNormalizedScreen, py::arg("screen_x"), py::arg("screen_y"), "Convert screen coordinates to normalized screen coordinates")
        //NormalizedScreen <-> World / Game
		.def("NormalizedScreenToWorldMap", &Overlay::NormalizedScreenToWorldMap, py::arg("norm_x"), py::arg("norm_y"), "Convert normalized screen coordinates to world map coordinates")
		.def("NormalizedScreenToGameMap", &Overlay::NormalizedScreenToGameMap, py::arg("norm_x"), py::arg("norm_y"), "Convert normalized screen coordinates to game map coordinates")
		.def("GamePosToNormalizedScreen", &Overlay::GamePosToNormalizedScreen, py::arg("x"), py::arg("y"), "Convert game position to normalized screen coordinates")

        .def("BeginDraw", py::overload_cast<>(&Overlay::BeginDraw), "BeginDraw with no arguments")
        .def("BeginDraw", py::overload_cast<std::string>(&Overlay::BeginDraw), py::arg("name"), "BeginDraw with name")
        .def("BeginDraw", py::overload_cast<std::string, float, float, float, float>(&Overlay::BeginDraw),
            py::arg("name"), py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"), "BeginDraw with name and bounds")
        .def("EndDraw", &Overlay::EndDraw)
        .def("DrawLine", &Overlay::DrawLine, py::arg("from"), py::arg("to"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
        .def("DrawLine3D", &Overlay::DrawLine3D, py::arg("from"), py::arg("to"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
		.def("DrawTriangle", &Overlay::DrawTriangle, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
		.def("DrawTriangle3D", &Overlay::DrawTriangle3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
		.def("DrawTriangleFilled", &Overlay::DrawTriangleFilled, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF)
		.def("DrawTriangleFilled3D", &Overlay::DrawTriangleFilled3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF)
		.def("DrawQuad", &Overlay::DrawQuad, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
		.def("DrawQuad3D", &Overlay::DrawQuad3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
		.def("DrawQuadFilled", &Overlay::DrawQuadFilled, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF)
		.def("DrawQuadFilled3D", &Overlay::DrawQuadFilled3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF)
		.def("DrawPoly", &Overlay::DrawPoly, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("numSegments") = 12, py::arg("thickness") = 1.0f)
		.def("DrawPoly3D", &Overlay::DrawPoly3D, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("numSegments") = 12, py::arg("thickness") = 1.0f, py::arg("autoZ") = true)
		.def("DrawPolyFilled", &Overlay::DrawPolyFilled, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("numSegments") = 12)
		.def("DrawPolyFilled3D", &Overlay::DrawPolyFilled3D, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("numSegments") = 12, py::arg("autoZ") = true)
		.def("DrawCubeOutline", &Overlay::DrawCubeOutline, py::arg("center"), py::arg("size"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
		.def("DrawCubeFilled", &Overlay::DrawCubeFilled, py::arg("center"), py::arg("size"), py::arg("color") = 0xFFFFFFFF)

        .def("DrawText", &Overlay::DrawText2D, py::arg("position"), py::arg("text"), py::arg("color") = 0xFFFFFFFF, py::arg("centered") = true, py::arg("scale") = 1.0)
        .def("DrawText3D", &Overlay::DrawText3D, py::arg("position3D"), py::arg("text"), py::arg("color") = 0xFFFFFFFF, py::arg("autoZ") = true, py::arg("centered") = true, py::arg("scale") = 1.0)
        .def("GetDisplaySize", &Overlay::GetDisplaySize)
        .def("IsMouseClicked", &Overlay::IsMouseClicked, py::arg("button") = 0)
        .def("PushClipRect", &Overlay::PushClipRect, py::arg("x"), py::arg("y"), py::arg("x2"), py::arg("y2"))
        .def("PopClipRect", &Overlay::PopClipRect);

}

PYBIND11_EMBEDDED_MODULE(PyOverlay, m) {
    bind_Point2D(m);
    bind_Point3D(m);
    bind_overlay(m);
}



PYBIND11_EMBEDDED_MODULE(PyPathing, m) {
    // Bind PathingTrapezoid
    py::class_<PathingTrapezoid>(m, "PathingTrapezoid")
        .def(py::init<>())  // Default constructor
        .def_readwrite("id", &PathingTrapezoid::id)
        .def_readwrite("XTL", &PathingTrapezoid::XTL)
        .def_readwrite("XTR", &PathingTrapezoid::XTR)
        .def_readwrite("YT", &PathingTrapezoid::YT)
        .def_readwrite("XBL", &PathingTrapezoid::XBL)
        .def_readwrite("XBR", &PathingTrapezoid::XBR)
        .def_readwrite("YB", &PathingTrapezoid::YB)
        .def_readwrite("portal_left", &PathingTrapezoid::portal_left)
        .def_readwrite("portal_right", &PathingTrapezoid::portal_right)
        .def_readwrite("neighbor_ids", &PathingTrapezoid::neighbor_ids);  // List of adjacent trapezoid IDs

    // Bind Portal
    py::class_<Portal>(m, "Portal")
        .def(py::init<>())
        .def_readwrite("left_layer_id", &Portal::left_layer_id)
        .def_readwrite("right_layer_id", &Portal::right_layer_id)
        .def_readwrite("h0004", &Portal::h0004)
        .def_readwrite("pair_index", &Portal::pair_index)  // Paired portal index
        .def_readwrite("trapezoid_indices", &Portal::trapezoid_indices);  // Vector of trapezoid IDs


    // Bind Node as base class
    py::class_<Node>(m, "Node")
        .def(py::init<>())
        .def_readwrite("type", &Node::type)
        .def_readwrite("id", &Node::id);

    // Bind XNode, YNode, and SinkNode as derived classes
    py::class_<XNode, Node>(m, "XNode")
        .def(py::init<>())
        .def_readwrite("pos", &XNode::pos)   // Python-safe 2D position
        .def_readwrite("dir", &XNode::dir)   // Direction vector
        .def_readwrite("left_id", &XNode::left_id)
        .def_readwrite("right_id", &XNode::right_id);

    py::class_<YNode, Node>(m, "YNode")
        .def(py::init<>())
        .def_readwrite("pos", &YNode::pos)
        .def_readwrite("left_id", &YNode::left_id)
        .def_readwrite("right_id", &YNode::right_id);

    py::class_<SinkNode, Node>(m, "SinkNode")
        .def(py::init<>())
        .def_readwrite("trapezoid_ids", &SinkNode::trapezoid_ids);  // List of trapezoid IDs

    // Bind PathingMap
    py::class_<PathingMap>(m, "PathingMap")
        .def(py::init<>())  // Default constructor
        .def_readwrite("zplane", &PathingMap::zplane)
        .def_readwrite("h0004", &PathingMap::h0004)
        .def_readwrite("h0008", &PathingMap::h0008)
        .def_readwrite("h000C", &PathingMap::h000C)
        .def_readwrite("h0010", &PathingMap::h0010)
        .def_readwrite("trapezoids", &PathingMap::trapezoids)  // Vector of PathingTrapezoid
        .def_readwrite("sink_nodes", &PathingMap::sink_nodes)  // Vector of SinkNode
        .def_readwrite("x_nodes", &PathingMap::x_nodes)        // Vector of XNode
        .def_readwrite("y_nodes", &PathingMap::y_nodes)        // Vector of YNode
        .def_readwrite("h0034", &PathingMap::h0034)
        .def_readwrite("h0038", &PathingMap::h0038)
        .def_readwrite("portals", &PathingMap::portals)        // Vector of Portal
        .def_readwrite("root_node_id", &PathingMap::root_node_id);

    // Bind the GetPathingMaps function
    m.def("get_map_boundaries", &GetMapBoundaries, "Retrieve map boundaries");
    m.def("get_pathing_maps", &GetPathingMaps, "Retrieve and convert all pathing maps");
}

PyCamera::PyCamera() {
	GetContext();
}

void PyCamera::GetContext() {
    const auto camera = GW::CameraMgr::GetCamera();
	look_at_agent_id = camera->look_at_agent_id;
	h0004 = camera->h0004;
	h0008 = camera->h0008;
	h000C = camera->h000C;
	max_distance = camera->max_distance;
	h0014 = camera->h0014;
	yaw = camera->yaw;
	current_yaw = camera->GetCurrentYaw();
	pitch = camera->pitch;
	camera_zoom = camera->distance;
	h0024.push_back(camera->h0024[0]);
	h0024.push_back(camera->h0024[1]);
	h0024.push_back(camera->h0024[2]);
	h0024.push_back(camera->h0024[3]);
	yaw_right_click = camera->yaw_right_click;
	yaw_right_click2 = camera->yaw_right_click2;
	pitch_right_click = camera->pitch_right_click;
	distance2 = camera->distance2;
	acceleration_constant = camera->acceleration_constant;
	time_since_last_keyboard_rotation = camera->time_since_last_keyboard_rotation;
	time_since_last_mouse_rotation = camera->time_since_last_mouse_rotation;
	time_since_last_mouse_move = camera->time_since_last_mouse_move;
	time_since_last_agent_selection = camera->time_since_last_agent_selection;
	time_in_the_map = camera->time_in_the_map;
	time_in_the_district = camera->time_in_the_district;
	yaw_to_go = camera->yaw_to_go;
	pitch_to_go = camera->pitch_to_go;
	dist_to_go = camera->dist_to_go;
	max_distance2 = camera->max_distance2;
	h0070.push_back(camera->h0070[0]);
	h0070.push_back(camera->h0070[1]);
	position = Point3D(camera->position.x, camera->position.y, camera->position.z);
	camera_pos_to_go = Point3D(camera->camera_pos_to_go.x, camera->camera_pos_to_go.y, camera->camera_pos_to_go.z);
	cam_pos_inverted = Point3D(camera->cam_pos_inverted.x, camera->cam_pos_inverted.y, camera->cam_pos_inverted.z);
	cam_pos_inverted_to_go = Point3D(camera->cam_pos_inverted_to_go.x, camera->cam_pos_inverted_to_go.y, camera->cam_pos_inverted_to_go.z);
	look_at_target = Point3D(camera->look_at_target.x, camera->look_at_target.y, camera->look_at_target.z);
	look_at_to_go = Point3D(camera->look_at_to_go.x, camera->look_at_to_go.y, camera->look_at_to_go.z);
	field_of_view = camera->field_of_view;
	field_of_view2 = camera->field_of_view2;
}

void PyCamera::SetYaw(float _yaw) {
    GW::GameThread::Enqueue([_yaw] {
        const auto camera = GW::CameraMgr::GetCamera();
        camera->SetYaw(_yaw);
        });
    
}

void PyCamera::SetPitch(float _pitch) {
	GW::GameThread::Enqueue([_pitch] {
		const auto camera = GW::CameraMgr::GetCamera();
		camera->SetPitch(_pitch);
		});
}

void PyCamera::SetCameraPos(float x, float y, float z) {
	GW::GameThread::Enqueue([x, y, z] {
		const auto camera = GW::CameraMgr::GetCamera();
		camera->position = GW::Vec3f(x, y, z);
		});
}

void PyCamera::SetLookAtTarget(float x, float y, float z) {
	GW::GameThread::Enqueue([x, y, z] {
		const auto camera = GW::CameraMgr::GetCamera();
		camera->look_at_target = GW::Vec3f(x, y, z);
		});
}

void PyCamera::SetMaxDist(float dist) {
	GW::GameThread::Enqueue([dist] {
        GW::CameraMgr::SetMaxDist(dist);
		});
}

void PyCamera::SetFieldOfView(float fov) {
	GW::GameThread::Enqueue([fov] {
		GW::CameraMgr::SetFieldOfView(fov);

		});
}

void PyCamera::UnlockCam(bool unlock) {
	GW::GameThread::Enqueue([unlock] {
		GW::CameraMgr::UnlockCam(unlock);
		});
}

bool PyCamera::GetCameraUnlock() {
	return GW::CameraMgr::GetCameraUnlock();
}

void PyCamera::SetFog(bool fog) {
	GW::GameThread::Enqueue([fog] {
		GW::CameraMgr::SetFog(fog);
		});
}

void PyCamera::ForwardMovement(float amount, bool true_fordward) {
	GW::GameThread::Enqueue([amount, true_fordward] {
		GW::CameraMgr::ForwardMovement(amount, true_fordward);
		});
}

void PyCamera::VerticalMovement(float amount) {
	GW::GameThread::Enqueue([amount] {
		GW::CameraMgr::VerticalMovement(amount);
		});
}

void PyCamera::SideMovement(float amount) {
	GW::GameThread::Enqueue([amount] {
		GW::CameraMgr::SideMovement(amount);
		});
}

void PyCamera::RotateMovement(float angle) {
	GW::GameThread::Enqueue([angle] {
		GW::CameraMgr::RotateMovement(angle);
		});
}

Point3D PyCamera::ComputeCameraPos() {
	auto  camera_pos = GW::CameraMgr::ComputeCamPos();
	return Point3D(camera_pos.x, camera_pos.y, camera_pos.z);
}

void PyCamera::UpdateCameraPos() {
	GW::GameThread::Enqueue([] {
		GW::CameraMgr::UpdateCameraPos();
		});
}

void bind_camera(py::module_& m) {
    py::class_<PyCamera>(m, "PyCamera")
        .def(py::init<>())  // default constructor

        // Read/write fields
        .def_readwrite("look_at_agent_id", &PyCamera::look_at_agent_id)
        .def_readwrite("yaw", &PyCamera::yaw)
        .def_readwrite("pitch", &PyCamera::pitch)
        .def_readwrite("camera_zoom", &PyCamera::camera_zoom)
        .def_readwrite("max_distance", &PyCamera::max_distance)
        .def_readwrite("yaw_right_click", &PyCamera::yaw_right_click)
        .def_readwrite("yaw_right_click2", &PyCamera::yaw_right_click2)
        .def_readwrite("pitch_right_click", &PyCamera::pitch_right_click)
        .def_readwrite("distance2", &PyCamera::distance2)
        .def_readwrite("acceleration_constant", &PyCamera::acceleration_constant)
        .def_readwrite("time_since_last_keyboard_rotation", &PyCamera::time_since_last_keyboard_rotation)
        .def_readwrite("time_since_last_mouse_rotation", &PyCamera::time_since_last_mouse_rotation)
        .def_readwrite("time_since_last_mouse_move", &PyCamera::time_since_last_mouse_move)
        .def_readwrite("time_since_last_agent_selection", &PyCamera::time_since_last_agent_selection)
        .def_readwrite("time_in_the_map", &PyCamera::time_in_the_map)
        .def_readwrite("time_in_the_district", &PyCamera::time_in_the_district)
        .def_readwrite("yaw_to_go", &PyCamera::yaw_to_go)
        .def_readwrite("pitch_to_go", &PyCamera::pitch_to_go)
        .def_readwrite("dist_to_go", &PyCamera::dist_to_go)
        .def_readwrite("max_distance2", &PyCamera::max_distance2)
        .def_readwrite("field_of_view", &PyCamera::field_of_view)
        .def_readwrite("field_of_view2", &PyCamera::field_of_view2)
        .def_readwrite("h0024", &PyCamera::h0024)
        .def_readwrite("h0070", &PyCamera::h0070)
        .def_readwrite("position", &PyCamera::position)
        .def_readwrite("camera_pos_to_go", &PyCamera::camera_pos_to_go)
        .def_readwrite("cam_pos_inverted", &PyCamera::cam_pos_inverted)
        .def_readwrite("cam_pos_inverted_to_go", &PyCamera::cam_pos_inverted_to_go)
        .def_readwrite("look_at_target", &PyCamera::look_at_target)
        .def_readwrite("look_at_to_go", &PyCamera::look_at_to_go)

        // Methods (bound instance)
        .def("SetYaw", &PyCamera::SetYaw, py::arg("_yaw"))
        .def("SetPitch", &PyCamera::SetPitch, py::arg("_pitch"))
        .def("SetMaxDist", &PyCamera::SetMaxDist, py::arg("dist"))
        .def("SetFieldOfView", &PyCamera::SetFieldOfView, py::arg("fov"))
        .def("UnlockCam", &PyCamera::UnlockCam, py::arg("unlock"))
        .def("GetCameraUnlock", &PyCamera::GetCameraUnlock)
        .def("ForwardMovement", &PyCamera::ForwardMovement, py::arg("amount"), py::arg("true_forward"))
        .def("VerticalMovement", &PyCamera::VerticalMovement, py::arg("amount"))
        .def("SideMovement", &PyCamera::SideMovement, py::arg("amount"))
        .def("RotateMovement", &PyCamera::RotateMovement, py::arg("angle"))
        .def("ComputeCameraPos", &PyCamera::ComputeCameraPos)
        .def("UpdateCameraPos", &PyCamera::UpdateCameraPos)
        .def("SetCameraPos", &PyCamera::SetCameraPos, py::arg("x"), py::arg("y"), py::arg("z"))
        .def("SetLookAtTarget", &PyCamera::SetLookAtTarget, py::arg("x"), py::arg("y"), py::arg("z"));
}


PYBIND11_EMBEDDED_MODULE(PyCamera, m) {
    bind_camera(m);
}

using namespace DirectX;

void Py2DRenderer::set_primitives(const std::vector<std::vector<Point2D>>& prims, D3DCOLOR draw_color) {
    primitives = prims;
    color = draw_color;
}

void Py2DRenderer::set_world_zoom_x(float zoom) { world_zoom_x = zoom; }
void Py2DRenderer::set_world_zoom_y(float zoom) { world_zoom_y = zoom; }
void Py2DRenderer::set_world_pan(float x, float y) { world_pan_x = x; world_pan_y = y; }
void Py2DRenderer::set_world_rotation(float r) { world_rotation = r; }
void Py2DRenderer::set_world_space(bool enabled) { world_space = enabled; }
void Py2DRenderer::set_world_scale(float x) { world_scale = x; }

void Py2DRenderer::set_screen_offset(float x, float y) { screen_offset_x = x; screen_offset_y = y; }
void Py2DRenderer::set_screen_zoom_x(float zoom) { screen_zoom_x = zoom; }
void Py2DRenderer::set_screen_zoom_y(float zoom) { screen_zoom_y = zoom; }
void Py2DRenderer::set_screen_rotation(float r) { screen_rotation = r; }

void Py2DRenderer::set_circular_mask(bool enabled) { use_circular_mask = enabled; }
void Py2DRenderer::set_circular_mask_radius(float radius) { mask_radius = radius; }
void Py2DRenderer::set_circular_mask_center(float x, float y) { mask_center_x = x; mask_center_y = y; }

void Py2DRenderer::set_rectangle_mask(bool enabled) { use_rectangle_mask = enabled; }
void Py2DRenderer::set_rectangle_mask_bounds(float x, float y, float width, float height) {
	mask_rect_x = x;
	mask_rect_y = y;
	mask_rect_width = width;
    mask_rect_height = height;
}

void Py2DRenderer::SetupProjection() {
    if (!g_d3d_device) return;

    D3DVIEWPORT9 vp;
    g_d3d_device->GetViewport(&vp);

    float w = 5000.0f * 2;

    XMMATRIX ortho(
        2.0f / w, 0, 0, 0,
        0, 2.0f / w, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

    //float xscale = 1.0f;
    //float yscale = static_cast<float>(vp.Width) / static_cast<float>(vp.Height);

    float xscale = world_scale;
    float yscale = (static_cast<float>(vp.Width) / static_cast<float>(vp.Height)) * world_scale;


    float xtrans = static_cast<float>(0); // optional pan offset
    float ytrans = static_cast<float>(0); // optional pan offset

    XMMATRIX vp_matrix(
        xscale, 0, 0, 0,
        0, yscale, 0, 0,
        0, 0, 1, 0,
        xtrans, ytrans, 0, 1
    );

    XMMATRIX proj = ortho * vp_matrix;

    g_d3d_device->SetTransform(D3DTS_PROJECTION, reinterpret_cast<const D3DMATRIX*>(&proj));
}

void Py2DRenderer::ApplyStencilMask() {
    if (!g_d3d_device) return;

    auto FillRect = [](float x, float y, float w, float h, D3DCOLOR color) {
        D3DVertex vertices[4] = {
            {x,     y,     0.0f, 1.0f, color},
            {x + w, y,     0.0f, 1.0f, color},
            {x,     y + h, 0.0f, 1.0f, color},
            {x + w, y + h, 0.0f, 1.0f, color}
        };
        g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
        };

    auto FillCircle = [](float x, float y, float radius, D3DCOLOR color, unsigned resolution = 192u) {
        D3DVertex vertices[193];
        for (unsigned i = 0; i <= resolution; ++i) {
            float angle = static_cast<float>(i) / resolution * XM_2PI;
            vertices[i] = {
                x + radius * cosf(angle),
                y + radius * sinf(angle),
                0.0f,
                1.0f,
                color
            };
        }
        g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, resolution, vertices, sizeof(D3DVertex));
        };

    g_d3d_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    g_d3d_device->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
    g_d3d_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
    g_d3d_device->Clear(0, nullptr, D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);

    g_d3d_device->SetRenderState(D3DRS_STENCILREF, 1);
    g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
    g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

    if (use_circular_mask)
        FillCircle(mask_center_x, mask_center_y, mask_radius, D3DCOLOR_ARGB(0, 0, 0, 0));
    else if (use_rectangle_mask)
        FillRect(mask_rect_x, mask_rect_y, mask_rect_width, mask_rect_height, D3DCOLOR_ARGB(0, 0, 0, 0));
    else
        FillRect(-5000.0f, -5000.0f, 10000.0f, 10000.0f, D3DCOLOR_ARGB(0, 0, 0, 0));

    g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
    g_d3d_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
}

void Py2DRenderer::ResetStencilMask() {
    if (!g_d3d_device) return;
    g_d3d_device->SetRenderState(D3DRS_STENCILREF, 0);
    g_d3d_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0x00000000);
    g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NEVER);
    g_d3d_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
    g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    g_d3d_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
}


void Py2DRenderer::render() {
    if (!g_d3d_device || primitives.empty()) return;

    IDirect3DStateBlock9* state_block = nullptr;
    if (FAILED(g_d3d_device->CreateStateBlock(D3DSBT_ALL, &state_block))) return;

    D3DMATRIX old_world, old_view, old_proj;
    g_d3d_device->GetTransform(D3DTS_WORLD, &old_world);
    g_d3d_device->GetTransform(D3DTS_VIEW, &old_view);
    g_d3d_device->GetTransform(D3DTS_PROJECTION, &old_proj);

    if (world_space) {
        SetupProjection();
        D3DVIEWPORT9 vp;
        g_d3d_device->GetViewport(&vp);

        // World width used in projection
        float world_width = 5000.0f * 2.0f;

        auto translate = XMMatrixTranslation(-world_pan_x, -world_pan_y, 0);
        auto rotateZ = XMMatrixRotationZ(world_rotation);
        auto zoom = XMMatrixScaling(world_zoom_x, world_zoom_y, 1.0f);
        auto scale = XMMatrixScaling(world_scale, world_scale, 1.0f);
        auto flipY = XMMatrixScaling(1.0f, -1.0f, 1.0f); //flip happens HERE

        auto view = translate * rotateZ * scale * flipY * zoom;


        g_d3d_device->SetTransform(D3DTS_VIEW, reinterpret_cast<const D3DMATRIX*>(&view));
        g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    }
    else {
        g_d3d_device->SetTransform(D3DTS_VIEW, &old_view);
        g_d3d_device->SetTransform(D3DTS_PROJECTION, &old_proj);
        g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    }

    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing
    g_d3d_device->SetIndices(nullptr);
    g_d3d_device->SetFVF(D3DFVF_CUSTOMVERTEX);
    g_d3d_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
    g_d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_d3d_device->SetPixelShader(nullptr);
    g_d3d_device->SetVertexShader(nullptr);
    g_d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_d3d_device->SetRenderState(D3DRS_LIGHTING, false);
    g_d3d_device->SetRenderState(D3DRS_ZENABLE, false);
    g_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    g_d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    g_d3d_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    g_d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g_d3d_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    g_d3d_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    auto FillRect = [](float x, float y, float w, float h, D3DCOLOR color) {
        if (!g_d3d_device) return;
        D3DVertex vertices[4] = {
            {x,     y,     0.0f, 1.0f, color},
            {x + w, y,     0.0f, 1.0f, color},
            {x,     y + h, 0.0f, 1.0f, color},
            {x + w, y + h, 0.0f, 1.0f, color}
        };
        g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
        };

    auto FillCircle = [](float x, float y, float radius, D3DCOLOR color, unsigned resolution = 192u) {
        if (!g_d3d_device) return;
        D3DVertex vertices[193];
        for (unsigned i = 0; i <= resolution; ++i) {
            float angle = static_cast<float>(i) / resolution * XM_2PI;
            vertices[i] = {
                x + radius * cosf(angle),
                y + radius * sinf(angle),
                0.0f,
                1.0f,
                color
            };
        }
        g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, resolution, vertices, sizeof(D3DVertex));
        };

    g_d3d_device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);

    if (use_circular_mask) {
        g_d3d_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
        g_d3d_device->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
        g_d3d_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
        g_d3d_device->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
        g_d3d_device->SetRenderState(D3DRS_STENCILREF, 1);
        g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
        g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

        FillCircle(mask_center_x, mask_center_y, mask_radius, D3DCOLOR_ARGB(0, 0, 0, 0));

        g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
        g_d3d_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
        g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    }
	else if (use_rectangle_mask) {
        g_d3d_device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
        g_d3d_device->SetRenderState(D3DRS_STENCILMASK, 0xFFFFFFFF);
        g_d3d_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
        g_d3d_device->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
        g_d3d_device->SetRenderState(D3DRS_STENCILREF, 1);
        g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
        g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

        FillRect(mask_rect_x, mask_rect_y, mask_rect_width, mask_rect_height, D3DCOLOR_ARGB(0, 0, 0, 0));

        // Set stencil test to only draw where the rect was filled
        g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
        g_d3d_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
        g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
    }
    else {
        FillRect(-5000.0f, -5000.0f, 10000.0f, 10000.0f, D3DCOLOR_ARGB(0, 0, 0, 0));
    }

    float cos_r = cosf(world_rotation);
    float sin_r = sinf(world_rotation);

    for (const auto& shape : primitives) {
        if (shape.size() != 3 && shape.size() != 4) continue;

        D3DVertex verts[4];
        for (size_t i = 0; i < shape.size(); ++i) {
            float x = shape[i].x;
            float y = shape[i].y;

            float x_out = x, y_out = y;
            if (world_space) {
                x_out = x * cos_r - y * sin_r;
                y_out = x * sin_r + y * cos_r;

                x_out *= world_scale;
                y_out *= world_scale;

                x_out = x_out * world_zoom_x + world_pan_x + screen_offset_x;
                y_out = y_out * world_zoom_y + world_pan_y + screen_offset_y;
            }

            verts[i] = {
                x_out, y_out, 0.0f, 1.0f, color
            };
        }

        if (shape.size() == 4)
            g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, verts, sizeof(D3DVertex));
        else
            g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, verts, sizeof(D3DVertex));
    }

    if (use_circular_mask || use_rectangle_mask) {
        g_d3d_device->SetRenderState(D3DRS_STENCILREF, 0);
        g_d3d_device->SetRenderState(D3DRS_STENCILWRITEMASK, 0x00000000);
        g_d3d_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NEVER);
        g_d3d_device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
        g_d3d_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
        g_d3d_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    }

    g_d3d_device->SetTransform(D3DTS_WORLD, &old_world);
    g_d3d_device->SetTransform(D3DTS_VIEW, &old_view);
    g_d3d_device->SetTransform(D3DTS_PROJECTION, &old_proj);

    state_block->Apply();
    state_block->Release();
}

void Py2DRenderer::DrawLine(Point2D from, Point2D to, D3DCOLOR color, float thickness) {
    if (!g_d3d_device) return;

    // Ensure 2D screen-space rendering is not affected by Z-buffer
    g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    if (thickness <= 1.0f) {
        D3DVertex verts[] = {
            { float(from.x), float(from.y), 0.0f, 1.0f, color },
            { float(to.x),   float(to.y),   0.0f, 1.0f, color }
        };
        g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
        g_d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, verts, sizeof(D3DVertex));
        return;
    }

    float dx = to.x - from.x;
    float dy = to.y - from.y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len == 0.0f) return;

    float nx = -dy / len;
    float ny = dx / len;
    float half = thickness * 0.5f;

    D3DVertex quad[] = {
        { from.x - nx * half, from.y - ny * half, 0.0f, 1.0f, color },
        { from.x + nx * half, from.y + ny * half, 0.0f, 1.0f, color },
        { to.x - nx * half, to.y - ny * half, 0.0f, 1.0f, color },
        { to.x + nx * half, to.y + ny * half, 0.0f, 1.0f, color }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(D3DVertex));
}


void Py2DRenderer::DrawTriangle(Point2D p1, Point2D p2, Point2D p3, D3DCOLOR color, float thickness) {
    DrawLine(p1, p2, color, thickness);
    DrawLine(p2, p3, color, thickness);
    DrawLine(p3, p1, color, thickness);
}

void Py2DRenderer::DrawTriangleFilled(Point2D p1, Point2D p2, Point2D p3, D3DCOLOR color) {
    // Ensure 2D screen-space rendering is not affected by Z-buffer
    g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    D3DVertex verts[] = {
        { float(p1.x), float(p1.y), 0.0f, 1.0f, color },
        { float(p2.x), float(p2.y), 0.0f, 1.0f, color },
        { float(p3.x), float(p3.y), 0.0f, 1.0f, color }
    };
    g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, verts, sizeof(D3DVertex));
}


void Py2DRenderer::DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4, D3DCOLOR color, float thickness) {
    DrawLine(p1, p2, color, thickness);
    DrawLine(p2, p3, color, thickness);
    DrawLine(p3, p4, color, thickness);
    DrawLine(p4, p1, color, thickness);
}


void Py2DRenderer::DrawQuadFilled(Point2D p1, Point2D p2, Point2D p3, Point2D p4, D3DCOLOR color) {
    // Ensure 2D screen-space rendering is not affected by Z-buffer
    g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    D3DVertex verts[6] = {
        { float(p1.x), float(p1.y), 0.0f, 1.0f, color },
        { float(p2.x), float(p2.y), 0.0f, 1.0f, color },
        { float(p3.x), float(p3.y), 0.0f, 1.0f, color },
        { float(p3.x), float(p3.y), 0.0f, 1.0f, color },
        { float(p4.x), float(p4.y), 0.0f, 1.0f, color },
        { float(p1.x), float(p1.y), 0.0f, 1.0f, color }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, verts, sizeof(D3DVertex));
}


void Py2DRenderer::DrawPoly(Point2D center, float radius, D3DCOLOR color, int segments, float thickness) {
    float step = 2.0f * 3.141592654 / segments;
    for (int i = 0; i < segments; ++i) {
        float a0 = step * i;
        float a1 = step * (i + 1);
        Point2D p1(center.x + cosf(a0) * radius, center.y + sinf(a0) * radius);
        Point2D p2(center.x + cosf(a1) * radius, center.y + sinf(a1) * radius);
        DrawLine(p1, p2, color, thickness);
    }
}

void Py2DRenderer::DrawPolyFilled(Point2D center, float radius, D3DCOLOR color, int segments) {
    // Ensure 2D screen-space rendering is not affected by Z-buffer
    g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_d3d_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_d3d_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    std::vector<D3DVertex> verts;
    verts.emplace_back(D3DVertex{ static_cast<float>(center.x), static_cast<float>(center.y), 0.0f, 1.0f, color });

    float step = 2.0f * 3.141592654 / segments;
    for (int i = 0; i <= segments; ++i) {
        float angle = step * i;
        float x = center.x + cosf(angle) * radius;
        float y = center.y + sinf(angle) * radius;
        verts.emplace_back(D3DVertex{ x, y, 0.0f, 1.0f, color });
    }

    g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, segments, verts.data(), sizeof(D3DVertex));
}


void Py2DRenderer::Setup3DView() {
    if (!g_d3d_device) return;

    // Lighting and blending
    g_d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_d3d_device->SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);

    // Depth testing
    g_d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    g_d3d_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

    auto* cam = GW::CameraMgr::GetCamera();
    if (!cam) return;

    // Right-handed camera vectors (flip Z)
    D3DXVECTOR3 eye(cam->position.x, cam->position.y, -cam->position.z);
    D3DXVECTOR3 at(cam->look_at_target.x, cam->look_at_target.y, -cam->look_at_target.z);
    D3DXVECTOR3 up(0.0f, 0.0f, 1.0f); // RH: Z+ is up

    D3DXMATRIX view;
    D3DXMatrixLookAtRH(&view, &eye, &at, &up);
    g_d3d_device->SetTransform(D3DTS_VIEW, &view);

    float fov = GW::Render::GetFieldOfView();
    float aspect = static_cast<float>(GW::Render::GetViewportWidth()) / static_cast<float>(GW::Render::GetViewportHeight());
    float near_plane = 48000.0f / 1024.0f;
    float far_plane = 48000.0f;

    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovRH(&proj, fov, aspect, near_plane, far_plane);
    g_d3d_device->SetTransform(D3DTS_PROJECTION, &proj);

    //Set identity world transform
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    g_d3d_device->SetTransform(D3DTS_WORLD, &identity);
   
}


void Py2DRenderer::DrawLine3D(Point3D from, Point3D to, D3DCOLOR color, bool use_occlusion) {
    if (!g_d3d_device) return;

    Setup3DView();

	if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

    // Flip Z to match RH system (because the view/projection flips Z)
    from.z = -from.z;
    to.z = -to.z;

    struct D3DVertex3D {
        float x, y, z;
        D3DCOLOR color;
    };

    D3DVertex3D verts[] = {
        { from.x, from.y, from.z, color },
        { to.x,   to.y,   to.z,   color }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, verts, sizeof(D3DVertex3D));
}

void Py2DRenderer::DrawTriangle3D(Point3D p1, Point3D p2, Point3D p3, D3DCOLOR color, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();
    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    struct D3DVertex3D { float x, y, z; D3DCOLOR color; };
    D3DVertex3D verts[] = {
        { p1.x, p1.y, -p1.z, color },
        { p2.x, p2.y, -p2.z, color },
        { p3.x, p3.y, -p3.z, color }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 2, verts, sizeof(D3DVertex3D));
    g_d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, &verts[2], sizeof(D3DVertex3D));
}

void Py2DRenderer::DrawTriangleFilled3D(Point3D p1, Point3D p2, Point3D p3, D3DCOLOR color, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();
    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    struct D3DVertex3D { float x, y, z; D3DCOLOR color; };
    D3DVertex3D verts[] = {
        { p1.x, p1.y, -p1.z, color },
        { p2.x, p2.y, -p2.z, color },
        { p3.x, p3.y, -p3.z, color }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, verts, sizeof(D3DVertex3D));
}

void Py2DRenderer::DrawQuad3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, D3DCOLOR color, bool use_occlusion) {
    DrawTriangle3D(p1, p2, p3, color, use_occlusion);
    DrawTriangle3D(p3, p4, p1, color, use_occlusion);
}

void Py2DRenderer::DrawQuadFilled3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, D3DCOLOR color, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();
    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    struct D3DVertex3D { float x, y, z; D3DCOLOR color; };
    D3DVertex3D verts[] = {
        { p1.x, p1.y, -p1.z, color },
        { p2.x, p2.y, -p2.z, color },
        { p3.x, p3.y, -p3.z, color },
        { p3.x, p3.y, -p3.z, color },
        { p4.x, p4.y, -p4.z, color },
        { p1.x, p1.y, -p1.z, color }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, verts, sizeof(D3DVertex3D));
}

void Py2DRenderer::DrawPoly3D(Point3D center, float radius, D3DCOLOR color, int segments, bool autoZ, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();
    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    struct D3DVertex3D { float x, y, z; D3DCOLOR color; };

    std::vector<Point3D> points;
    const float step = XM_2PI / segments;
    for (int i = 0; i <= segments; ++i) {
        float angle = step * i;
        float x = center.x + cosf(angle) * radius;
        float y = center.y + sinf(angle) * radius;
        float z = autoZ ? overlay.findZ(x, y, center.z) : center.z;
        points.push_back({ x, y, z });
    }

    for (int i = 0; i < segments; ++i) {
        D3DVertex3D verts[] = {
            { points[i].x, points[i].y, -points[i].z, color },
            { points[i + 1].x, points[i + 1].y, -points[i + 1].z, color }
        };
        g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
        g_d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, verts, sizeof(D3DVertex3D));
    }
}

void Py2DRenderer::DrawPolyFilled3D(Point3D center, float radius, D3DCOLOR color, int segments, bool autoZ, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();
    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    struct D3DVertex3D { float x, y, z; D3DCOLOR color; };

    std::vector<D3DVertex3D> verts;
    verts.push_back({ center.x, center.y, -center.z, color }); // center of the fan

    const float step = XM_2PI / segments;
    for (int i = 0; i <= segments; ++i) {
        float angle = step * i;
        float x = center.x + cosf(angle) * radius;
        float y = center.y + sinf(angle) * radius;
        float z = autoZ ? overlay.findZ(x, y, center.z) : center.z;
        verts.push_back({ x, y, -z, color });
    }

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, segments, verts.data(), sizeof(D3DVertex3D));
}

void Py2DRenderer::DrawCubeOutline(Point3D center, float size, D3DCOLOR color, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();

    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    float h = size * 0.5f;

    Point3D c[8] = {
        {center.x - h, center.y - h, center.z - h},
        {center.x + h, center.y - h, center.z - h},
        {center.x + h, center.y + h, center.z - h},
        {center.x - h, center.y + h, center.z - h},
        {center.x - h, center.y - h, center.z + h},
        {center.x + h, center.y - h, center.z + h},
        {center.x + h, center.y + h, center.z + h},
        {center.x - h, center.y + h, center.z + h}
    };

    const int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},
        {4,5},{5,6},{6,7},{7,4},
        {0,4},{1,5},{2,6},{3,7}
    };

    for (int i = 0; i < 12; ++i) {
        DrawLine3D(c[edges[i][0]], c[edges[i][1]], color, use_occlusion);
    }
}

void Py2DRenderer::DrawCubeFilled(Point3D center, float size, D3DCOLOR color, bool use_occlusion) {
    if (!g_d3d_device) return;
    Setup3DView();

    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    float h = size * 0.5f;

    Point3D c[8] = {
        {center.x - h, center.y - h, center.z - h},
        {center.x + h, center.y - h, center.z - h},
        {center.x + h, center.y + h, center.z - h},
        {center.x - h, center.y + h, center.z - h},
        {center.x - h, center.y - h, center.z + h},
        {center.x + h, center.y - h, center.z + h},
        {center.x + h, center.y + h, center.z + h},
        {center.x - h, center.y + h, center.z + h}
    };

    const int faces[6][4] = {
        {0,1,2,3}, // back
        {4,5,6,7}, // front
        {0,1,5,4}, // bottom
        {2,3,7,6}, // top
        {1,2,6,5}, // right
        {0,3,7,4}  // left
    };

    for (int i = 0; i < 6; ++i) {
        DrawQuadFilled3D(c[faces[i][0]], c[faces[i][1]], c[faces[i][2]], c[faces[i][3]], color, use_occlusion);
    }
}


void bind_2drenderer(py::module_& m) {
    py::class_<Py2DRenderer>(m, "Py2DRenderer")
        .def(py::init<>())
        .def("set_primitives", &Py2DRenderer::set_primitives, py::arg("primitives"), py::arg("draw_color") = 0xFFFFFFFF)
        .def("set_world_zoom_x", &Py2DRenderer::set_world_zoom_x, py::arg("zoom"))
        .def("set_world_zoom_y", &Py2DRenderer::set_world_zoom_y, py::arg("zoom"))
        .def("set_world_pan", &Py2DRenderer::set_world_pan, py::arg("x"), py::arg("y"))
        .def("set_world_rotation", &Py2DRenderer::set_world_rotation, py::arg("r"))
        .def("set_world_space", &Py2DRenderer::set_world_space, py::arg("enabled"))
		.def("set_world_scale", &Py2DRenderer::set_world_scale, py::arg("scale"))

        .def("set_screen_offset", &Py2DRenderer::set_screen_offset, py::arg("x"), py::arg("y"))
        .def("set_screen_zoom_x", &Py2DRenderer::set_screen_zoom_x, py::arg("zoom"))
        .def("set_screen_zoom_y", &Py2DRenderer::set_screen_zoom_y, py::arg("zoom"))
        .def("set_screen_rotation", &Py2DRenderer::set_screen_rotation, py::arg("r"))

        .def("set_circular_mask", &Py2DRenderer::set_circular_mask, py::arg("enabled"))
        .def("set_circular_mask_radius", &Py2DRenderer::set_circular_mask_radius, py::arg("radius"))
        .def("set_circular_mask_center", &Py2DRenderer::set_circular_mask_center, py::arg("x"), py::arg("y"))

        .def("set_rectangle_mask", &Py2DRenderer::set_rectangle_mask, py::arg("enabled"))
        .def("set_rectangle_mask_bounds", &Py2DRenderer::set_rectangle_mask_bounds, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"))

        .def("render", &Py2DRenderer::render)


        .def("DrawLine", &Py2DRenderer::DrawLine, py::arg("from"), py::arg("to"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
        .def("DrawTriangle", &Py2DRenderer::DrawTriangle, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
        .def("DrawTriangleFilled", &Py2DRenderer::DrawTriangleFilled, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF)
        .def("DrawQuad", &Py2DRenderer::DrawQuad, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF, py::arg("thickness") = 1.0f)
        .def("DrawQuadFilled", &Py2DRenderer::DrawQuadFilled, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF)
        .def("DrawPoly", &Py2DRenderer::DrawPoly, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("segments") = 3, py::arg("thickness") = 1.0f)
        .def("DrawPolyFilled", &Py2DRenderer::DrawPolyFilled, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("segments") = 3)
        .def("DrawCubeOutline", &Py2DRenderer::DrawCubeOutline, py::arg("center"), py::arg("size"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)
        .def("DrawCubeFilled", &Py2DRenderer::DrawCubeFilled, py::arg("center"), py::arg("size"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)

        .def("DrawLine3D", &Py2DRenderer::DrawLine3D, py::arg("from"), py::arg("to"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)
        .def("DrawTriangle3D", &Py2DRenderer::DrawTriangle3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)
        .def("DrawTriangleFilled3D", &Py2DRenderer::DrawTriangleFilled3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)
        .def("DrawQuad3D", &Py2DRenderer::DrawQuad3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)
        .def("DrawQuadFilled3D", &Py2DRenderer::DrawQuadFilled3D, py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"), py::arg("color") = 0xFFFFFFFF, py::arg("use_occlusion") = true)
        .def("DrawPoly3D", &Py2DRenderer::DrawPoly3D, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("segments") = 3, py::arg("autoZ") = true, py::arg("use_occlusion") = true)
        .def("DrawPolyFilled3D", &Py2DRenderer::DrawPolyFilled3D, py::arg("center"), py::arg("radius"), py::arg("color") = 0xFFFFFFFF, py::arg("segments") = 3, py::arg("autoZ") = true, py::arg("use_occlusion") = true)

        .def("Setup3DView", &Py2DRenderer::Setup3DView)
        .def("ApplyStencilMask", &Py2DRenderer::ApplyStencilMask)
        .def("ResetStencilMask", &Py2DRenderer::ResetStencilMask);
}

PYBIND11_EMBEDDED_MODULE(Py2DRenderer, m) {
    bind_2drenderer(m);
}
