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

////検出した入力オブジェクト1個ごとに呼ばれるコールバック
//BOOL CALLBACK EnumObjectCallBack(LPCDIDEVICEOBJECTINSTANCE pInstance, void* pvRef)
//{
//	//次を検索
//	return DIENUM_CONTINUE;
//}

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
	//失敗したら
	if (ret != DI_OK)
		goto NEXT;

	//データ形式を設定(拡張機能つきジョイスティック)
	ret = dev->SetDataFormat(&c_dfDIJoystick2);
	//失敗したら
	if (ret != DI_OK)
		goto NEXT;

	//配列に追加
	param->joyPadDev.push_back(dev);
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
	//配列にあるデバイスを開放
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		//解放
		SafeRelease(it->inputDevice);
	}
}

//----------------------------------------------------------------------
//! @brief ジョイパッドの初期設定
//!
//! @param[in] windouハンドル
//!
//! @return 通るかどうか
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
	//失敗したら
	if (ret != DI_OK)
		return false;
	//デバイス配列を設定
	std::vector<LPDIRECTINPUTDEVICE8>::iterator it;
	for (it = param.joyPadDev.begin(); it != param.joyPadDev.end(); it++)
	{
		LPDIRECTINPUTDEVICE8 dev = *it;
		//アプリがアクティブ時にデバイスを排他アクセスに設定
		ret = dev->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
		//失敗したら
		if (ret != DI_OK)
			continue;
		//アクセス権を要求
		ret = dev->Acquire();
		//失敗したら
		if (ret != DI_OK)
			continue;
		//デバイス1個分の情報
		JoyPadSet initPad;
		initPad.inputDevice = dev;
		//配列に追加する
		m_joyPadSet.push_back(initPad);
	}
	return true;
}

//----------------------------------------------------------------------
//! @brief 更新
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void JoyPad::Update()
{
	//すべてのデバイスについて処理する
	std::vector<JoyPadSet>::iterator it;
	for (it = m_joyPadSet.begin(); it != m_joyPadSet.end(); it++)
	{
		HRESULT ret;
		//情報を更新する
		it->inputDevice->Poll();
		//失敗したら
		if (ret != DI_OK)
		{
			//アクセス権を要求
			ret = it->inputDevice->Acquire();
			while (ret == DIERR_INPUTLOST)
			{
				//アクセス権を要求
				ret = it->inputDevice->Acquire();
			}
		}
		//前フレームの入力情報をバックアップ
		it->joypadOld = it->joypad;
		//入力情報を取得
		ret = it->inputDevice->GetDeviceState(sizeof(DIJOYSTATE2), &it->joypad);
		//失敗したら
		if (ret != DI_OK)
			continue;
		if ((BYTE)(it->joypad.rgbButtons[0] & 0x80 == 0))
		{
			//ボタンが押されている
		}
	}
}
