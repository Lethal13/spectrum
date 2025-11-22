#ifndef PLATFORM_WIN32_SPECTRUM_H
#define PLATFORM_WIN32_SPECTRUM_H

#include "./../../../include/spectrum/spectrum.h"
#include <stdint.h>
#include <Windows.h>

struct win32_framebuffer
{
    BITMAPINFO bitmap_info;
    void *data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t pitch;
    uint32_t bytes_per_pixel;
};

struct win32_engine_context
{
    HMODULE dll_handle;
    update_render_fn *update_render_cb;
};

struct win32_window_dimensions
{
	int32_t width;
	int32_t height;
};


static void win32_show_last_error_message(const char *title);

static struct win32_engine_context win32_engine_load(const char *source_dll);
static void win32_engine_unload(struct win32_engine_context *ctx);

static void win32_display_buffer(HDC device_context, int32_t window_width, int32_t window_height, struct win32_framebuffer* buffer);
static void win32_resize_dib_section(struct win32_framebuffer *framebuffer, uint32_t width, uint32_t height);

static void win32_process_messages();
LRESULT CALLBACK win32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
