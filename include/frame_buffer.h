#pragma once

#include <common.h>

struct FrameBufferObject
{
    uint32_t renderer_id;
    uint32_t color_buffer_id;
    uint32_t render_buffer_id;
};

inline void frame_buffer_object_create(FrameBufferObject& fbo)
{
    uint32_t renderer_id;
    GLCall(glGenFramebuffers(1, &renderer_id));
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, renderer_id));

    uint32_t color_buffer_id;
    texture_create(&color_buffer_id, 2000, 1200, 3);

    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[color_buffer_id].renderer_id, 0));

    uint32_t render_buffer_id;
    GLCall(glGenRenderbuffers(1, &render_buffer_id));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_id));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2000, 1200));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, render_buffer_id));

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG(ERROR, "unable to completely create framebuffer.");
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    fbo.renderer_id         = renderer_id;
    fbo.color_buffer_id     = color_buffer_id;
    fbo.render_buffer_id    = render_buffer_id;
}

inline void frame_buffer_object_bind(const FrameBufferObject& fbo)
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo.renderer_id));
}