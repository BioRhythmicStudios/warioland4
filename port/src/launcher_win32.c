// Native Windows launcher. Keep game startup and save ownership in wl4.exe.
#define COBJMACROS
#include <windows.h>
#include <wincodec.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "port/gba_memory.h"
#include "port/rom_validate.h"

#define CAP 32768
#define SETTINGS L"Software\\BioRhythmicStudios\\WL4Launcher"
static HWND pathBox, scaleBox, launchButton;
static wchar_t directory[CAP], romPath[CAP];
static HFONT font, heading;
static HANDLE game;
static HBRUSH backgroundBrush, fieldBrush;
static HBITMAP sprite;
static const COLORREF purple = RGB(40, 17, 66), panel = RGB(61, 28, 91);
static const COLORREF gold = RGB(255, 204, 64), cream = RGB(249, 235, 255);
#define SPRITE_SCALE 4
#define SPRITE_WIDTH (46 * SPRITE_SCALE)
#define SPRITE_HEIGHT (36 * SPRITE_SCALE)

// Decode the embedded original PNG with Windows WIC, then replicate each pixel.
// AlphaBlend receives an already enlarged premultiplied bitmap: no filtering.
static int LoadSprite(void)
{
    IWICImagingFactory *factory = NULL;
    IWICBitmapDecoder *decoder = NULL;
    IWICBitmapFrameDecode *frame = NULL;
    IWICFormatConverter *converter = NULL;
    IStream *stream = NULL;
    HRSRC resource = FindResourceW(NULL, MAKEINTRESOURCEW(101), RT_RCDATA);
    HGLOBAL loaded = resource ? LoadResource(NULL, resource) : NULL;
    BYTE *source = NULL, *pixels = NULL;
    UINT width = 0, height = 0;
    HRESULT result = E_FAIL;
    if (!loaded) goto done;
    stream = SHCreateMemStream(LockResource(loaded), SizeofResource(NULL, resource));
    if (!stream) goto done;
    result = CoCreateInstance(&CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                              &IID_IWICImagingFactory, (void **)&factory);
    if (FAILED(result)) goto done;
    result = IWICImagingFactory_CreateDecoderFromStream(factory, stream, NULL,
                                                        WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(result)) goto done;
    result = IWICBitmapDecoder_GetFrame(decoder, 0, &frame);
    if (FAILED(result)) goto done;
    result = IWICBitmapFrameDecode_GetSize(frame, &width, &height);
    if (FAILED(result) || width != 46 || height != 36) { result = E_FAIL; goto done; }
    result = IWICImagingFactory_CreateFormatConverter(factory, &converter);
    if (FAILED(result)) goto done;
    result = IWICFormatConverter_Initialize(converter, (IWICBitmapSource *)frame,
                                            &GUID_WICPixelFormat32bppPBGRA,
                                            WICBitmapDitherTypeNone, NULL, 0,
                                            WICBitmapPaletteTypeCustom);
    if (FAILED(result)) goto done;
    source = malloc(width * height * 4);
    if (!source) { result = E_OUTOFMEMORY; goto done; }
    result = IWICFormatConverter_CopyPixels(converter, NULL, width * 4, width * height * 4, source);
    if (FAILED(result)) goto done;
    BITMAPINFO info = {0};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = SPRITE_WIDTH;
    info.bmiHeader.biHeight = -SPRITE_HEIGHT;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    sprite = CreateDIBSection(NULL, &info, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);
    if (!sprite) { result = E_OUTOFMEMORY; goto done; }
    for (UINT y = 0; y < SPRITE_HEIGHT; ++y)
        for (UINT x = 0; x < SPRITE_WIDTH; ++x)
            memcpy(pixels + (y * SPRITE_WIDTH + x) * 4,
                   source + ((y / SPRITE_SCALE) * width + x / SPRITE_SCALE) * 4, 4);
done:
    free(source);
    if (converter) IWICFormatConverter_Release(converter);
    if (frame) IWICBitmapFrameDecode_Release(frame);
    if (decoder) IWICBitmapDecoder_Release(decoder);
    if (factory) IWICImagingFactory_Release(factory);
    if (stream) IStream_Release(stream);
    return SUCCEEDED(result) && sprite != NULL;
}

static void RoundWindow(HWND window)
{
    // The preference and caption colors are supported on Windows 11. Older
    // versions reject these attributes and use a rounded region instead.
    DWORD preference = 2;
    if (FAILED(DwmSetWindowAttribute(window, 33, &preference, sizeof(preference)))) {
        RECT bounds;
        GetWindowRect(window, &bounds);
        HRGN region = CreateRoundRectRgn(0, 0, bounds.right - bounds.left + 1,
                                         bounds.bottom - bounds.top + 1, 24, 24);
        if (region && !SetWindowRgn(window, region, TRUE)) DeleteObject(region);
    }
    DwmSetWindowAttribute(window, 35, &purple, sizeof(purple));
    DwmSetWindowAttribute(window, 36, &gold, sizeof(gold));
}

static void DrawButton(const DRAWITEMSTRUCT *item)
{
    HDC dc = item->hDC;
    RECT rect = item->rcItem;
    int saved = SaveDC(dc);
    FillRect(dc, &rect, backgroundBrush);
    COLORREF fill = item->itemState & ODS_DISABLED ? RGB(154, 131, 92)
                    : item->itemState & ODS_SELECTED ? RGB(220, 160, 35) : gold;
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 226, 125));
    SelectObject(dc, brush); SelectObject(dc, pen);
    RoundRect(dc, rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1, 14, 14);
    SetBkMode(dc, TRANSPARENT); SetTextColor(dc, purple); SelectObject(dc, font);
    wchar_t text[80]; GetWindowTextW(item->hwndItem, text, 80);
    if (item->itemState & ODS_SELECTED) OffsetRect(&rect, 1, 1);
    DrawTextW(dc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (item->itemState & ODS_FOCUS) {
        InflateRect(&rect, -5, -5); DrawFocusRect(dc, &rect);
    }
    RestoreDC(dc, saved); DeleteObject(brush); DeleteObject(pen);
}

static const wchar_t *title = L"Wario Land 4 \x2014 Native PC Port";

static void Error(HWND window, const wchar_t *message)
{
    MessageBoxW(window, message, title, MB_OK | MB_ICONERROR);
}

static void SystemError(HWND window, const wchar_t *context)
{
    wchar_t reason[512], message[1024];
    DWORD code = GetLastError();
    if (!FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL, code, 0, reason, 512, NULL))
        swprintf(reason, 512, L"Windows error %lu", code);
    swprintf(message, 1024, L"%ls\n\n%ls", context, reason);
    Error(window, message);
}

static int Validate(HWND window)
{
    static wchar_t absolute[CAP];
    wchar_t message[512];
    char error[256];
    unsigned char *bytes;
    LARGE_INTEGER size;
    DWORD read;
    HANDLE file;
    GetWindowTextW(pathBox, romPath, CAP);
    if (!romPath[0]) {
        Error(window, L"Choose your own legally obtained Wario Land 4 US ROM first.");
        return 0;
    }
    DWORD length = GetFullPathNameW(romPath, CAP, absolute, NULL);
    if (!length || length >= CAP) {
        Error(window, L"The ROM path is too long or cannot be resolved.");
        return 0;
    }
    wcscpy(romPath, absolute);
    file = CreateFileW(romPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        SystemError(window, L"Cannot open the selected ROM. Browse to an existing readable file.");
        return 0;
    }
    if (!GetFileSizeEx(file, &size) || size.QuadPart != GBA_ROM_SIZE) {
        CloseHandle(file);
        Error(window, L"Invalid ROM size. Wario Land 4 must be exactly 8 MiB (8388608 bytes).");
        return 0;
    }
    bytes = malloc(GBA_ROM_SIZE);
    if (!bytes) {
        CloseHandle(file);
        Error(window, L"Not enough memory to validate the ROM.");
        return 0;
    }
    int ok = ReadFile(file, bytes, GBA_ROM_SIZE, &read, NULL) && read == GBA_ROM_SIZE;
    CloseHandle(file);
    if (!ok) {
        free(bytes);
        Error(window, L"Cannot read the complete ROM. Check file permissions and try again.");
        return 0;
    }
    ok = PortValidateRom(bytes, read, error, sizeof(error)) == 0;
    free(bytes);
    if (!ok) {
        MultiByteToWideChar(CP_UTF8, 0, error, -1, message, 512);
        Error(window, message);
    }
    if (ok) SetWindowTextW(pathBox, romPath);
    return ok;
}

static void Remember(HWND window)
{
    HKEY key;
    DWORD scale = (DWORD)SendMessageW(scaleBox, CB_GETCURSEL, 0, 0) + 1;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, SETTINGS, 0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL);
    if (result == ERROR_SUCCESS) {
        result = RegSetValueExW(key, L"RomPath", 0, REG_SZ, (BYTE *)romPath,
                               (DWORD)((wcslen(romPath) + 1) * sizeof(wchar_t)));
        if (result == ERROR_SUCCESS)
            result = RegSetValueExW(key, L"Scale", 0, REG_DWORD, (BYTE *)&scale, sizeof(scale));
        RegCloseKey(key);
    }
    if (result != ERROR_SUCCESS) {
        SetLastError(result);
        SystemError(window, L"The selection could not be remembered for next time.");
    }
}

static void Launch(HWND window)
{
    static wchar_t exe[CAP], dll[CAP], command[CAP * 2 + 80];
    static char narrow[CAP];
    BOOL replaced = FALSE;
    STARTUPINFOW startup = {0};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process;
    if (game || !Validate(window)) return;
    // wl4 currently uses narrow fopen/argv. Reject lossy conversion explicitly.
    if (!WideCharToMultiByte(CP_ACP, GetACP() == CP_UTF8 ? 0 : WC_NO_BEST_FIT_CHARS, romPath, -1,
                             narrow, CAP, NULL, GetACP() == CP_UTF8 ? NULL : &replaced) || replaced) {
        Error(window, L"The game cannot read this path with the current Windows text encoding.\n"
                      L"Choose a ROM in a folder whose name uses characters supported by this Windows installation.");
        return;
    }
    swprintf(exe, CAP, L"%ls\\wl4.exe", directory);
    swprintf(dll, CAP, L"%ls\\SDL3.dll", directory);
    if (GetFileAttributesW(exe) == INVALID_FILE_ATTRIBUTES) {
        Error(window, L"wl4.exe is missing. Keep wl4-launcher.exe beside wl4.exe and SDL3.dll.");
        return;
    }
    HMODULE dependency = LoadLibraryExW(dll, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!dependency) {
        SystemError(window, L"SDL3.dll or one of its dependencies is missing or incompatible.\n"
                            L"Restore the matching x64 runtime files beside wl4.exe.");
        return;
    }
    FreeLibrary(dependency);
    // Windows paths cannot contain quotes; a file path cannot end with a backslash.
    swprintf(command, CAP * 2 + 80, L"\"%ls\" \"%ls\" --scale %ld", exe, romPath,
             (long)SendMessageW(scaleBox, CB_GETCURSEL, 0, 0) + 1);
    Remember(window);
    if (!CreateProcessW(exe, command, NULL, NULL, FALSE, CREATE_NO_WINDOW,
                        NULL, directory, &startup, &process)) {
        SystemError(window, L"Cannot start wl4.exe. Check that its runtime dependencies are installed.");
        return;
    }
    CloseHandle(process.hThread);
    game = process.hProcess;
    EnableWindow(launchButton, FALSE);
    SetWindowTextW(launchButton, L"Game running");
    SetTimer(window, 1, 250, NULL);
}

static HWND Control(HWND parent, const wchar_t *type, const wchar_t *text,
                    DWORD style, int x, int y, int width, int height, int id)
{
    if (wcscmp(type, L"BUTTON") == 0)
        style = (style & ~BS_TYPEMASK) | BS_OWNERDRAW;
    HWND child = CreateWindowExW(0, type, text, WS_CHILD | WS_VISIBLE | style,
                                 x, y, width, height, parent, (HMENU)(INT_PTR)id, NULL, NULL);
    SendMessageW(child, WM_SETFONT, (WPARAM)font, TRUE);
    return child;
}

static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE: {
        HKEY key;
        DWORD size = sizeof(romPath), scale = 3, scaleSize = sizeof(scale);
        font = CreateFontW(-16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
        heading = CreateFontW(-27, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
        backgroundBrush = CreateSolidBrush(purple);
        fieldBrush = CreateSolidBrush(panel);
        HWND label = Control(window, L"STATIC", L"WARIO LAND 4", 0, 32, 28, 480, 38, 0);
        SendMessageW(label, WM_SETFONT, (WPARAM)heading, TRUE);
        Control(window, L"STATIC", L"NATIVE PC PORT", 0, 34, 74, 460, 24, 0);
        Control(window, L"STATIC", L"BioRhythmicStudios", 0, 34, 107, 460, 24, 0);
        Control(window, L"STATIC", L"YOUR ROM  /  Unmodified US release (AWAE)", 0, 28, 166, 700, 24, 0);
        pathBox = Control(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 28, 197, 562, 30, 10);
        SendMessageW(pathBox, EM_SETLIMITTEXT, CAP - 1, 0);
        Control(window, L"BUTTON", L"Browse...", WS_TABSTOP, 604, 194, 128, 36, 11);
        Control(window, L"STATIC", L"Window scale", 0, 28, 253, 115, 24, 0);
        scaleBox = Control(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST, 148, 249, 98, 180, 12);
        for (int i = 1; i <= 6; ++i) {
            wchar_t item[16]; swprintf(item, 16, L"%dx", i);
            SendMessageW(scaleBox, CB_ADDSTRING, 0, (LPARAM)item);
        }
        Control(window, L"STATIC", L"F11 toggles fullscreen in the game.", 0, 274, 253, 440, 26, 0);
        launchButton = Control(window, L"BUTTON", L"Launch Game", WS_TABSTOP, 528, 322, 204, 48, 13);
        Control(window, L"BUTTON", L"About", WS_TABSTOP, 28, 330, 88, 34, 14);
        Control(window, L"BUTTON", L"CREDITS", WS_TABSTOP, 128, 330, 96, 34, 15);
        Control(window, L"BUTTON", L"README", WS_TABSTOP, 236, 330, 96, 34, 16);
        if (RegOpenKeyExW(HKEY_CURRENT_USER, SETTINGS, 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
            if (RegGetValueW(key, NULL, L"RomPath", RRF_RT_REG_SZ, NULL, romPath, &size) != ERROR_SUCCESS)
                romPath[0] = 0;
            RegGetValueW(key, NULL, L"Scale", RRF_RT_REG_DWORD, NULL, &scale, &scaleSize);
            RegCloseKey(key);
        }
        SendMessageW(scaleBox, CB_SETCURSEL, scale >= 1 && scale <= 6 ? scale - 1 : 2, 0);
        SetWindowTextW(pathBox, romPath);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 11: {
            // The Windows dialog pumps nested window callbacks: keep long paths off the stack.
            static wchar_t selected[CAP];
            GetWindowTextW(pathBox, selected, CAP);
            OPENFILENAMEW dialog = {0};
            dialog.lStructSize = sizeof(dialog);
            dialog.hwndOwner = window;
            dialog.lpstrFilter = L"Game Boy Advance ROM (*.gba)\0*.gba\0All files\0*.*\0";
            dialog.lpstrFile = selected; dialog.nMaxFile = CAP;
            dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetOpenFileNameW(&dialog)) {
                SetWindowTextW(pathBox, selected);
                if (Validate(window)) Remember(window);
            } else if (CommDlgExtendedError()) {
                Error(window, L"Windows could not open the file dialog. Try entering the ROM path directly.");
            }
            return 0;
        }
        case 13: Launch(window); return 0;
        case 15:
            MessageBoxW(window, L"CREDITS\n\n"
                        L"PC port and launcher: BioRhythmicStudios\n"
                        L"Original Wario Land 4 decompilation contributors\n\n"
                        L"Vampire Wario sprite: MisteryGX\n"
                        L"Displayed from the original transparent PNG at crisp 4x pixel scale.\n\n"
                        L"Wario and Wario Land 4 belong to Nintendo.", title, MB_OK);
            return 0;
        case 16:
            MessageBoxW(window, L"README / QUICK START\n\n"
                        L"1. Browse to your own legally obtained, unmodified US ROM (AWAE).\n"
                        L"2. Choose a window scale, then click Launch Game.\n"
                        L"3. Press F11 in the game for fullscreen.\n\n"
                        L"Controls: arrows move, X = A, Z = B, Enter = Start.\n"
                        L"A / S = L / R, Backspace = Select, Escape = quit.\n\n"
                        L"Your ROM path is remembered. Saves stay in the game's usual AppData location.\n"
                        L"Keep wl4.exe and its matching SDL3.dll beside this launcher.\n"
                        L"Full build and usage instructions: port/README.md in the source tree.", title, MB_OK);
            return 0;
        case 14:
            MessageBoxW(window, L"Wario Land 4 Native PC Port\n\n"
                        L"PC port and launcher: BioRhythmicStudios\n"
                        L"Built on the work of the original Wario Land 4 decompilation contributors.\n\n"
                        L"Wario Land 4 is a Nintendo game. This unofficial port includes no ROM.\n"
                        L"Use your own legally obtained ROM. Saves are managed by the game.", title, MB_OK);
            return 0;
        }
        break;
    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, (HWND)lParam == GetDlgItem(window, 0) ? gold : cream);
        SetBkMode(dc, TRANSPARENT);
        RECT bounds; GetWindowRect((HWND)lParam, &bounds);
        MapWindowPoints(NULL, window, (POINT *)&bounds, 2);
        return (LRESULT)(bounds.top < 150 ? fieldBrush : backgroundBrush);
    }
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORLISTBOX:
        SetTextColor((HDC)wParam, cream); SetBkColor((HDC)wParam, panel);
        return (LRESULT)fieldBrush;
    case WM_DRAWITEM:
        if (((DRAWITEMSTRUCT *)lParam)->CtlType == ODT_BUTTON) {
            DrawButton((DRAWITEMSTRUCT *)lParam); return TRUE;
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT paint;
        HDC dc = BeginPaint(window, &paint);
        RECT client; GetClientRect(window, &client); FillRect(dc, &client, backgroundBrush);
        HGDIOBJ oldBrush = SelectObject(dc, fieldBrush), oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
        RoundRect(dc, 16, 12, 744, 158, 22, 22);
        SelectObject(dc, oldBrush); SelectObject(dc, oldPen);
        RECT divider = {28, 299, 732, 300}; FillRect(dc, &divider, fieldBrush);
        if (sprite) {
            HDC source = CreateCompatibleDC(dc);
            HGDIOBJ old = SelectObject(source, sprite);
            BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            AlphaBlend(dc, 548, 14, SPRITE_WIDTH, SPRITE_HEIGHT,
                       source, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, blend);
            SelectObject(source, old); DeleteDC(source);
        }
        EndPaint(window, &paint); return 0;
    }
    case WM_TIMER:
        if (game && WaitForSingleObject(game, 0) == WAIT_OBJECT_0) {
            DWORD code; GetExitCodeProcess(game, &code);
            CloseHandle(game); game = NULL; KillTimer(window, 1);
            EnableWindow(launchButton, TRUE); SetWindowTextW(launchButton, L"Launch Game");
            if (code) {
                wchar_t reason[512];
                swprintf(reason, 512, L"The game stopped with error 0x%08lX.\n\n"
                         L"A runtime dependency may be missing or incompatible, or startup failed. "
                         L"Keep the matching x64 runtime files beside wl4.exe. "
                         L"For details, run wl4.exe directly from a terminal.", code);
                Error(window, reason);
            }
        }
        return 0;
    case WM_DESTROY:
        if (game) CloseHandle(game); // Closing the launcher never terminates the game.
        DeleteObject(font); DeleteObject(heading);
        DeleteObject(backgroundBrush); DeleteObject(fieldBrush);
        PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command, int show)
{
    (void)previous; (void)command;
    DWORD length = GetModuleFileNameW(NULL, directory, CAP);
    if (!length || length >= CAP || !wcsrchr(directory, L'\\')) return 1;
    *wcsrchr(directory, L'\\') = 0;
    HRESULT com = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(com) || !LoadSprite()) {
        Error(NULL, L"Cannot load the embedded launcher artwork with Windows Imaging Component.");
        if (SUCCEEDED(com)) CoUninitialize();
        return 1;
    }
    WNDCLASSW cls = {0};
    cls.lpfnWndProc = WindowProc; cls.hInstance = instance;
    cls.lpszClassName = L"WL4Launcher"; cls.hCursor = LoadCursor(NULL, IDC_ARROW);
    cls.hIcon = LoadIcon(NULL, IDI_APPLICATION); cls.hbrBackground = NULL;
    if (!RegisterClassW(&cls)) return 1;
    RECT bounds = {0, 0, 760, 400};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRectEx(&bounds, style, FALSE, 0);
    HWND window = CreateWindowExW(0, cls.lpszClassName, title,
                                  style, CW_USEDEFAULT, CW_USEDEFAULT,
                                  bounds.right - bounds.left, bounds.bottom - bounds.top, NULL, NULL, instance, NULL);
    if (!window) return 1;
    RoundWindow(window);
    ShowWindow(window, show);
    MSG msg;
    int result;
    while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
        if (!IsDialogMessageW(window, &msg)) {
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
    }
    DeleteObject(sprite);
    CoUninitialize();
    return result < 0 ? 1 : 0;
}
