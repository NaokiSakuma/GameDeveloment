//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file  JoyPad.cpp
//!
//! @brief JoyPad(�R���g���[��)�������\�[�X�t�@�C��
//!
//! @date   2017/05/16
//!
//! @author N.Sakuma
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

#include "JoyPad.h"

//���o�����W���C�p�b�h�̏����i�[���邽�߂̍\����
struct SearchJoyPadSet
{
	//DirectInput
	LPDIRECTINPUT8 dinput;
	//�e�R���g���[�����Ƃ̃f�o�C�X
	std::vector<LPDIRECTINPUTDEVICE8> joyPadDev;
	//�R���X�g���N�^
	SearchJoyPadSet()
	{
		dinput = NULL;
	}
};

////���o�������̓I�u�W�F�N�g1���ƂɌĂ΂��R�[���o�b�N
//BOOL CALLBACK EnumObjectCallBack(LPCDIDEVICEOBJECTINSTANCE pInstance, void* pvRef)
//{
//	//��������
//	return DIENUM_CONTINUE;
//}

//���o�����W���C�p�b�h���ƂɌĂ΂��R�[���o�b�N
BOOL CALLBACK EnumJoyPadCallBack(LPCDIDEVICEINSTANCE lpddi, void* pvRef)
{
	//����
	HRESULT ret;
	//SearchJoyPadSet�^�ɕϊ�
	SearchJoyPadSet*param = reinterpret_cast<SearchJoyPadSet*>(pvRef);
	//DirectInputDevice
	LPDIRECTINPUTDEVICE8 dev;
	//�R���g���[�����ƂɃf�o�C�X�𐶐�
	ret = param->dinput->CreateDevice(lpddi->guidInstance,&dev,NULL);
	//���s������
	if (ret != DI_OK)
		goto NEXT;

	//�f�[�^�`����ݒ�(�g���@�\���W���C�X�e�B�b�N)
	ret = dev->SetDataFormat(&c_dfDIJoystick2);
	//���s������
	if (ret != DI_OK)
		goto NEXT;

	//�z��ɒǉ�
	param->joyPadDev.push_back(dev);
NEXT:
	//��������
	return DIENUM_CONTINUE;
}

//----------------------------------------------------------------------
//! @brief �R���X�g���N�^
//!
//! @param[in] 
//----------------------------------------------------------------------
JoyPad::JoyPad()
{
}

//----------------------------------------------------------------------
//! @brief �f�X�g���N�^
//----------------------------------------------------------------------
JoyPad::~JoyPad()
{
	//�z��ɂ���f�o�C�X���J��
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		//���
		SafeRelease(it->inputDevice);
	}
}

//----------------------------------------------------------------------
//! @brief �W���C�p�b�h�̏����ݒ�
//!
//! @param[in] windou�n���h��
//!
//! @return �ʂ邩�ǂ���
//----------------------------------------------------------------------
bool JoyPad::Initialize(HWND window)
{
	//����
	HRESULT ret;
	//DirectInput�̍쐬
	ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL);
	//���s������
	if (ret != DI_OK)
		return false;
	//�󂯓n���p�p�����[�^
	SearchJoyPadSet param;
	param.dinput = m_pDInput;
	//���p�\�ȃf�o�C�X��񋓂���
	ret = m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyPadCallBack, &param, DIEDFL_ATTACHEDONLY);
	//���s������
	if (ret != DI_OK)
		return false;
	//�f�o�C�X�z���ݒ�
	std::vector<LPDIRECTINPUTDEVICE8>::iterator it;
	for (it = param.joyPadDev.begin(); it != param.joyPadDev.end(); it++)
	{
		LPDIRECTINPUTDEVICE8 dev = *it;
		//�A�v�����A�N�e�B�u���Ƀf�o�C�X��r���A�N�Z�X�ɐݒ�
		ret = dev->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		//���s������
		if (ret != DI_OK)
			continue;
		//�A�N�Z�X����v��
		ret = dev->Acquire();
		//���s������
		//if (ret != DI_OK)
		//	continue;
		//�f�o�C�X1���̏��
		JoyPadSet initPad;
		initPad.inputDevice = dev;
		//�z��ɒǉ�����
		m_joyPadSet.push_back(initPad);
	}
	return true;
}

//----------------------------------------------------------------------
//! @brief �X�V
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void JoyPad::Update()
{
	//���ׂẴf�o�C�X�ɂ��ď�������
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		HRESULT ret;
		//�����X�V����
		ret = it->inputDevice->Poll();
		//���s������
		if (ret != DI_OK)
		{
			//�A�N�Z�X����v��
			ret = it->inputDevice->Acquire();
			while (ret == DIERR_INPUTLOST)
			{
				//�A�N�Z�X����v��
				ret = it->inputDevice->Acquire();
			}
		}
		//�O�t���[���̓��͏����o�b�N�A�b�v
		it->joypadOld = it->joypad;
		//���͏����擾
		ret = it->inputDevice->GetDeviceState(sizeof(DIJOYSTATE2), &it->joypad);
		//���s������
		if (ret != DI_OK)
			continue;
		if (it->joypad.rgbButtons[0] & 0x80)
		{
			//�{�^����������Ă���
			static int a = 0;
			a++;
		}
	}
}

//---------------------------------------
// �L�[�������Ă��邩�֐�
// [In] �p�b�hID : �{�^�� ID
// [ret]�o�b�hID�̃{�^��ID��������Ă���(true)
//		������Ă��Ȃ�(false)
//---------------------------------------
bool JoyPad::buttonPush(int padId, int buttonId)
{
	std::vector<JoyPadSet>::iterator it;
	for(it = m_joyPadSet.begin(); it< m_joyPadSet.end(); it++)
	{
		if(it->joypad.rgbButtons[padId] & buttonId
			{
		return true;
	}
	return false;
}
//---------------------------------------
// �L�[�������ꂽ�u�Ԃ��֐�
// [In] �p�b�hID : �{�^�� ID
// [ret]�o�b�hID�̃{�^��ID�������ꂽ�u��(true)
//		�����ꂽ�u�Ԃł͂Ȃ�(false)
//---------------------------------------
bool JoyPad::buttonTrigger(int padId, int buttonId)
{

}
//---------------------------------------
// �L�[�𗣂����u�Ԃ��֐�
// [In] �p�b�hID : �{�^�� ID
// [ret]�o�b�hID�̃{�^��ID�𗣂����u��(true)
//		�������u�Ԃł͂Ȃ�(false)
//---------------------------------------
bool JoyPad::buttonRelease(int padId, int buttonId)
{

}
//---------------------------------------
// �L�[�������Ă��鎞��
// [In] �p�b�hID : �{�^�� ID
// [ret]�o�ߎ���
//---------------------------------------
int JoyPad::buttonPushTime(int padId, int buttonId)
{

}
//---------------------------------------
// LX�X�e�B�b�N�̌X��
// [In] �p�b�hID
// [ret]�X�e�B�b�N�̌X��
//---------------------------------------
LONG JoyPad::getStickLX(int padId)
{

}
//---------------------------------------
// LY�X�e�B�b�N�̌X��
// [In] �p�b�hID
// [ret]�X�e�B�b�N�̌X��
//---------------------------------------
LONG JoyPad::getStickLY(int padId)
{

}
//---------------------------------------
// LZ�X�e�B�b�N�̌X��
// [In] �p�b�hID
// [ret]�X�e�B�b�N�̌X��
//---------------------------------------
LONG JoyPad::getStickLZ(int padId)
{

}
//---------------------------------------
// LRX�X�e�B�b�N�̌X��
// [In] �p�b�hID
// [ret]�X�e�B�b�N�̌X��
//---------------------------------------
LONG JoyPad::getStickLRX(int padId)
{

}
//---------------------------------------
// LRY�X�e�B�b�N�̌X��
// [In] �p�b�hID
// [ret]�X�e�B�b�N�̌X��
//---------------------------------------
LONG JoyPad::getStickLRY(int padId)
{

}
//---------------------------------------
// LRZ�X�e�B�b�N�̌X��
// [In] �p�b�hID
// [ret]�X�e�B�b�N�̌X��
//---------------------------------------
LONG JoyPad::getStickLRZ(int padId)
{

}
//---------------------------------------
// �����L�[�̎擾
// [In] �p�b�hID : �L�[ID
// [ret] �����L�[
//---------------------------------------
DWORD JoyPad::getDirectionKey(int padId)
{

}