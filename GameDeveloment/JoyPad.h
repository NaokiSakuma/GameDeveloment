//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file  JoyPad.h
//!
//! @brief JoyPad(�R���g���[��)�������w�b�_�t�@�C��
//!
//! @date   2017/05/16
//!
//! @author N.Sakuma
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

#pragma once
//DirectInput�̃o�[�W�������w��
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>

template<typename T>

//���
void SafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = NULL;
	}
}
class JoyPad
{
public:
	//�R���X�g���N�^
	JoyPad();
	//�f�X�g���N�^
	~JoyPad();
	//�W���C�p�b�h�̏����ݒ�
	bool Initialize(HWND window);
	//�X�V
	void Update();
private:
	//�W���C�p�b�h1���̏��
	struct JoyPadSet
	{
		JoyPadSet()
		{
			inputDevice = NULL;
		}
		//�f�o�C�X
		LPDIRECTINPUTDEVICE8 inputDevice;
		//���͏��
		DIJOYSTATE2 joypad;
		//�O��̓��͏��
		DIJOYSTATE2 joypadOld;
	};
	//DirectInput�C���X�^���X
	LPDIRECTINPUT8 m_pDInput;

	//�W���C�p�b�h�z��
	std::vector<JoyPadSet>m_joyPadSet;
};