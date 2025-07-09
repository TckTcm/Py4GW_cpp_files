#include "py_2d_renderer.h"


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

void Py2DRenderer::DrawTexture(const std::string& file_path, float screen_pos_x, float screen_pos_y, float width, float height, uint32_t int_tint) {
    D3DCOLOR tint = D3DCOLOR_ARGB((int_tint >> 24) & 0xFF, (int_tint >> 16) & 0xFF, (int_tint >> 8) & 0xFF, int_tint & 0xFF);
    tint = 0xFFFFFFFF; // Force full opacity for 2D textures

    if (!g_d3d_device) return;

    std::wstring wpath(file_path.begin(), file_path.end());
    LPDIRECT3DTEXTURE9 texture = overlay.texture_manager.GetTexture(wpath);
    if (!texture) return;

    float w = width * 0.5f;
    float h = height * 0.5f;

    struct Vertex {
        float x, y, z, rhw;
        float u, v;
        D3DCOLOR color;
    };

    Vertex quad[] = {
        { screen_pos_x - w, screen_pos_y - h, 0.0f, 1.0f, 0.0f, 0.0f, tint },
        { screen_pos_x + w, screen_pos_y - h, 0.0f, 1.0f, 1.0f, 0.0f, tint },
        { screen_pos_x + w, screen_pos_y + h, 0.0f, 1.0f, 1.0f, 1.0f, tint },
        { screen_pos_x - w, screen_pos_y + h, 0.0f, 1.0f, 0.0f, 1.0f, tint },
    };

    // Set render states for textures
    g_d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
    g_d3d_device->SetTexture(0, texture);

    // Set texture blending
    g_d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, quad, sizeof(Vertex));
}


void Py2DRenderer::DrawTexture3D(const std::string& file_path, float world_pos_x, float world_pos_y, float world_pos_z, float width, float height, bool use_occlusion, uint32_t int_tint) {
    D3DCOLOR tint = D3DCOLOR_ARGB((int_tint >> 24) & 0xFF, (int_tint >> 16) & 0xFF, (int_tint >> 8) & 0xFF, int_tint & 0xFF);
    tint = 0xFFFFFFFF;

    if (!g_d3d_device) return;

    std::wstring wpath(file_path.begin(), file_path.end());
    LPDIRECT3DTEXTURE9 texture = overlay.texture_manager.GetTexture(wpath);
    if (!texture) return;

    Setup3DView();

    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    // Flip Z to match RH coordinate system
    world_pos_z = -world_pos_z;

    float w = width / 2.0f;
    float h = height / 2.0f;

    struct D3DTexturedVertex3D {
        float x, y, z;
        float u, v;
        D3DCOLOR color;
    };

    D3DTexturedVertex3D verts[] = {
        { world_pos_x - w, world_pos_y - h, world_pos_z, 0.0f, 0.0f, tint },
        { world_pos_x + w, world_pos_y - h, world_pos_z, 1.0f, 0.0f, tint },
        { world_pos_x + w, world_pos_y + h, world_pos_z, 1.0f, 1.0f, tint },
        { world_pos_x - w, world_pos_y + h, world_pos_z, 0.0f, 1.0f, tint }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
    g_d3d_device->SetTexture(0, texture);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, verts, sizeof(D3DTexturedVertex3D));

    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, TRUE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    }
}

void Py2DRenderer::DrawQuadTextured3D(const std::string& file_path,
    Point3D p1, Point3D p2, Point3D p3, Point3D p4,
    bool use_occlusion,
    uint32_t int_tint) {
    D3DCOLOR tint = D3DCOLOR_ARGB((int_tint >> 24) & 0xFF, (int_tint >> 16) & 0xFF, (int_tint >> 8) & 0xFF, int_tint & 0xFF);
    tint = 0xFFFFFFFF;
    if (!g_d3d_device) return;

    std::wstring wpath(file_path.begin(), file_path.end());
    LPDIRECT3DTEXTURE9 texture = TextureManager::Instance().GetTexture(wpath);
    if (!texture) return;

    Setup3DView();

    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    }

    // Flip Z to match RH system
    p1.z = -p1.z;
    p2.z = -p2.z;
    p3.z = -p3.z;
    p4.z = -p4.z;

    struct D3DTexturedVertex3D {
        float x, y, z;
        float u, v;
        D3DCOLOR color;
    };

    D3DTexturedVertex3D verts[] = {
        { p1.x, p1.y, p1.z, 0.0f, 0.0f, tint },
        { p2.x, p2.y, p2.z, 1.0f, 0.0f, tint },
        { p3.x, p3.y, p3.z, 1.0f, 1.0f, tint },
        { p4.x, p4.y, p4.z, 0.0f, 1.0f, tint }
    };

    g_d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
    g_d3d_device->SetTexture(0, texture);
    g_d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, verts, sizeof(D3DTexturedVertex3D));

    if (!use_occlusion) {
        g_d3d_device->SetRenderState(D3DRS_ZENABLE, TRUE);
        g_d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
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
        .def("ResetStencilMask", &Py2DRenderer::ResetStencilMask)
        .def("DrawTexture", &Py2DRenderer::DrawTexture, py::arg("file_path"), py::arg("screen_pos_x"), py::arg("screen_pos_y"), py::arg("width") = 100.0f, py::arg("height") = 100.0f, py::arg("int_tint") = 0xFFFFFFFF)
        .def("DrawTexture3D", &Py2DRenderer::DrawTexture3D, py::arg("file_path"), py::arg("world_pos_x"), py::arg("world_pos_y"), py::arg("world_pos_z"), py::arg("width") = 100.0f, py::arg("height") = 100.0f, py::arg("use_occlusion") = true, py::arg("int_tint") = 0xFFFFFFFF)
        .def("DrawQuadTextured3D", &Py2DRenderer::DrawQuadTextured3D, py::arg("file_path"),
            py::arg("p1"), py::arg("p2"), py::arg("p3"), py::arg("p4"),
            py::arg("use_occlusion") = true, py::arg("int_tint") = 0xFFFFFFFF);
}

PYBIND11_EMBEDDED_MODULE(Py2DRenderer, m) {
    bind_2drenderer(m);
}
