#pragma once

#include <common.h>

struct Texture
{
    char file_path[MAX_PATH_LENGTH];
    int width;
    int height;
    int n_channels;
    uint32_t renderer_id;
};

extern std::vector<Texture> textures;

void textures_init();

void texture_create(uint32_t* texture_id, int width, int height, int n_channels, const void* data = nullptr);
bool texture_load(uint32_t* texture_id, const char* file_path);
void texture_bind(uint32_t texture_id, uint32_t slot = 0);