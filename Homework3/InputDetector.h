#pragma once

#pragma comment(lib, "dinput8.lib")
#include <dinput.h>

class InputDetector
{
public:
	LPDIRECTINPUT8 m_directInput;//接口
	LPDIRECTINPUTDEVICE8 m_keyboardInputDevice;//键盘设备
	LPDIRECTINPUTDEVICE8 m_mouseInputDevice;//鼠标设备
	char m_keyboardStates[256];//键盘设备状态
	DIMOUSESTATE m_mouseStates;//鼠标设备状态
	InputDetector();
	~InputDetector();//析构
	bool Init(HINSTANCE hinst, HWND hwnd);//初始化
	void updata();//刷新
	bool key(int code);
private:
	bool DeviceRead(IDirectInputDevice8* pDIDevice, void* pBuffer, long Size);//读取设备状态
};

