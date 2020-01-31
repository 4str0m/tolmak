#pragma once

#include <cstdint>
#include <vector>

struct Texture
{
    int width;
    int height;
    int n_channels;
    uint32_t renderer_id;
};

extern std::vector<Texture> textures;

bool load_texture(uint32_t* texture_id, const char* file_name);
void bind_texture(uint32_t texture_id, uint32_t slot = 0);