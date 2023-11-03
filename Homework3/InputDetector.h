#pragma once

#pragma comment(lib, "dinput8.lib")
#include <dinput.h>

class InputDetector
{
public:
	LPDIRECTINPUT8 m_directInput;//�ӿ�
	LPDIRECTINPUTDEVICE8 m_keyboardInputDevice;//�����豸
	LPDIRECTINPUTDEVICE8 m_mouseInputDevice;//����豸
	char m_keyboardStates[256];//�����豸״̬
	DIMOUSESTATE m_mouseStates;//����豸״̬
	InputDetector();
	~InputDetector();//����
	bool Init(HINSTANCE hinst, HWND hwnd);//��ʼ��
	void updata();//ˢ��
	bool key(int code);
private:
	bool DeviceRead(IDirectInputDevice8* pDIDevice, void* pBuffer, long Size);//��ȡ�豸״̬
};

