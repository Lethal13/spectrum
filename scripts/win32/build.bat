@echo off

@REM /nologo: Suppresses display of sign-on banner.
@REM /MTd: Compiles to create a debug multithreaded executable file, by using LIBCMTD.lib.
@REM /Z7: Generates C 7.0-compatible debugging information.
@REM /Od: Disables optimization.
@REM /Oi: Generates intrinsic functions.
@REM /TC: Specifies all source files are C.
@REM /WX: Treat warnings as errors.

:: Define source file paths
:: Path to platform-specific code
set BasePlatformPath=..\\..\\src\\platform\\win32\\win32_spectrum.c
:: Path to core library code
set BaseLibPath=..\\..\\src\\engine\\spectrum.c

:: Define compiler flags
set CompilerFlags=/nologo /TC /MTd /Z7 /Od /Oi /WX /W4 /wd4201 /wd4100 -DSPECTRUM_DEBUG
:: /W4: Set warning level to 4 (highest standard level)
:: -DSPECTRUM_DEBUG: Define SPECTRUM_DEBUG preprocessor macro

:: Define linker flags
set LinkerFlags=/incremental:no kernel32.lib /out:win32_spectrum.exe /subsystem:WINDOWS /MACHINE:X64
:: kernel32.lib: Core Windows functionality
:: /out: Specifies output executable name
:: /MACHINE:X64: Targets 64-bit architecture

:: Create build directories if they don't exist
IF NOT EXIST ..\..\build mkdir ..\..\build
IF NOT EXIST ..\..\build\msvc_build_x64 mkdir ..\..\build\msvc_build_x64

:: Change to build directory
pushd ..\..\build\msvc_build_x64

:: Compile core library as a DLL
call cl %CompilerFlags% %BaseLibPath% /Fmspectrum.map /LD /Fespectrum.dll /link -incremental:no -EXPORT:update_and_render
:: /Fm: Generate map file
:: /LD: Create a DLL
:: -EXPORT: Export the raytracer function from the DLL

:: Compile platform-specific code as an executable
call cl %CompilerFlags% %BasePlatformPath% /Fmwin32_spectrum.map /link %LinkerFlags%
:: Links against the DLL created above

:: Return to original directory
popd
