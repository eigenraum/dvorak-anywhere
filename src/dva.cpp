/*
 * dvorakAnywhere - dvorak emulator for windows
 * Copyright (C) 2008 Dennis Felsing
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * $Id: dva.cpp 6 2008-04-01 16:12:43Z r0q $
 */

#define _WIN32_WINNT 0x0501 // required for SendInput
#include <string>
#include <windows.h>

#ifndef MAPVK_VK_TO_VSC
#define MAPVK_VK_TO_VSC 0
#endif

const std::string NAME("dva");
const std::string VERSION("0.2");

HHOOK hKeyHook;

LRESULT CALLBACK WindowProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

LRESULT CALLBACK KeyboardProc (int, WPARAM, LPARAM);

bool isDown(int key)
{
    return GetAsyncKeyState(key) < 0;
}

bool isDownAltGr()
{
    return isDown(VK_RMENU) || (isDown(VK_LMENU) && isDown(VK_CONTROL));
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW; wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "dva";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(
        0,
        "dva", // name of the window class
        (NAME + " " + VERSION).c_str(),   // title of the window
        WS_SYSMENU,             // window style
        CW_USEDEFAULT,          // x-position of the window
        CW_USEDEFAULT,          // y-position of the window
        0,                      // width of the window
        0,                      // height of the window
        NULL,                   // we have no parent window, NULL
        NULL,                   // we aren't using menus, NULL
        hInstance,              // application handle
        NULL);                  // used with multiple windows, NULL

    if (hWnd == NULL)
    {
        MessageBox(NULL, "Could not install the keyboard hook.",
            NAME.c_str(), MB_ICONERROR);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    BOOL result;

    while ((result = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (result == -1)
            return 1;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(
    HWND hWnd, 
    UINT message, 
    WPARAM wParam, 
    LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            hKeyHook = SetWindowsHookEx (
                13 /*WH_KEYBOARD_LL*/, 
                (HOOKPROC) KeyboardProc, 
                GetModuleHandle(NULL), 0);

            // returning -1 makes CreateWindowEx fail
            return hKeyHook == NULL ? -1 : 0;
        }

        case WM_DESTROY:
        {
            if (hKeyHook != NULL)
            {
                UnhookWindowsHookEx(hKeyHook);
                hKeyHook = NULL;
            }
            PostQuitMessage(0);
            return 0;
        }

        default:
        {
            return DefWindowProc (hWnd, message, wParam, lParam);
        }
    }
}

// see http://simon.spruenker.de/dvorak/ for the layout
void processGermanDvorakII(
    int oVkey, 
    int& write, 
    int& vkey, 
    int& unicodeCharacter, 
    bool& toggleAltGr, 
    bool& toggleShift)
{
    switch (oVkey)
    {
        case 0x31 /*1*/: 
        {
            if (isDownAltGr()) unicodeCharacter = 185;
            break;
        }
        case 0x37 /*7*/: 
        {
            if (isDown(VK_SHIFT) || isDownAltGr()) write = 2;
            break;
        }
        case 0x38 /*8*/: 
        {
            if (isDownAltGr()) write = 2;
            break;
        }
        case 0x39 /*9*/: 
        {
            if (isDownAltGr()) write = 2;
            break;
        }
        case 0x30 /*0*/: 
        {
            if (isDownAltGr()) write = 2;
            break;
        }
        case 0xdb /*ß*/: vkey = 0xbb /*+*/; break;
        case 0xdd /*´*/:
        {
            if (isDownAltGr()) unicodeCharacter = 96 /*`*/;
            else vkey = 0xe2 /*<*/; 
            break;
        }

        case   (int)'Q': 
        {
            if (isDownAltGr()) vkey = 0x37;
            else vkey = 0xba /*Ü*/;
            break;
        }
        case   (int)'W':
        {
            if (isDownAltGr()) vkey = 0x38;
            else vkey = 0xbc /*,*/;
            break;
        }
        case   (int)'E':
        {
            if (isDownAltGr()) vkey = 0x39;
            else vkey = 0xbe /*.*/;
            break;
        }
        case   (int)'R': 
        {
            if (isDownAltGr()) vkey = 0x30;
            else vkey =   (int)'P';
            break;
        }
        case   (int)'T': vkey =   (int)'Y'; break;
        case   (int)'Z': vkey =   (int)'F'; break;
        case   (int)'U': vkey =   (int)'G'; break;
        case   (int)'I':
        {
            if (isDownAltGr()) unicodeCharacter = 169;
            else vkey =   (int)'C';
            break;
        }
        case   (int)'O':
        {
            if (isDownAltGr()) unicodeCharacter = 8482;
            else vkey =   (int)'T';
            break;
        }
        case   (int)'P': vkey =   (int)'Z'; break;
        case 0xba /*Ü*/: 
        {
            vkey = 0xdb /*ß*/;
            toggleShift = true;
            break;
        }
        case 0xbb /*+*/:
        {
            toggleShift = true;
            if (isDown(VK_SHIFT))
            {
                vkey = 0xdb /*ß*/;
                toggleAltGr = true;
            }
            else vkey = 0x37 /*7*/; 
            break;
        }

        case   (int)'A':
        {
            if (isDownAltGr()) vkey = (int)'Q';
            break;
        }
        case   (int)'S': vkey =   (int)'O'; break;
        case   (int)'D': vkey =   (int)'E'; break;
        case   (int)'F': 
        {
            if (isDownAltGr()) vkey = 0xe2 /*<*/;
            else vkey =   (int)'I';
            break;
        }
        case   (int)'G':
        {
            if (isDownAltGr()) unicodeCharacter = 166;
            else vkey =   (int)'U';
            break;
        }
        case   (int)'H': vkey =   (int)'H'; break;
        case   (int)'J': vkey =   (int)'D'; break;
        case   (int)'K':
        {
            if (isDownAltGr()) unicodeCharacter = 174;
            vkey =   (int)'R';
            break;
        }
        case   (int)'L': 
        {
            if (isDownAltGr() && !isDown(VK_SHIFT))
            {
                vkey = 0x38;
                toggleAltGr = true;
                toggleShift = true;
            }
            else vkey =   (int)'N';
            break;
        }
        case 0xc0 /*Ö*/:
        {
            if (isDownAltGr() && !isDown(VK_SHIFT))
            {
                vkey = 0x39;
                toggleAltGr = true;
                toggleShift = true;
            }
            else vkey =   (int)'S';
            break;
        }
        case 0xde /*Ä*/: vkey =   (int)'L'; break;
        case 0xbf /*#*/: vkey = 0xbd /*-*/; break;

        case 0xe2 /*<*/: vkey = 0xde /*Ä*/; break;
        case   (int)'Y': 
        {
            if (isDownAltGr())
            {
                vkey = 0xde /*Ä*/;
                toggleAltGr = true;
            }
            else
                vkey = 0xc0 /*Ö*/;

            break;
        }
        case   (int)'X':
        {
            if (isDownAltGr()) write = 2;
            else vkey = (int)'Q';
            break;
        }
        case   (int)'C': vkey =   (int)'J'; break;
        case   (int)'V': vkey =   (int)'K'; break;
        case   (int)'B': vkey =   (int)'X'; break;
        case   (int)'N': vkey =   (int)'B'; break;
        case   (int)'M': vkey =   (int)'M'; break;
        case 0xbc /*,*/: vkey =   (int)'W'; break;
        case 0xbe /*.*/: vkey =   (int)'V'; break;
        case 0xbd /*-*/: vkey = 0xbf /*#*/; break;

        default: write = 0;
    }
}
// What the last key-down of each physical key was turned into, so that the
// matching key-up releases the right key.
const int KEY_UNCHANGED  = 0;     // passed through (or not held down)
const int KEY_SUPPRESSED = 0x100; // swallowed, nothing to release
// any other value: the virtual key that was injected instead
int keyState[256];

void addKey(INPUT* inputs, int& count, WORD vk, bool up)
{
    INPUT& input = inputs[count++];
    ZeroMemory(&input, sizeof(INPUT));
    input.type       = INPUT_KEYBOARD;
    input.ki.wVk     = vk;
    input.ki.wScan   = (WORD)MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
    if (vk == VK_RMENU || vk == VK_RCONTROL)
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
}

void addUnicode(INPUT* inputs, int& count, WCHAR character, bool up)
{
    INPUT& input = inputs[count++];
    ZeroMemory(&input, sizeof(INPUT));
    input.type       = INPUT_KEYBOARD;
    input.ki.wScan   = character;
    input.ki.dwFlags = KEYEVENTF_UNICODE | (up ? KEYEVENTF_KEYUP : 0);
}

struct Modifier
{
    WORD vk;
    bool isDown; // currently held, so it has to be released temporarily
};

void addModifier(Modifier* modifiers, int& count, WORD vk, bool isDown)
{
    modifiers[count].vk = vk;
    modifiers[count].isDown = isDown;
    count++;
}

LRESULT CALLBACK KeyboardProc (int nCode, WPARAM wParam, LPARAM lParam)
{
    // forward
    if (nCode < 0)
        return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT * pKbHS = (KBDLLHOOKSTRUCT*)lParam; // the pressed key

    // ignore keys we injected
    if ((pKbHS->flags & LLKHF_INJECTED) || pKbHS->vkCode > 0xff)
        return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

    int& state = keyState[pKbHS->vkCode];
    INPUT inputs[16];
    int   inputCount = 0;

    if ((wParam == WM_KEYUP) || (wParam == WM_SYSKEYUP))
    {
        int previous = state;
        state = KEY_UNCHANGED;

        if (previous == KEY_UNCHANGED)
            return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

        if (previous != KEY_SUPPRESSED)
        {
            addKey(inputs, inputCount, (WORD)previous, true);
            SendInput(inputCount, inputs, sizeof(INPUT));
        }
        return 1;
    }

    int  vkey  = pKbHS->vkCode; // key code that will be simulated
    int  write = 1; // write default key (0), our key (1) or no key (2)
    // set to unicode value to write values not on keyboard
    int  unicodeCharacter = -1;
    // AltGr (Control + Alt) on when off, off when on
    bool toggleAltGr = false;
    bool toggleShift = false; // same for Shift

    processGermanDvorakII(
        pKbHS->vkCode, 
        write, 
        vkey,
        unicodeCharacter, 
        toggleAltGr, 
        toggleShift);

    int newState = KEY_UNCHANGED;
    if (write == 2 || (write == 1 && unicodeCharacter != -1))
        newState = KEY_SUPPRESSED;
    else if (write == 1)
        newState = vkey;

    // the mapping changed while the key is held (e.g. a modifier was
    // pressed during auto-repeat), so release the previously injected key
    if (state != KEY_UNCHANGED && state != KEY_SUPPRESSED && state != newState)
        addKey(inputs, inputCount, (WORD)state, true);
    state = newState;

    if (write == 1)
    {
        // send characters that are not on keyboard
        if (unicodeCharacter != -1)
        {
            addUnicode(inputs, inputCount, (WCHAR)unicodeCharacter, false);
            addUnicode(inputs, inputCount, (WCHAR)unicodeCharacter, true);
        }
        else
        {
            Modifier modifiers[5];
            int      modifierCount = 0;

            if (toggleAltGr) // swapping AltGr
            {
                // Although AltGr has the same effect as Control + Alt it is
                // not the same on german keyboards, so release whichever
                // keys are actually held.
                if (isDown(VK_RMENU))
                    addModifier(modifiers, modifierCount, VK_RMENU, true);
                else if (isDown(VK_LMENU) && isDown(VK_CONTROL))
                {
                    addModifier(modifiers, modifierCount, VK_LMENU, true);
                    if (isDown(VK_LCONTROL))
                        addModifier(modifiers, modifierCount, VK_LCONTROL, true);
                    if (isDown(VK_RCONTROL))
                        addModifier(modifiers, modifierCount, VK_RCONTROL, true);
                }
                else
                    addModifier(modifiers, modifierCount, VK_RMENU, false);
            }

            if (toggleShift) // swapping Shift
            {
                if (isDown(VK_SHIFT))
                {
                    if (isDown(VK_LSHIFT))
                        addModifier(modifiers, modifierCount, VK_LSHIFT, true);
                    if (isDown(VK_RSHIFT))
                        addModifier(modifiers, modifierCount, VK_RSHIFT, true);
                }
                else
                    addModifier(modifiers, modifierCount, VK_LSHIFT, false);
            }

            for (int i = 0; i < modifierCount; i++)
                addKey(inputs, inputCount, modifiers[i].vk, modifiers[i].isDown);

            // write
            addKey(inputs, inputCount, (WORD)vkey, false);

            for (int i = modifierCount - 1; i >= 0; i--)
                addKey(inputs, inputCount, modifiers[i].vk, !modifiers[i].isDown);
        }
    }

    if (inputCount > 0)
        SendInput(inputCount, inputs, sizeof(INPUT));

    if (write == 0)
        return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

    return 1;
}
