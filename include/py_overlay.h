#pragma once
#include "Headers.h"
using namespace DirectX;


constexpr auto ALTITUDE_UNKNOWN = std::numeric_limits<float>::max();

extern GW::Vec3f MouseWorldPos;
extern GW::Vec2f MouseScreenPos;

struct TimedTexture {
    IDirect3DTexture9* texture = nullptr;
    std::wstring name;
    std::chrono::steady_clock::time_point last_used;

    TimedTexture() = default;

    TimedTexture(IDirect3DTexture9* tex, const std::wstring& key)
        : texture(tex), name(key), last_used(std::chrono::steady_clock::now()) {
    }

    void Touch() {
        last_used = std::chrono::steady_clock::now();
    }
};

class TextureManager {
public:
    static TextureManager& Instance() {
        static TextureManager instance;
        return instance;
    }

    // Assign device before any texture use
    void SetDevice(IDirect3DDevice9* device) {
        d3d_device = device;
    }

    void AddTexture(const std::wstring& name, IDirect3DTexture9* texture) {
        textures[name] = TimedTexture(texture, name);
    }

    IDirect3DTexture9* GetTexture(const std::wstring& name) {
        auto it = textures.find(name);
        if (it != textures.end()) {
            it->second.Touch();
            return it->second.texture;
        }

        // Lazy-load if not found
        if (!d3d_device && g_d3d_device)
            SetDevice(g_d3d_device);

        if (!d3d_device)
            return nullptr;

        IDirect3DTexture9* new_texture = nullptr;
        if (D3DXCreateTextureFromFileW(d3d_device, name.c_str(), &new_texture) == D3D_OK && new_texture) {
            AddTexture(name, new_texture);
            return new_texture;
        }

        return nullptr;
    }

    void CleanupOldTextures(int timeout_seconds = 30) {
        auto now = std::chrono::steady_clock::now();
        for (auto it = textures.begin(); it != textures.end(); ) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.last_used).count();
            if (duration > timeout_seconds) {
                if (it->second.texture) it->second.texture->Release();
                it = textures.erase(it);
            }
            else {
                ++it;
            }
        }
    }

private:
    std::unordered_map<std::wstring, TimedTexture> textures;
    IDirect3DDevice9* d3d_device = nullptr;
};











struct GlobalMouseClass {
    void SetMouseWorldPos(float x, float y, float z);
    GW::Vec3f GetMouseWorldPos();

    void SetMouseCoords(float x, float y);
    GW::Vec2f GetMouseCoords();
};

class Point2D{
public:
    int x = 0;
    int y = 0;
    Point2D() = default; // Default constructor
    Point2D(int x, int y) : x(x), y(y) {};
};

class Point3D {
public:
    float x = 0;
    float y = 0;
    float z = 0;
    Point3D() = default; // Default constructor
    Point3D(float x, float y, float z) : x(x), y(y), z(z) {};
};

struct D3DVertex {
    float x, y, z, rhw;
    D3DCOLOR color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)


class Overlay {
public:
    ImDrawList* drawList;

    XMMATRIX CreateViewMatrix(const XMFLOAT3& eye_pos, const XMFLOAT3& look_at_pos, const XMFLOAT3& up_direction);
    XMMATRIX CreateProjectionMatrix(float fov, float aspect_ratio, float near_plane, float far_plane);
    GW::Vec3f GetWorldToScreen(const GW::Vec3f& world_position, const XMMATRIX& mat_view, const XMMATRIX& mat_proj, float viewport_width, float viewport_height);
    void GetScreenToWorld();

    TextureManager texture_manager;

public:
    Overlay() : texture_manager(TextureManager::Instance()) { }
	
    void RefreshDrawList();
    Point2D GetMouseCoords();
    float findZ(float x, float y, float pz);
    Point2D WorldToScreen(float x, float y, float z);
    Point3D GetMouseWorldPos();

    
    // Game <-> World
    Point2D GamePosToWorldMap(float x, float y);
    Point2D WorlMapToGamePos(float x, float y);
    
    // World <-> Screen
	Point2D WorldMapToScreen(float x, float y);
    Point2D ScreenToWorldMap(float screen_x, float screen_y);
    
    // Game <-> Screen (combined)
	Point2D GameMapToScreen(float x, float y);
    Point2D ScreenToGameMapPos(float screen_x, float screen_y);
    
    //NormalizedScreen <-> Screen
    Point2D NormalizedScreenToScreen(float norm_x, float norm_y);
    Point2D ScreenToNormalizedScreen(float screen_x, float screen_y);
    
    //NormalizedScreen <-> World / Game
    Point2D NormalizedScreenToWorldMap(float norm_x, float norm_y);
    Point2D NormalizedScreenToGameMap(float norm_x, float norm_y);
    Point2D GamePosToNormalizedScreen(float x, float y);
    

    void BeginDraw();
	void BeginDraw(std::string name);
	void BeginDraw(std::string name, float x, float y, float width, float height);
    void EndDraw();
    void DrawLine(Point2D from, Point2D to, ImU32 color, float thickness);
    void DrawLine3D(Point3D from, Point3D to, ImU32 color, float thickness);
	void DrawTriangle(Point2D p1, Point2D p2, Point2D p3, ImU32 color, float thickness = 1.0f);
	void DrawTriangle3D(Point3D p1, Point3D p2, Point3D p3, ImU32 color, float thickness = 1.0f);
    void DrawTriangleFilled(Point2D p1, Point2D p2, Point2D p3, ImU32 color);
    void DrawTriangleFilled3D(Point3D p1, Point3D p2, Point3D p3, ImU32 color);	
    void DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4, ImU32 color, float thickness = 1.0f);
	void DrawQuad3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, ImU32 color, float thickness = 1.0f);
	void DrawQuadFilled(Point2D p1, Point2D p2, Point2D p3, Point2D p4, ImU32 color);
	void DrawQuadFilled3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, ImU32 color);
    void DrawPoly(Point2D center, float radius, ImU32 color = 0xFFFFFFFF, int numSegments = 32, float thickness = 1.0f);
	void DrawPolyFilled(Point2D center, float radius, ImU32 color = 0xFFFFFFFF, int numSegments = 32);
    void DrawPoly3D(Point3D center, float radius, ImU32 color = 0xFFFFFFFF, int numSegments = 32, float thickness = 1.0f, bool autoZ = true);
	void DrawPolyFilled3D(Point3D center, float radius, ImU32 color = 0xFFFFFFFF, int numSegments = 32, bool autoZ = true);
	void DrawCubeOutline(Point3D center, float size, ImU32 color, float thickness);
    void DrawCubeFilled(Point3D center, float size, ImU32 color);
    void DrawText2D(Point2D position, std::string text, ImU32 color, bool centered = true, float scale = 1.0f);
    void DrawText3D(Point3D position3D, std::string text, ImU32 color, bool autoZ = true, bool centered = true, float scale = 1.0f);
    void DrawTexture(const std::string& path, float width= 32.0f, float height= 32.0f) {
        std::wstring wpath(path.begin(), path.end());
        IDirect3DTexture9* tex = TextureManager::Instance().GetTexture(wpath);
        if (!tex)
            return;


        ImTextureID tex_id = reinterpret_cast<ImTextureID>(tex);
        ImGui::Image(tex_id, ImVec2(width, height));
    }
	void DrawTexturedRect(float x, float y, float width, float height, const std::string& texture_path) {
		std::wstring wpath(texture_path.begin(), texture_path.end());
		IDirect3DTexture9* tex = TextureManager::Instance().GetTexture(wpath);
		if (!tex)
			return;
		ImTextureID tex_id = reinterpret_cast<ImTextureID>(tex);
		ImGui::GetWindowDrawList()->AddImage(tex_id, ImVec2(x, y), ImVec2(x + width, y + height));
	}
	void UpkeepTextures(int timeout=30) {
		TextureManager::Instance().CleanupOldTextures(timeout);
	}

    bool ImageButton(const std::string& caption, const std::string& file_path, float width=32.0f, float height=32.0f, int frame_padding = 0);


	bool IsMouseClicked(int button);
    Point2D GetDisplaySize();
	void PushClipRect(float x, float y, float x2, float y2);
	void PopClipRect() {drawList->PopClipRect();}
};


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


class Py2DRenderer {
private:
    Overlay overlay;
public:

	void set_primitives(const std::vector<std::vector<Point2D>>& prims, D3DCOLOR color);
    void set_world_zoom_x(float zoom);
	void set_world_zoom_y(float zoom);
    void set_world_pan(float x, float y);
    void set_world_rotation(float r);
    void set_world_space(bool enabled);
	void set_world_scale(float x);

    // Screen-space offset (pixel units)
    void set_screen_offset(float x, float y);
	void set_screen_zoom_x(float zoom);
	void set_screen_zoom_y(float zoom);
	void set_screen_rotation(float r);

    void set_circular_mask(bool enabled);
    void set_circular_mask_radius(float radius);
	void set_circular_mask_center(float x, float y);

	void set_rectangle_mask(bool enabled);
	void set_rectangle_mask_bounds(float x, float y, float width, float height);

    void render();

	void Setup3DView();

    void DrawLine(Point2D from, Point2D to, D3DCOLOR color, float thickness);
    void DrawLine3D(Point3D from, Point3D to, D3DCOLOR color, bool use_occlusion=true);
    void DrawTriangle(Point2D p1, Point2D p2, Point2D p3, D3DCOLOR color, float thickness = 1.0f);
    void DrawTriangle3D(Point3D p1, Point3D p2, Point3D p3, D3DCOLOR color, bool use_occlusion = true);
    void DrawTriangleFilled(Point2D p1, Point2D p2, Point2D p3, D3DCOLOR color);
    void DrawTriangleFilled3D(Point3D p1, Point3D p2, Point3D p3, D3DCOLOR color, bool use_occlusion = true);
    void DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4, D3DCOLOR color, float thickness = 1.0f);
    void DrawQuad3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, D3DCOLOR color, bool use_occlusion = true);
    void DrawQuadFilled(Point2D p1, Point2D p2, Point2D p3, Point2D p4, D3DCOLOR color);
    void DrawQuadFilled3D(Point3D p1, Point3D p2, Point3D p3, Point3D p4, D3DCOLOR color, bool use_occlusion = true);
    void DrawPoly(Point2D center, float radius, D3DCOLOR color = 0xFFFFFFFF, int numSegments = 32, float thickness = 1.0f);
    void DrawPolyFilled(Point2D center, float radius, D3DCOLOR color = 0xFFFFFFFF, int numSegments = 32);
    void DrawPoly3D(Point3D center, float radius, D3DCOLOR color = 0xFFFFFFFF, int numSegments = 32, bool autoZ = true, bool use_occlusion = true);
    void DrawPolyFilled3D(Point3D center, float radius, D3DCOLOR color = 0xFFFFFFFF, int numSegments = 32, bool autoZ = true, bool use_occlusion = true);
    void DrawCubeOutline(Point3D center, float size, D3DCOLOR color, bool use_occlusion=true);
    void DrawCubeFilled(Point3D center, float size, D3DCOLOR color, bool use_occlusion=true);
    void DrawTexture(const std::string& file_path, float screen_pos_x, float screen_pos_y, float width, float height, uint32_t int_tint);
    void DrawTexture3D(const std::string& file_path, float world_pos_x, float world_pos_y, float world_pos_z, float width, float height, bool use_occlusion, uint32_t int_tint);
    void DrawQuadTextured3D(const std::string& file_path, Point3D p1, Point3D p2, Point3D p3, Point3D p4, bool use_occlusion, uint32_t int_tint);

    void ApplyStencilMask();
    void ResetStencilMask();


private:
    void SetupProjection();

    struct D3DVertex {
        float x, y, z, rhw;
        D3DCOLOR color;
    };

    struct D3DVertex3D {
        float x, y, z;           // No RHW
        D3DCOLOR color;
    };


    std::vector<std::vector<Point2D>> primitives;
    D3DCOLOR color;
    float world_zoom_x = 1.0f;
	float world_zoom_y = 1.0f;
    float world_pan_x = 0.0f;
    float world_pan_y = 0.0f;
	float world_scale = 1.0f;
    float world_rotation = 0.0f;
    bool world_space = true;

    // Screen transform (pixels)
    float screen_offset_x = 0.0f;
    float screen_offset_y = 0.0f;
	float screen_zoom_x = 1.0f;
	float screen_zoom_y = 1.0f;
	float screen_rotation = 0.0f;

    bool use_circular_mask = false;
    float mask_radius = 5000.0f;
	float mask_center_x = 0.0f;
	float mask_center_y = 0.0f;

	bool use_rectangle_mask = false;
	float mask_rect_x = 0.0f;
	float mask_rect_y = 0.0f;
	float mask_rect_width = 0.0f;
	float mask_rect_height = 0.0f;



	

    void Py2DRenderer::EnableDepthBuffer(bool enable) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, enable ? D3DZB_TRUE : D3DZB_FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, enable ? TRUE : FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    }


};


