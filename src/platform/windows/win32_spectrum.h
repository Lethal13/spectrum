#ifndef PLATFORM_WIN32_SPECTRUM_H
#define PLATFORM_WIN32_SPECTRUM_H

#include "./../../../include/spectrum/spectrum.h"
#include <stdint.h>
#include <Windows.h>

struct win32_engine_context
{
    HMODULE dll_handle;
    update_render_fn *update_render_cb;
};

static void win32_show_last_error_message(const char *title);

static struct win32_engine_context win32_engine_load(const char *source_dll);
static void win32_engine_unload(struct win32_engine_context *ctx);

#endif
