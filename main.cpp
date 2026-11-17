#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <cstdlib>
#include <vector>
#include <iostream>

constexpr TGAColor white = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor green = { 0, 255,   0, 255 };
constexpr TGAColor red = { 0,   0, 255, 255 };
constexpr TGAColor blue = { 255, 128,  64, 255 };
constexpr TGAColor yellow = { 0, 200, 255, 255 };

vec3 barycentric(vec2 A, vec2 B, vec2 C, vec2 P) {
    float denom = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);

    if (std::abs(denom) < 1e-2) return { -1.f , 1.f, 1.f };

    float alpha = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denom;
    float beta = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denom;
    float gamma = 1.0f - alpha - beta;

    return { alpha, beta, gamma };
}

/*
void line_naive(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor &color) {
    float m = (y1 - y0) / static_cast<float>(x1 - x0);
    float temp;
    if (abs(m) < 1) 
    {
        image.set(x0, y0, color);
        for (int i = x0+1; i <= x1; ++i)
        {
            temp = (i * m) + (y0-m*x0);
            image.set(i, std::round(temp), color);
        }
    }
    else
    {
        m /= m * m;
        image.set(x0, y0, color);
        for (int i = y0+1; i <= y1; ++i)
        {
            temp = (i * m) + (x0-m*y0);
            image.set(std::round(temp), i, color);
        }
    }
}


void line_dda(int x0, int y0, int x1, int y1, TGAImage &image, const TGAColor &color) {
    float m = (y1 - y0) / static_cast<float>(x1 - x0);
    if (abs(m) < 1)
    {
        image.set(x0, y0, color);
        float y = y0;
        for (int i = x0 + 1; i <= x1; ++i)
        {
            y += m;
            image.set(i, std::round(y), color);
        }
    }
    else
    {
        m /= m * m;
        image.set(x0, y0, color);
        float x = x0;
        for (int i = y0 + 1; i <= y1; ++i)
        {
            x += m;
            image.set(std::round(x), i, color);
        }
    }
}

void line_error(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color) {
    float m = (y1 - y0) / static_cast<float>(x1 - x0);
    float error=0.0f;
    float derror = std::abs(m);

    if (std::abs(m) < 1)
    {
        int y = y0;
        for (int i = x0; i <= x1; ++i)
        {
            image.set(i, y, color);
            error += derror;
            if (error >= 0.5f)
            {
                ++y;
                error -= 1.0f;
            }
        }
    }
    else
    {
        derror /= derror * derror;
        int x = x0;
        for (int i = y0; i <= y1; ++i)
        {
            image.set(x, i, color);
            error += derror;
            if (error >= 0.5f)
            {
                ++x;
                error -= 1.0f;
            }
        }
    }
}


void line_bresenham(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color) {
    int dy = std::abs(y1 - y0);
    int dx = std::abs(x1 - x0);
    int error2 = 0;

    if (dy < dx)
    {
        int y = y0;
        int derror2 = 2 * dy;
        for (int i = x0; i <= x1; ++i)
        {
            image.set(i, y, color);
            error2 += derror2;
            if (error2 >= dx)
            {
                ++y;
                error2 -= 2*dx;
            }
        }
    }
    else
    {
        int x = x0;
        int derror2 = 2 * dx;
        for (int i = y0; i <= y1; ++i)
        {
            image.set(x, i, color);
            error2 += derror2;
            if (error2 >= dy)
            {
                ++x;
                error2 -= 2*dy;
            }
        }
    }
}
*/

void line_bresenham(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor& color) {
    bool steep = false;    

    if (std::abs(y1 - y0) > std::abs(x1 - x0)) 
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x1 < x0) 
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dy = std::abs(y1 - y0);
    int dx = std::abs(x1 - x0);
    int error2 = 0;
    int y = y0;
    int derror2 = 2 * dy;

    int y_step = (y1 > y0) ? 1 : -1;
   
    for (int i = x0; i <= x1; ++i)
    {
        if (!steep)
        {
            image.set(i, y, color);
        }
        else
        {
            image.set(y, i, color);
        }

        error2 += derror2;
        if (error2 >= dx)
        {
            y += y_step;
            error2 -= 2 * dx;
        }
    }    
}

void triangle(
    vec2 t0, vec2 t1, vec2 t2, 
    vec3 w0, vec3 w1, vec3 w2, 
    float ity0, float ity1, float ity2,
    TGAImage& image, std::vector<float>& zbuffer
) {
    int minX = std::min({ (int)t0.x, (int)t1.x, (int)t2.x });
    int maxX = std::max({ (int)t0.x, (int)t1.x, (int)t2.x });
    int minY = std::min({ (int)t0.y, (int)t1.y, (int)t2.y });
    int maxY = std::max({ (int)t0.y, (int)t1.y, (int)t2.y });

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX > image.width()) maxX = image.width()-1;
    if (maxY > image.height()) maxY = image.height()-1;
 
    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            vec2 p = { x, y };
            vec3 bc = barycentric(t0, t1, t2, p);

            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            
            float pz = bc.x * w0.z + bc.y * w1.z + bc.z * w2.z;
            float ity = bc.x * ity0 + bc.y * ity1 + bc.z * ity2;

            int color_val = std::clamp((int)(ity * 255), 0, 255);
            TGAColor face_color = {(uint8_t)color_val, (uint8_t)color_val, (uint8_t)color_val, 255};

            int idx = x + y * image.width();

            if (zbuffer[idx] < pz)
            {
                image.set(x, y, face_color);
                zbuffer[idx] = pz;
            }
        }
    }
}

mat<4, 4> lookat(vec3 eye, vec3 center, vec3 up) {
    vec3 z = normalized(eye-center);
    vec3 x = normalized(cross(up, z));
    vec3 y = normalized(cross(z, x));

    // 回転の逆行列（Minv）と、平行移動の逆行列（Tr）の器を用意
    mat<4, 4> Minv = {}; // Matrix Inverse
    mat<4, 4> Tr = {}; // Translation

    for (int i = 0; i < 4; ++i)
    {
        Minv[i][i] = 1.0f;
        Tr[i][i] = 1.0f;
    }

    for (int i = 0; i < 3; i++) 
    {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];

        Tr[i][3] = -eye[i];
    }

    return Minv * Tr;
}

int main(int argc, char** argv) {
    constexpr int width = 1000;
    constexpr int height = 1000;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::vector<float> zbuffer(width * height, -std::numeric_limits<float>::max());

    Model* model = new Model("C:/tinyrenderer/obj/african_head/african_head.obj");
    
    
    vec3 light_dir = normalized(vec3 { 0.0f, 0.0f, -1.0f });

    vec3 eye = { 1.0f, 1.0f, 3.0f }; // camera's position
    vec3 center = {0.0f, 0.0f, 0.0f}; // target's position
    vec3 up = {0.0f, 1.0f, 0.0f};

    mat<4, 4> ModelView = lookat(eye, center, up);
    mat<4, 4> Projection = {};

    for (int i = 0; i < 4; ++i)
    {
        Projection[i][i] = 1.0f;
    }

    float c = norm(eye - center);
    Projection[3][2] = -1.0f / c; // Perspective Projection : 透視図法

    
    for (int i = 0; i < model->nfaces(); i++) 
    {
        
        vec2 screen_coords[3]; // coordinate : 座標
        vec3 world_coords[3]; 
        float intensity[3];

        for (int j = 0; j < 3; j++) 
        {
            vec4 v = model->vert(i, j);
            vec4 n_local = model->normal(i, j);
            n_local[3] = 0.0f;

            vec3 n = (ModelView * n_local).xyz();

            intensity[j] = std::clamp(n * -1*light_dir, 0.0, 1.0);
            
            vec4 clip_coords = Projection * ModelView * v;

            vec3 ndc = {
                clip_coords.x / clip_coords.w,
                clip_coords.y / clip_coords.w,
                clip_coords.z / clip_coords.w
            };

            world_coords[j] = ndc;

            screen_coords[j].x = (ndc.x + 1.f) * width / 2.f;
            screen_coords[j].y = (ndc.y + 1.f) * height / 2.f;
        }

        vec3 edge1 = world_coords[2] - world_coords[0];
        vec3 edge2 = world_coords[1] - world_coords[0];
        
        if (cross(edge1, edge2).z < 0)
        {
            triangle(
                screen_coords[0], screen_coords[1], screen_coords[2], 
                world_coords[0], world_coords[1], world_coords[2],
                intensity[0], intensity[1], intensity[2],
                framebuffer, zbuffer
            );
        }
    }
    

    

    framebuffer.write_tga_file("C:/tinyrenderer/framebuffer3.tga");
    delete model;
    return 0;
}