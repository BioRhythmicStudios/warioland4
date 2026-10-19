// Native Windows launcher. Keep game startup and save ownership in wl4.exe.
#include <windows.h>
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
        HWND label = Control(window, L"STATIC", L"WARIO LAND 4", 0, 24, 20, 540, 38, 0);
        SendMessageW(label, WM_SETFONT, (WPARAM)heading, TRUE);
        Control(window, L"STATIC", L"Native PC Port  /  BioRhythmicStudios", 0, 26, 61, 540, 26, 0);
        Control(window, L"STATIC", L"Select your own legally obtained, unmodified US ROM (AWAE).", 0, 26, 103, 550, 25, 0);
        pathBox = Control(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 26, 137, 440, 28, 10);
        SendMessageW(pathBox, EM_SETLIMITTEXT, CAP - 1, 0);
        Control(window, L"BUTTON", L"Browse...", WS_TABSTOP, 478, 136, 98, 30, 11);
        Control(window, L"STATIC", L"Window scale", 0, 26, 194, 115, 24, 0);
        scaleBox = Control(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST, 148, 190, 98, 180, 12);
        for (int i = 1; i <= 6; ++i) {
            wchar_t item[16]; swprintf(item, 16, L"%dx", i);
            SendMessageW(scaleBox, CB_ADDSTRING, 0, (LPARAM)item);
        }
        Control(window, L"STATIC", L"F11 toggles fullscreen in the game.", 0, 265, 194, 310, 26, 0);
        launchButton = Control(window, L"BUTTON", L"Launch Game", WS_TABSTOP | BS_DEFPUSHBUTTON, 390, 246, 186, 42, 13);
        Control(window, L"BUTTON", L"About", WS_TABSTOP, 26, 251, 98, 32, 14);
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
        case 14:
            MessageBoxW(window, L"Wario Land 4 Native PC Port\n\n"
                        L"PC port and launcher: BioRhythmicStudios\n"
                        L"Built on the work of the original Wario Land 4 decompilation contributors.\n\n"
                        L"Wario Land 4 is a Nintendo game. This unofficial port includes no ROM.\n"
                        L"Use your own legally obtained ROM. Saves are managed by the game.", title, MB_OK);
            return 0;
        }
        break;
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
        DeleteObject(font); DeleteObject(heading); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command, int show)
{
    (void)previous; (void)command;
    DWORD length = GetModuleFileNameW(NULL, directory, CAP);
    if (!length || length >= CAP || !wcsrchr(directory, L'\\')) return 1;
    *wcsrchr(directory, L'\\') = 0;
    WNDCLASSW cls = {0};
    cls.lpfnWndProc = WindowProc; cls.hInstance = instance;
    cls.lpszClassName = L"WL4Launcher"; cls.hCursor = LoadCursor(NULL, IDC_ARROW);
    cls.hIcon = LoadIcon(NULL, IDI_APPLICATION); cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClassW(&cls)) return 1;
    HWND window = CreateWindowExW(0, cls.lpszClassName, title,
                                  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 620, 350, NULL, NULL, instance, NULL);
    if (!window) return 1;
    ShowWindow(window, show);
    MSG msg;
    int result;
    while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
        if (!IsDialogMessageW(window, &msg)) {
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
    }
    return result < 0 ? 1 : 0;
}
