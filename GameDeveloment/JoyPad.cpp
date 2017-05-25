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
		//if (ret != DI_OK)
		//	continue;
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
		ret = it->inputDevice->Poll();
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
		if (it->joypad.rgbButtons[0] & 0x80)
		{
			//ボタンが押されている
			static int a = 0;
			a++;
		}
	}
}

//---------------------------------------
// キーを押しているか関数
// [In] パッドID : ボタン ID
// [ret]バッドIDのボタンIDが押されている(true)
//		押されていない(false)
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
// キーが押された瞬間か関数
// [In] パッドID : ボタン ID
// [ret]バッドIDのボタンIDが押された瞬間(true)
//		押された瞬間ではない(false)
//---------------------------------------
bool JoyPad::buttonTrigger(int padId, int buttonId)
{

}
//---------------------------------------
// キーを離した瞬間か関数
// [In] パッドID : ボタン ID
// [ret]バッドIDのボタンIDを離した瞬間(true)
//		離した瞬間ではない(false)
//---------------------------------------
bool JoyPad::buttonRelease(int padId, int buttonId)
{

}
//---------------------------------------
// キーを押している時間
// [In] パッドID : ボタン ID
// [ret]経過時間
//---------------------------------------
int JoyPad::buttonPushTime(int padId, int buttonId)
{

}
//---------------------------------------
// LXスティックの傾き
// [In] パッドID
// [ret]スティックの傾き
//---------------------------------------
LONG JoyPad::getStickLX(int padId)
{

}
//---------------------------------------
// LYスティックの傾き
// [In] パッドID
// [ret]スティックの傾き
//---------------------------------------
LONG JoyPad::getStickLY(int padId)
{

}
//---------------------------------------
// LZスティックの傾き
// [In] パッドID
// [ret]スティックの傾き
//---------------------------------------
LONG JoyPad::getStickLZ(int padId)
{

}
//---------------------------------------
// LRXスティックの傾き
// [In] パッドID
// [ret]スティックの傾き
//---------------------------------------
LONG JoyPad::getStickLRX(int padId)
{

}
//---------------------------------------
// LRYスティックの傾き
// [In] パッドID
// [ret]スティックの傾き
//---------------------------------------
LONG JoyPad::getStickLRY(int padId)
{

}
//---------------------------------------
// LRZスティックの傾き
// [In] パッドID
// [ret]スティックの傾き
//---------------------------------------
LONG JoyPad::getStickLRZ(int padId)
{

}
//---------------------------------------
// 方向キーの取得
// [In] パッドID : キーID
// [ret] 方向キー
//---------------------------------------
DWORD JoyPad::getDirectionKey(int padId)
{

}