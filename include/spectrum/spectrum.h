#ifndef ENGINE_SPECTRUM_H
#define ENGINE_SPECTRUM_H

#include <stdint.h>

struct framebuffer
{
    void *data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t pitch;
    uint32_t bytes_per_pixel;
};

#define UPDATE_RENDER(name) void name(struct framebuffer *framebuffer)

typedef UPDATE_RENDER(update_render_fn);

#endif
