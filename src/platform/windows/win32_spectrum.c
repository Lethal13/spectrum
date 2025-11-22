#include <stdio.h>

#include "./win32_spectrum.h"

static struct win32_framebuffer g_framebuffer;
static uint32_t g_running;

static void win32_show_last_error_message(const char *title)
{
	DWORD error = GetLastError();
	char error_msg[256];

	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
				   NULL,
				   error,
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				   error_msg,
				   sizeof(error_msg),
				   NULL);

	MessageBoxA(NULL, error_msg, title, MB_ICONEXCLAMATION | MB_OK);
}

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


static struct win32_window_dimensions win32_get_window_dimensions(HWND hwnd)
{
	struct win32_window_dimensions result = {0};
	RECT client_rect;
	GetClientRect(hwnd, &client_rect);
	result.width = client_rect.right - client_rect.left;
	result.height = client_rect.bottom - client_rect.top;

	return result;
}

static void
win32_display_buffer(HDC device_context, int32_t window_width, int32_t window_height, struct win32_framebuffer* buffer)
{
	// TODO(Alexandris): Check also SETDIBitsToDevice function, if not scaling is needed is faster(?)
	StretchDIBits(device_context,
				  /*
				  X, Y, Width, Height,
				  X, Y, Width, Height,
				  */
				  0,
				  0,
				  window_width,
				  window_height,
				  0,
				  0,
				  buffer->width,
				  buffer->height,
				  buffer->data,
				  &buffer->bitmap_info,
				  DIB_RGB_COLORS,
				  SRCCOPY);
}

static void win32_resize_dib_section(struct win32_framebuffer *framebuffer, uint32_t width, uint32_t height)
{
	if (framebuffer->data)
	{
		VirtualFree(framebuffer->data, 0, MEM_RELEASE);
		framebuffer->data = 0;
	}

	int32_t bytes_per_pixel = 4;
	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->bytes_per_pixel = bytes_per_pixel;
	framebuffer->stride = framebuffer->width * bytes_per_pixel;
	framebuffer->pitch = framebuffer->width * bytes_per_pixel;

	// NOTE: When the biHeight field is negative, this is the clue to
	// WINDOWS to treat this bitmap as top-down, not bottom-up.
	framebuffer->bitmap_info.bmiHeader.biSize = sizeof(framebuffer->bitmap_info.bmiHeader);
	framebuffer->bitmap_info.bmiHeader.biWidth = framebuffer->width;
	framebuffer->bitmap_info.bmiHeader.biHeight = framebuffer->height;
	framebuffer->bitmap_info.bmiHeader.biPlanes = 1;
	framebuffer->bitmap_info.bmiHeader.biBitCount = 32;
	framebuffer->bitmap_info.bmiHeader.biCompression = BI_RGB;

	uint32_t bitmap_memory_size = (width * height * bytes_per_pixel);
	framebuffer->data = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);


	// TODO(Alexandris): Consider clearing the buffer to a default color.
	// Alternatively, implement a function to resize and interpolate the existing image
	// to temporarily fill the resized areas until the engine provides a new frame,
	// preventing black artifacts during resizing.
}

static void win32_process_messages()
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		switch (message.message)
		{
            case WM_QUIT: g_running = 0; break;
            default: TranslateMessage(&message); DispatchMessageA(&message);
		}
	}
}


LRESULT CALLBACK win32_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            g_running = 0;
        } break;
		case WM_SIZE:
		{
			int32_t width = LOWORD(lParam);
			int32_t height = HIWORD(lParam);
			win32_resize_dib_section(&g_framebuffer, width, height);
		}
		break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			int32_t vkcode = (int32_t) wParam;
			// bool was_down = ((lParam & (1 << 30)) != 0);
			// bool is_down = ((lParam & (1 << 31)) != 0);

			uint32_t alt_key_was_down = (lParam & (1 << 29));
			if ((vkcode == VK_F4) && alt_key_was_down)
			{
                PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
		}
		break;
		default: result = DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return result;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow)
{
    struct win32_engine_context ctx = win32_engine_load("spectrum.dll");
	win32_resize_dib_section(&g_framebuffer, 1024, 768);

    if (!ctx.dll_handle)
    {
        win32_show_last_error_message("Failed to load spectrum.dll");
        return -1;
    }

	WNDCLASSA window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = &win32_window_proc;
	window_class.hInstance = hInstance;
	// window_class.hIcon;
	window_class.lpszClassName = "SpectrumWindowClass";

	if (!RegisterClassA(&window_class))
	{
		win32_show_last_error_message("Window Registration Failed");
		return -1;
	}

	HWND hwnd = CreateWindowExA(0,
								window_class.lpszClassName,
								"Spectrum Engine",
								WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								CW_USEDEFAULT,
								0,
								0,
								hInstance,
								0);

	if (hwnd == NULL)
	{
		win32_show_last_error_message("Window Creation Failed");
		return -1;
	}


	ShowWindow(hwnd, nCmdShow);
	HDC device_context = GetDC(hwnd); // permanent, because of CS_OWNDC


	SetWindowPos(hwnd, HWND_TOP, 100, 100, 1600, 900, SWP_SHOWWINDOW);
	g_running = 1;

    while(g_running)
    {
        win32_process_messages();

		struct win32_window_dimensions dimensions = win32_get_window_dimensions(hwnd);
		win32_display_buffer(device_context, dimensions.width, dimensions.height, &g_framebuffer);
    }

    win32_engine_unload(&ctx);
    return 0;
}
