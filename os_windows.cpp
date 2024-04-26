#include "api.h"

static vector<DWORD> *pid_list = new vector<DWORD>();
static inline void fill_pid_list(const wchar_t* exeName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &processEntry)) {
            do {
                if (wcsstr(processEntry.szExeFile, exeName) != NULL) {
                    (*pid_list).push_back(processEntry.th32ProcessID);
                }
            } while (Process32NextW(hSnapshot, &processEntry));
        };
        CloseHandle(hSnapshot);
    }
}

typedef struct {
    size_t list_sz;
    WindowHandle win;
}clbckParam;

static inline BOOL CALLBACK winListPidClbck(WindowHandle handle, LPARAM param) {
    DWORD pid = 0;
    clbckParam *cp = (clbckParam *)param;
    GetWindowThreadProcessId(handle, &pid);
    if (IsWindowVisible(handle)) {
        for(size_t i = 0; i < cp->list_sz; i++) {
            if(pid == (*pid_list)[i]) {
                cp->win = handle;
                return FALSE;
            }
        }
    }
    return TRUE;
}

static inline WindowHandle get_win_from_pid(const wchar_t* exeName) {
    fill_pid_list(exeName);
    clbckParam cp = { .list_sz = (*pid_list).size(), .win = NULL };
    EnumWindows(winListPidClbck, (LPARAM)&cp);
    delete pid_list;
    return cp.win;
}

void *CaptureWindow(const wchar_t *exeName) {
    WindowHandle win = get_win_from_pid(exeName);
    ASSERT_EXIT(win, "Failed to get window handle");
    if(IsIconic(win)) {
        ShowWindow(win, SW_MAXIMIZE);
    }
    SetForegroundWindow(win);
    RECT rect;
    GetWindowRect(win, &rect);
    LONG w = rect.right - rect.left;
    LONG h = rect.bottom - rect.top;
#ifdef TEST
    printf("ch: %ld, cw: %ld\n", rect.top, rect.left);
    printf("h:%ld, w:%ld\n", h, w);
#endif
    Sleep(100);
    HDC winDC = GetDC(NULL);
    ASSERT_EXIT(winDC, "Failed to get winDC");
    HDC memDC = CreateCompatibleDC(winDC);
    ASSERT_EXIT(memDC, "Failed to create memDC", ReleaseDC(NULL, winDC));
    HBITMAP hbmWin = CreateCompatibleBitmap(winDC, w, h);
    ASSERT_EXIT(hbmWin, "Failed to create hbmWin", DeleteDC(memDC), ReleaseDC(NULL, winDC));
    SelectObject(memDC, hbmWin);
    auto cleanup = [&]() {
        DeleteObject(hbmWin);
        DeleteDC(memDC);
        ReleaseDC(NULL, winDC);
    };
    ASSERT_EXIT(BitBlt(memDC, 0, 0, w, h, winDC, rect.left, rect.top, SRCCOPY), "BitBlt failed", cleanup());
    BITMAP bmpWin;
    GetObject(hbmWin, sizeof(BITMAP), &bmpWin);
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER   bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpWin.bmWidth;
    bi.biHeight = bmpWin.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    DWORD dwBmpSize = ((bmpWin.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpWin.bmHeight;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char*)GlobalLock(hDIB);
    GetDIBits(winDC, hbmWin, 0,
        (UINT)bmpWin.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    void * ret = NULL;
#ifdef SAVE_BMP
    HANDLE hFile = CreateFileW(L"./captureqwsx.bmp",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    bmfHeader.bfType = 0x4D42;
    DWORD dwBytesWritten = 0;
    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);
    CloseHandle(hFile);
#elif defined(SAVE_OPENCV)
    cv::Mat* image = new cv::Mat(bi.biHeight, bi.biWidth, CV_8UC4, lpbitmap);
    cv::cvtColor(*image, *image, cv::COLOR_BGRA2RGB);
    ret = image;
#ifdef TEST
    std::cout << "img = \n"<< " "  << (*image) << "\n\n";
#endif
#endif
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    cleanup();
    return ret;
}



#ifdef TEST
int main() {
    const wchar_t* exeName = L"brave";
    void *ret = CaptureWindow(exeName);
#ifdef SAVE_OPENCV    
    auto ptr = (cv::Mat*)ret
    delete ptr;
#endif
    return EXIT_SUCCESS;
}
#endif


