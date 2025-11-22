#include "./../../include/spectrum/spectrum.h"


UPDATE_RENDER(update_render)
{
    uint32_t *pixels = (uint32_t*)framebuffer->data;
    for(size_t i = 0; i < framebuffer->width * framebuffer->height; ++i)
    {
        *pixels++ = 0xFFFF0000;
    }
}
