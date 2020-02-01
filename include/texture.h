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

void texture_create(uint32_t* texture_id, int width, int height, int n_channels);
bool texture_load(uint32_t* texture_id, const char* file_name);
void texture_bind(uint32_t texture_id, uint32_t slot = 0);