//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file  JoyPad.h
//!
//! @brief JoyPad(コントローラ)を扱うヘッダファイル
//!
//! @date   2017/05/16
//!
//! @author N.Sakuma
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

#pragma once
//DirectInputのバージョンを指定
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vector>

//方向キーのフラグを定義する
#define PAD_DIR_UP 0x01		//上
#define PAD_DIR_LEFT 0x02	//右
#define PAD_DIR_DOWN 0x04	//下
#define PAD_DIR_RIGHT 0x08	//右
#define PAD_DIR_NONE 0x10	//なし

template<typename T>

//解放
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
	//コンストラクタ
	JoyPad();
	//デストラクタ
	~JoyPad();
	//ジョイパッドの初期設定
	bool Initialize(HWND window);
	//更新
	void Update();
	bool buttonPush(int padId, int buttonId);
	bool buttonTrigger(int padId, int buttonId);
	bool buttonRelease(int padId, int buttonId);
	int buttonPushTime(int padId, int buttonId);
	LONG getStickLX(int padId);
	LONG getStickLY(int padId);
	LONG getStickLZ(int padId);
	LONG getStickLRX(int padId);
	LONG getStickLRY(int padId);
	LONG getStickLRZ(int padId);
	DWORD getDirectionKey(int padId);
private:
	//ジョイパッド1個分の情報
	struct JoyPadSet
	{
		JoyPadSet()
		{
			inputDevice = NULL;
		}
		//デバイス
		LPDIRECTINPUTDEVICE8 inputDevice;
		//入力情報
		DIJOYSTATE2 joypad;
		//前回の入力情報
		DIJOYSTATE2 joypadOld;
	};
	//DirectInputインスタンス
	LPDIRECTINPUT8 m_pDInput;

	//ジョイパッド配列
	std::vector<JoyPadSet>m_joyPadSet;
};