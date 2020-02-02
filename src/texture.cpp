#include <texture.h>

#include <common.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static uint32_t default_texture_id = ~0;

std::vector<Texture> textures;

void textures_init()
{
    if (default_texture_id != (uint32_t)~0)
    {
        LOG(WARNING, "tried to init textures more than once.");
        return;
    }
    const uint8_t default_color[3] = { 255, 0, 255 };
    texture_create(&default_texture_id, 1, 1, 3, &default_color);
}

void textures_terminate()
{
    for (uint32_t i = 0; i < textures.size(); ++i)
        GLCall(glDeleteTextures(1, &textures[i].renderer_id));
}

void texture_create(uint32_t* texture_id, int width, int height, int n_channels, const void* data)
{
    uint32_t renderer_id;

    GLCall(glGenTextures(1, &renderer_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    textures.push_back({{0}, width, height, n_channels, renderer_id});
    *texture_id = textures.size()-1;
}

bool texture_load(uint32_t* texture_id, const char* file_path)
{
    size_t file_path_len = strlen(file_path);
    ASSERT(file_path_len < MAX_PATH_LENGTH, "file path too long: \"%s\".", file_path);

    for (uint32_t i = 0; i < textures.size(); ++i)
    {
        if (!strncmp(file_path, textures[i].file_path, MAX_PATH_LENGTH))
        {
            *texture_id = i;
            return true;
        }
    }

    uint32_t renderer_id;
    GLCall(glGenTextures(1, &renderer_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id));
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // load and generate the texture
    int width, height, n_channels;
    unsigned char *data = stbi_load(file_path, &width, &height, &n_channels, 0);
    if (data)
    {
        if (n_channels != 3)
            LOG(ERROR, "number of channels not supported (%d != 3): \"%s\".", n_channels, file_path);

        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
        LOG(WARNING, "failed to load texture: \"%s\".", file_path);
        *texture_id = default_texture_id;
        return false;
    }
    stbi_image_free(data);

    textures.push_back({{0}, width, height, n_channels, renderer_id});
    *texture_id = textures.size()-1;
    
    LOG(SUCCESS, "finished loading texture: \"%s\".", file_path);
    return true;
}

void texture_bind(uint32_t texture_id, uint32_t slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, textures[texture_id].renderer_id));
}