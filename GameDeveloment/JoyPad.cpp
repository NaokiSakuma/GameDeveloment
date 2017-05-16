//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file  JoyPad.cpp
//!
//! @brief JoyPad(コントローラ)を扱うソースファイル
//!
//! @date   2017/05/16
//!
//! @author N.Sakuma
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

#include "JoyPad.h"

//検出したジョイパッドの情報を格納するための構造体
struct SearchJoyPadSet
{
	//DirectInput
	LPDIRECTINPUT8 dinput;
	//各コントローラごとのデバイス
	std::vector<LPDIRECTINPUTDEVICE8> joyPadDev;
	//コンストラクタ
	SearchJoyPadSet()
	{
		dinput = NULL;
	}
};

BOOL CALLBACK EnumObjectCallBack(LPCDIDEVICEOBJECTINSTANCE pInstance, void* pvRef)
{
	//次を検索
	return DIENUM_CONTINUE;
}

//検出したジョイパッドごとに呼ばれるコールバック
BOOL CALLBACK EnumJoyPadCallBack(LPCDIDEVICEINSTANCE lpddi, void* pvRef)
{
	//結果
	HRESULT ret;
	//SearchJoyPadSet型に変換
	SearchJoyPadSet*param = reinterpret_cast<SearchJoyPadSet*>(pvRef);
	//DirectInputDevice
	LPDIRECTINPUTDEVICE8 dev;
	//コントローラごとにデバイスを生成
	ret = param->dinput->CreateDevice(lpddi->guidInstance,&dev,NULL);
	if (ret != DI_OK)
		goto NEXT;
NEXT:
	//次を検索
	return DIENUM_CONTINUE;
}

//----------------------------------------------------------------------
//! @brief コンストラクタ
//!
//! @param[in] 
//----------------------------------------------------------------------
JoyPad::JoyPad()
{
}

//----------------------------------------------------------------------
//! @brief デストラクタ
//----------------------------------------------------------------------
JoyPad::~JoyPad()
{
}

//----------------------------------------------------------------------
//! @brief 
//!
//! @param[in] 
//!
//! @return 
//----------------------------------------------------------------------
bool JoyPad::Initialize(HWND window)
{
	//結果
	HRESULT ret;
	//DirectInputの作成
	ret = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDInput, NULL);
	//失敗したら
	if (ret != DI_OK)
		return false;
	//受け渡し用パラメータ
	SearchJoyPadSet param;
	param.dinput = m_pDInput;
	//利用可能なデバイスを列挙する
	ret = m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyPadCallBack, &param, DIEDFL_ATTACHEDONLY);
	
	return true;
}
