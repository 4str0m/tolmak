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
        std::cout << "Warning: tried to init textures more than once." << std::endl;
        return;
    }
    const uint8_t default_color[3] = { 255, 0, 255 };
    texture_create(&default_texture_id, 1, 1, 3, &default_color);
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
    if (file_path_len > MAX_PATH_LENGTH) {
        std::cout << "Error: file path too long: " << file_path << std::endl;
        exit(EXIT_FAILURE); 
    }

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
    if (data) {
        if (n_channels != 3)
        {
            std::cout << "Error: number of channels not supported: " << file_path << std::endl;
            exit(EXIT_FAILURE); 
        }
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    } else {
        std::cout << "Warning: failed to load texture: " << file_path << std::endl;
        *texture_id = default_texture_id;
        return false;
    }
    stbi_image_free(data);

    textures.push_back({{0}, width, height, n_channels, renderer_id});
    *texture_id = textures.size()-1;
    return true;
}

void texture_bind(uint32_t texture_id, uint32_t slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, textures[texture_id].renderer_id));
}