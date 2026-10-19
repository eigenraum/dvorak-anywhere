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

    ShowWindow(hWnd, nCmdShow);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
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
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
            break;
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
            if (isDownAltGr()) write = 2;
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
LRESULT CALLBACK KeyboardProc (int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT * pKbHS = (KBDLLHOOKSTRUCT*)lParam; // the pressed key
    int  vkey  = pKbHS->vkCode; // key code that will be simulated

    int  write = 0; // write default key (0), our key (1) or no key (2)
    // set to unicode value to write values not on keyboard
    int  unicodeCharacter = -1;
    // AltGr (Control + Alt) on when off, off when on
    bool toggleAltGr = false;
    bool toggleShift = false; // same for Shift

    // possible flags for Shift, Control, Alt, AltGr  
    const int ownFlags[2] = {0, KEYEVENTF_KEYUP};
    // default: press, then release
    const int* pAltGrFlags[2] = {&ownFlags[0], &ownFlags[1]};
    // same for Shift
    const int* pShiftFlags[2] = {&ownFlags[0], &ownFlags[1]};

    // forward
    if (nCode < 0)
        return CallNextHookEx(hKeyHook, nCode, wParam, lParam);

    // ignore keys we injected
    if (pKbHS->flags & LLKHF_INJECTED)
        return 0;

    // process
    else if ((wParam == WM_KEYDOWN) || (wParam == WM_SYSKEYDOWN))
    {
        write = 1;

        processGermanDvorakII(
            pKbHS->vkCode, 
            write, 
            vkey,
            unicodeCharacter, 
            toggleAltGr, 
            toggleShift);
    }

    if (write == 1)
    {
        // send characters that are not on keyboard
        if (unicodeCharacter != -1)
        {
            KEYBDINPUT  kb={0};
            INPUT    Input={0};

            ::ZeroMemory(&kb,sizeof(KEYBDINPUT));
            ::ZeroMemory(&Input,sizeof(INPUT));
            kb.dwFlags  =  KEYEVENTF_UNICODE;
            kb.wVk      =  0;
            kb.wScan    = (WORD)unicodeCharacter;
            Input.type  =  INPUT_KEYBOARD;
            Input.ki    =  kb;
            ::SendInput(1,&Input,sizeof(Input));
        }
        else
        {
            // Although AltGr has the same effect as Control + Alt it is not
            // the same on german keyboards, so we have to save whether 
            // Control + Alt were pressed instead of AltGr.
            bool isAltControl = false;

            if (toggleAltGr) // swapping AltGr
            {
                if (isDownAltGr())
                {
                    pAltGrFlags[1] = &ownFlags[0];
                    pAltGrFlags[0] = &ownFlags[1];
                }
                if (isDown(VK_LMENU) && isDown(VK_CONTROL))
                {
                    isAltControl = true;
                    keybd_event(VK_LMENU, 0x45, *pAltGrFlags[0], 0);
                    keybd_event(VK_CONTROL, 0x45, *pAltGrFlags[0], 0);
                }
                else
                    keybd_event(VK_RMENU, 0x45, *pAltGrFlags[0], 0);
            }

            if (toggleShift) // swapping Shift
            {
                if (isDown(VK_SHIFT))
                {
                    pShiftFlags[1] = &ownFlags[0];
                    pShiftFlags[0] = &ownFlags[1];
                }
                keybd_event(VK_SHIFT, 0x45, *pShiftFlags[0], 0);
            }

            // write
            keybd_event(vkey, pKbHS->scanCode, pKbHS->flags, 0);

            if (isAltControl)
            {
                keybd_event(VK_LMENU, 0x45, *pAltGrFlags[1], 0);
                keybd_event(VK_CONTROL, 0x45, *pAltGrFlags[1], 0);
            }
            else if (toggleAltGr)
                keybd_event(VK_RMENU, 0x45, *pAltGrFlags[1], 0);
            if (toggleShift)
                keybd_event(VK_SHIFT, 0x45, *pShiftFlags[1], 0);
        }
    }
    return write;
}
