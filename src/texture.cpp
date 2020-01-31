#include <texture.h>

#include <common.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

std::vector<Texture> textures;

bool load_texture(uint32_t* texture_id, const char* file_name)
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

    textures.push_back({width, height, n_channels, renderer_id});
    *texture_id = textures.size()-1;
    return true;
}

void bind_texture(uint32_t texture_id, uint32_t slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, textures[texture_id].renderer_id));
}