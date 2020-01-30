#pragma once

#include <cstdint>

struct Texture
{
    int width;
    int height;
    int n_channels;
    uint32_t renderer_id;
};

bool load_texture(Texture& texture, const char* file_name);
void bind_texture(const Texture& texture, uint32_t slot = 0);