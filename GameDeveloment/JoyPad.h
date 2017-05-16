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

class JoyPad
{
public:
	//�R���X�g���N�^
	JoyPad();
	//�f�X�g���N�^
	~JoyPad();
	bool Initialize(HWND window);
private:
	//DirectInput�C���X�^���X
	LPDIRECTINPUT8 m_pDInput;

};