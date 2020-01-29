#pragma once

#include <cstdint>

struct Texture
{
	int width;
	int height;
	int n_channels;
	uint32_t renderer_id;
};

bool load_texture(const char* file_name, Texture& texture);
void bind_texture(const Texture& texture, uint32_t slot = 0);