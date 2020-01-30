#include <texture.h>

#include <common.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool load_texture(Texture& texture, const char* file_name)
{
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
    unsigned char *data = stbi_load(file_name, &width, &height, &n_channels, 0);
    if (data) {
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    } else {
        std::cout << "Failed to load texture" << std::endl;
        return false;
    }
    stbi_image_free(data);
    texture.width = width;
    texture.height = height;
    texture.n_channels = n_channels;
    texture.renderer_id = renderer_id;
    return true;
}

void bind_texture(const Texture& texture, uint32_t slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, texture.renderer_id));
}