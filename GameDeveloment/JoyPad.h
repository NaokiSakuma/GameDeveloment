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

class JoyPad
{
public:
	//コンストラクタ
	JoyPad();
	//デストラクタ
	~JoyPad();
	bool Initialize(HWND window);
private:
	//DirectInputインスタンス
	LPDIRECTINPUT8 m_pDInput;

};