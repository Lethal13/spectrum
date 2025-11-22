#include <stdio.h>

#include "./win32_spectrum.h"

static struct win32_engine_context win32_engine_load(const char *source_dll)
{
    struct win32_engine_context result = {0};

    result.dll_handle = LoadLibraryA(source_dll);
    if(result.dll_handle)
    {
        result.update_render_cb = (update_render_fn*)GetProcAddress(result.dll_handle, "update_render");
    }

    return result;
}

static void win32_engine_unload(struct win32_engine_context *ctx)
{
    if(ctx->dll_handle)
    {
        FreeLibrary(ctx->dll_handle);
        ctx->dll_handle = NULL;
    }

    ctx->update_render_cb = 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
{
    struct win32_engine_context ctx = win32_engine_load("spectrum.dll");

    if (!ctx.dll_handle)
    {
        MessageBoxA(NULL, "Failed to load spectrum.dll", "Error", MB_OK);
        return -1;
    }


    printf("Spectrum loaded.\n");

    win32_engine_unload(&ctx);
    return 0;
}
