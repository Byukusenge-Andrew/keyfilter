// keyfilter.cpp - symmetric ghost-key filter for shorted pairs G<->5 and H<->6.
// Whichever key of a pair arrives second (within WINDOW_MS) is dropped.
// Build (MinGW): g++ -O2 -mwindows keyfilter.cpp -o keyfilter.exe
// Exit: Stop-Process -Name keyfilter
#include <windows.h>

static const DWORD WINDOW_MS = 25;
static DWORD lastReal[256] = {0};   // time of last accepted key-down per vk
static bool  dropped[256]  = {false};

static int partner(DWORD vk) {
    switch (vk) {
        case 'G': return '5';
        case '5': return 'G';
        case 'H': return '6';
        case '6': return 'H';
    }
    return 0;
}

LRESULT CALLBACK Hook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        auto* k = reinterpret_cast<KBDLLHOOKSTRUCT*>(lp);
        if (!(k->flags & LLKHF_INJECTED) && k->vkCode < 256) {
            bool down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);
            bool up   = (wp == WM_KEYUP   || wp == WM_SYSKEYUP);
            int p = partner(k->vkCode);
            if (p) {
                if (down) {
                    // auto-repeat of a key we already accepted: let it through
                    bool repeat = lastReal[k->vkCode] && !dropped[k->vkCode] &&
                                  (GetAsyncKeyState((int)k->vkCode) & 0x8000) &&
                                  (k->time - lastReal[k->vkCode]) > WINDOW_MS;
                    if (!repeat) {
                        if (lastReal[p] && (k->time - lastReal[p]) <= WINDOW_MS) {
                            dropped[k->vkCode] = true;
                            return 1;               // ghost: swallow
                        }
                        dropped[k->vkCode] = false;
                        lastReal[k->vkCode] = k->time;
                    }
                } else if (up && dropped[k->vkCode]) {
                    dropped[k->vkCode] = false;
                    return 1;                       // swallow ghost key-up
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    HHOOK h = SetWindowsHookExW(WH_KEYBOARD_LL, Hook, GetModuleHandleW(nullptr), 0);
    if (!h) return 1;
    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0)) { TranslateMessage(&m); DispatchMessageW(&m); }
    UnhookWindowsHookEx(h);
    return 0;
}
