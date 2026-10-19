// Native Windows launcher. Keep game startup and save ownership in wl4.exe.
#include <windows.h>
#include <dwmapi.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "port/gba_memory.h"
#include "port/rom_validate.h"
#include "port/settings.h"

#define CAP 32768
#define SETTINGS L"Software\\BioRhythmicStudios\\WL4Launcher"
static HWND pathBox, scaleBox, launchButton, romNameLabel;
static wchar_t directory[CAP], romPath[CAP];
static HFONT font, heading;
static HANDLE privateFont;
static HICON largeIcon, smallIcon;
static HANDLE game;
static PortSettings settings;
static HBRUSH backgroundBrush, fieldBrush;
static const COLORREF purple = RGB(40, 17, 66), panel = RGB(61, 28, 91);
static const COLORREF gold = RGB(255, 204, 64), cream = RGB(249, 235, 255);
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

static LRESULT CALLBACK InfoProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) { DestroyWindow(window); return 0; }
        break;
    case WM_CLOSE: DestroyWindow(window); return 0;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
        SetTextColor((HDC)wParam, cream); SetBkColor((HDC)wParam, purple);
        return (LRESULT)backgroundBrush;
    case WM_DRAWITEM:
        if (((DRAWITEMSTRUCT *)lParam)->CtlType == ODT_BUTTON) {
            DrawButton((DRAWITEMSTRUCT *)lParam); return TRUE;
        }
        break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

// Use our private pixel font in launcher-owned information and error windows too.
// Native file-selection dialogs retain Windows' normal text and accessibility.
static void ShowText(HWND owner, const wchar_t *message, const wchar_t *caption, UINT flags)
{
    if (!font || !backgroundBrush) {
        MessageBoxW(owner, message, caption, flags); return;
    }
    WNDCLASSW cls = {0};
    cls.lpfnWndProc = InfoProc; cls.hInstance = GetModuleHandleW(NULL);
    cls.lpszClassName = L"WL4LauncherInfo"; cls.hCursor = LoadCursor(NULL, IDC_ARROW);
    cls.hIcon = largeIcon; cls.hbrBackground = backgroundBrush;
    RegisterClassW(&cls);
    HDC dc = GetDC(owner);
    HGDIOBJ old = SelectObject(dc, font);
    RECT textBounds = {0, 0, 816, 0};
    DrawTextW(dc, message, -1, &textBounds, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, old); ReleaseDC(owner, dc);
    RECT work; SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    int bodyHeight = textBounds.bottom + 16;
    if (bodyHeight > work.bottom - work.top - 150) bodyHeight = work.bottom - work.top - 150;
    if (bodyHeight < 100) bodyHeight = 100;
    RECT bounds = {0, 0, 864, bodyHeight + 100};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    AdjustWindowRectEx(&bounds, style, FALSE, WS_EX_DLGMODALFRAME);
    int width = bounds.right - bounds.left, height = bounds.bottom - bounds.top;
    HWND window = CreateWindowExW(WS_EX_DLGMODALFRAME, cls.lpszClassName, caption, style,
        work.left + (work.right - work.left - width) / 2,
        work.top + (work.bottom - work.top - height) / 2,
        width, height, owner, NULL, cls.hInstance, NULL);
    if (!window) { MessageBoxW(owner, message, caption, flags); return; }
    // Native multiline edits require CRLF; our messages use portable LF strings.
    size_t length = wcslen(message), used = 0;
    wchar_t *formatted = malloc((length * 2 + 1) * sizeof(wchar_t));
    if (!formatted) { DestroyWindow(window); MessageBoxW(owner, message, caption, flags); return; }
    for (size_t i = 0; i < length; ++i) {
        if (message[i] == L'\n' && (i == 0 || message[i - 1] != L'\r')) formatted[used++] = L'\r';
        formatted[used++] = message[i];
    }
    formatted[used] = 0;
    HWND body = CreateWindowExW(0, L"EDIT", formatted,
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL,
        24, 20, 816, bodyHeight, window, (HMENU)21, NULL, NULL);
    free(formatted);
    SendMessageW(body, WM_SETFONT, (WPARAM)font, TRUE);
    HWND button = CreateWindowExW(0, L"BUTTON", L"OK",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW,
        720, bodyHeight + 42, 120, 36, window, (HMENU)IDOK, NULL, NULL);
    SendMessageW(button, WM_SETFONT, (WPARAM)font, TRUE);
    SendMessageW(window, WM_SETICON, ICON_BIG, (LPARAM)largeIcon);
    SendMessageW(window, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
    RoundWindow(window);
    if (owner) EnableWindow(owner, FALSE);
    ShowWindow(window, SW_SHOW); SetFocus(button);
    MSG msg;
    int result = 1;
    while (IsWindow(window) && (result = GetMessageW(&msg, NULL, 0, 0)) > 0)
        if (!IsDialogMessageW(window, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (IsWindow(window)) DestroyWindow(window);
    if (owner) { EnableWindow(owner, TRUE); SetForegroundWindow(owner); }
    if (result == 0) PostQuitMessage((int)msg.wParam);
}

static void Error(HWND window, const wchar_t *message)
{
    ShowText(window, message, title, MB_OK | MB_ICONERROR);
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
    settings.scale = (int)SendMessageW(scaleBox, CB_GETCURSEL, 0, 0) + 1;
    char settingsError[256];
    if (PortSettingsSave(&settings, settingsError, sizeof(settingsError))) {
        wchar_t message[512];
        MultiByteToWideChar(CP_UTF8, 0, settingsError, -1, message, 512);
        Error(window, message); return;
    }
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

// Presentation only: do not shorten or rewrite the path used by validation/launch.
static void UpdateRomName(void)
{
    static wchar_t path[CAP], display[CAP + 32];
    if (!romNameLabel) return;
    GetWindowTextW(pathBox, path, CAP);
    const wchar_t *name = path;
    for (const wchar_t *cursor = path; *cursor; ++cursor)
        if (*cursor == L'\\' || *cursor == L'/') name = cursor + 1;
    if (!path[0])
        wcscpy(display, L"No ROM selected");
    else
        swprintf(display, CAP + 32, L"Selected ROM: %ls", *name ? name : L"(choose a file)");
    SetWindowTextW(romNameLabel, display);
}

static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE: {
        HKEY key;
        DWORD size = sizeof(romPath), scale = 3, scaleSize = sizeof(scale);
        font = CreateFontW(-16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_TT_PRECIS, 0, NONANTIALIASED_QUALITY, FIXED_PITCH, L"WL4 Pixel");
        heading = CreateFontW(-32, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_TT_PRECIS, 0, NONANTIALIASED_QUALITY, FIXED_PITCH, L"WL4 Pixel");
        backgroundBrush = CreateSolidBrush(purple);
        fieldBrush = CreateSolidBrush(panel);
        HWND label = Control(window, L"STATIC", L"WARIO LAND 4", 0, 32, 28, 620, 38, 0);
        SendMessageW(label, WM_SETFONT, (WPARAM)heading, TRUE);
        Control(window, L"STATIC", L"NATIVE PC PORT", 0, 34, 74, 460, 24, 0);
        Control(window, L"STATIC", L"BioRhythmicStudios", 0, 34, 107, 460, 24, 0);
        Control(window, L"STATIC", L"YOUR ROM  /  Unmodified US release (AWAE)", 0, 28, 166, 844, 24, 0);
        pathBox = Control(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 28, 197, 684, 30, 10);
        SendMessageW(pathBox, EM_SETLIMITTEXT, CAP - 1, 0);
        romNameLabel = Control(window, L"STATIC", L"No ROM selected", SS_ENDELLIPSIS | SS_NOPREFIX,
                               28, 235, 844, 20, 18);
        Control(window, L"BUTTON", L"Browse...", WS_TABSTOP, 728, 194, 144, 36, 11);
        Control(window, L"STATIC", L"Window scale", 0, 28, 270, 200, 24, 0);
        scaleBox = Control(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST, 244, 266, 88, 180, 12);
        for (int i = 1; i <= 6; ++i) {
            wchar_t item[16]; swprintf(item, 16, L"%dx", i);
            SendMessageW(scaleBox, CB_ADDSTRING, 0, (LPARAM)item);
        }
        Control(window, L"STATIC", L"F11: fullscreen in game", 0, 352, 270, 520, 26, 0);
        launchButton = Control(window, L"BUTTON", L"Launch Game", WS_TABSTOP, 652, 322, 220, 48, 13);
        Control(window, L"BUTTON", L"About", WS_TABSTOP, 28, 330, 96, 34, 14);
        Control(window, L"BUTTON", L"CREDITS", WS_TABSTOP, 136, 330, 128, 34, 15);
        Control(window, L"BUTTON", L"README", WS_TABSTOP, 276, 330, 112, 34, 16);
        Control(window, L"BUTTON", L"Settings", WS_TABSTOP, 400, 330, 144, 34, 17);
        if (RegOpenKeyExW(HKEY_CURRENT_USER, SETTINGS, 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
            if (RegGetValueW(key, NULL, L"RomPath", RRF_RT_REG_SZ, NULL, romPath, &size) != ERROR_SUCCESS)
                romPath[0] = 0;
            RegGetValueW(key, NULL, L"Scale", RRF_RT_REG_DWORD, NULL, &scale, &scaleSize);
            RegCloseKey(key);
        }
        char warning[256];
        int loaded = PortSettingsLoad(&settings, warning, sizeof(warning));
        if (loaded == 1 && scale >= 1 && scale <= 6) settings.scale = (int)scale;
        if (loaded < 0) {
            wchar_t message[512];
            MultiByteToWideChar(CP_UTF8, 0, warning, -1, message, 512);
            ShowText(window, message, L"Settings reset to defaults", MB_OK | MB_ICONWARNING);
        }
        SendMessageW(scaleBox, CB_SETCURSEL, settings.scale - 1, 0);
        SetWindowTextW(pathBox, romPath);
        UpdateRomName();
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 10:
            if (HIWORD(wParam) == EN_CHANGE) UpdateRomName();
            return 0;
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
        case 17: {
            // Carry any unsaved main-window scale choice into the settings draft.
            settings.scale = (int)SendMessageW(scaleBox, CB_GETCURSEL, 0, 0) + 1;
            EnableWindow(window, FALSE);
            int saved = PortSettingsShow(&settings);
            EnableWindow(window, TRUE); SetForegroundWindow(window);
            if (saved < 0) {
                Error(window, L"Cannot open Settings. Check that SDL3.dll is available and video can initialize.");
            } else if (saved) {
                SendMessageW(scaleBox, CB_SETCURSEL, settings.scale - 1, 0);
                Remember(window);
            }
            return 0;
        }
        case 15:
            ShowText(window, L"CREDITS\n\n"
                        L"PC port and launcher: BioRhythmicStudios\n"
                        L"Original Wario Land 4 decompilation contributors\n\n"
                        L"Neutral geometric icon: original project artwork.\n"
                        L"Pixel glyphs: Marcel Sondaar / IBM public-domain VGA font,\n"
                        L"via SDL debug font; altered TrueType packaging.\n\n"
                        L"Wario and Wario Land 4 belong to Nintendo.", title, MB_OK);
            return 0;
        case 16:
            ShowText(window, L"README / QUICK START\n\n"
                        L"1. Browse to your own legally obtained, unmodified US ROM (AWAE).\n"
                        L"2. Choose a window scale, then click Launch Game.\n"
                        L"3. Press F11 in the game for fullscreen.\n\n"
                        L"Default controls: arrows move, X = A, Z = B, Enter = Start.\n"
                        L"A / S = L / R, Backspace = Select, Escape = quit.\n"
                        L"Settings customizes physical keys, controller inputs and display.\n\n"
                        L"Your ROM path is remembered. Saves stay in the game's usual AppData location.\n"
                        L"Keep wl4.exe and its matching SDL3.dll beside this launcher.\n"
                        L"Full build and usage instructions: port/README.md in the source tree.", title, MB_OK);
            return 0;
        case 14:
            ShowText(window, L"Wario Land 4 Native PC Port\n\n"
                        L"PC port and launcher: BioRhythmicStudios\n"
                        L"Built on the work of the original Wario Land 4 decompilation contributors.\n\n"
                        L"Wario Land 4 is a Nintendo game. This unofficial port includes no ROM.\n"
                        L"Use your own legally obtained ROM. Saves are managed by the game.", title, MB_OK);
            return 0;
        }
        break;
    case WM_CTLCOLORSTATIC: {
        HDC dc = (HDC)wParam;
        SetTextColor(dc, ((HWND)lParam == romNameLabel || (HWND)lParam == GetDlgItem(window, 0)) ? gold : cream);
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
        RoundRect(dc, 16, 12, 884, 158, 22, 22);
        SelectObject(dc, oldBrush); SelectObject(dc, oldPen);
        RECT divider = {28, 308, 872, 309}; FillRect(dc, &divider, fieldBrush);
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
    largeIcon = LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1), IMAGE_ICON, 32, 32, LR_SHARED);
    smallIcon = LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1), IMAGE_ICON, 16, 16, LR_SHARED);
    HRSRC fontResource = FindResourceW(NULL, MAKEINTRESOURCEW(102), RT_RCDATA);
    HGLOBAL fontBytes = fontResource ? LoadResource(NULL, fontResource) : NULL;
    DWORD count = 0;
    if (fontBytes) privateFont = AddFontMemResourceEx(LockResource(fontBytes), SizeofResource(NULL, fontResource), NULL, &count);
    if (!privateFont || !count) { Error(NULL, L"Cannot load the embedded pixel font."); return 1; }
    WNDCLASSW cls = {0};
    cls.lpfnWndProc = WindowProc; cls.hInstance = instance;
    cls.lpszClassName = L"WL4Launcher"; cls.hCursor = LoadCursor(NULL, IDC_ARROW);
    cls.hIcon = largeIcon; cls.hbrBackground = NULL;
    if (!RegisterClassW(&cls)) return 1;
    RECT bounds = {0, 0, 900, 400};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRectEx(&bounds, style, FALSE, 0);
    HWND window = CreateWindowExW(0, cls.lpszClassName, title,
                                  style, CW_USEDEFAULT, CW_USEDEFAULT,
                                  bounds.right - bounds.left, bounds.bottom - bounds.top, NULL, NULL, instance, NULL);
    if (!window) return 1;
    SendMessageW(window, WM_SETICON, ICON_BIG, (LPARAM)largeIcon);
    SendMessageW(window, WM_SETICON, ICON_SMALL, (LPARAM)smallIcon);
    RoundWindow(window);
    ShowWindow(window, show);
    MSG msg;
    int result;
    while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
        if (!IsDialogMessageW(window, &msg)) {
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
    }
    RemoveFontMemResourceEx(privateFont);
    return result < 0 ? 1 : 0;
}
