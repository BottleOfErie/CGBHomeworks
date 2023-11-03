#include "InputDetector.h"


InputDetector::InputDetector()
{
	
}

InputDetector::~InputDetector()
{
    m_keyboardInputDevice->Unacquire();
    m_keyboardInputDevice->Release();
}

bool InputDetector::Init(HINSTANCE hinst, HWND hwnd)
{
    if (FAILED(DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)))return false;
    if (FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboardInputDevice, NULL)))return false;
    if (FAILED(m_keyboardInputDevice->SetDataFormat(&c_dfDIKeyboard)))return false;
    if (FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouseInputDevice, NULL)))return false;
    if (FAILED(m_mouseInputDevice->SetDataFormat(&c_dfDIMouse)))return false;
    m_keyboardInputDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
    m_keyboardInputDevice->Acquire();
    m_mouseInputDevice->SetCooperativeLevel(hwnd, DISCL_FOREGROUND);
    m_mouseInputDevice->Acquire();
    return true;
}

void InputDetector::updata()
{
    DeviceRead(m_keyboardInputDevice, (LPVOID)m_keyboardStates, sizeof(m_keyboardStates));
    ZeroMemory(&m_mouseStates, sizeof(m_mouseStates));
    DeviceRead(m_mouseInputDevice, (LPVOID)&m_mouseStates, sizeof(m_mouseStates));
}

bool InputDetector::key(int code)
{
    return m_keyboardStates[code] & 0x80;
}

bool InputDetector::DeviceRead(IDirectInputDevice8* pDIDevice, void* pBuffer, long Size)
{
    HRESULT hr;
    while (true)
    {
        pDIDevice->Poll();             // 轮询设备
        pDIDevice->Acquire();          // 获取设备的控制权
        if (SUCCEEDED(hr = pDIDevice->GetDeviceState(Size, pBuffer))) break;
        if ((hr != DIERR_INPUTLOST) || (hr != DIERR_NOTACQUIRED)) return FALSE;
        if (FAILED(pDIDevice->Acquire())) return FALSE;
    }
    return TRUE;
}