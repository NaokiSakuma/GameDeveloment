//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "ADX2Le.h"
#include "Resources\Music\Basic.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <sstream>
#include <CommonStates.h>
extern void ExitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() :
    m_window(0),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

Game::~Game()
{
	//サウンドライブラリの終了処理
	ADX2Le::Finalize();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
	//スプライトバッチの生成
	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
	//スプライトフォントの生成
	m_spriteFont = std::make_unique<SpriteFont>(m_d3dDevice.Get(), L"Resources/myfile.spritefont");
	//カウンターの初期化
	m_count = 0;
	//リソース情報
	ComPtr<ID3D11Resource> resource;
	//png用
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(m_d3dDevice.Get(), L"Resources/cat.png",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));
	//dds用
	//DX::ThrowIfFailed(
	//	CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Resources/cat.dds",
	//		resource.GetAddressOf(),
	//		m_texture.ReleaseAndGetAddressOf()));
	//猫のテクスチャ
	ComPtr<ID3D11Texture2D> cat;
	DX::ThrowIfFailed(resource.As(&cat));
	//テクスチャの情報
	CD3D11_TEXTURE2D_DESC catDesc;
	cat->GetDesc(&catDesc);	//テクスチャの縦幅、横幅を取得

	//テクスチャの原点を画像の中心にする
	m_origin.x = float(catDesc.Width / 2);	//x座標
	m_origin.y = float(catDesc.Height / 2); //y座標
	//表示座標を画面の中央に指定
	m_screenPos.x = m_outputWidth / 2.f;	//x座標
	m_screenPos.y = m_outputHeight / 2.f;	//y座標
	//キーボードのインスタンス生成
	m_keyboard = std::make_unique<Keyboard>();
	//マウスのインスタンス生成
	m_mouse = std::make_unique<Mouse>();
	//ウィンドウハンドラを通知
	m_mouse->SetWindow(window);

	//acfファイルの読み込み
	ADX2Le::Initialize("Resources/Music/ADX2_samples.acf");
	//ACBとAWBを読み込む
	ADX2Le::LoadAcb("Resources/Music/CueSheet_0.acb", "Resources/Music/CueSheet_0.awb");
	//音の再生
	ADX2Le::Play(CRI_BASIC_MUSIC1);
	//ゲームパッドのインスタンス生成
	m_gamepad = std::make_unique<GamePad>();
	//フラグの初期化
	m_changeFlag = true;
	//ジョイパッドのインスタンス生成
	m_joypad = std::make_unique<JoyPad>();
	m_joypad->Initialize(window);
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//サウンドライブラリの毎フレーム更新
	ADX2Le::Update();
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

	//カウンターを進める
	m_count++;
	//文字列に代入
	//m_str = L"aiueo";
	//ストリング
	std::wstringstream ss;
	//描画
	ss << L"aiueo" << m_count;
	//文字列に代入
	m_str = ss.str();
	//キーボードの状態を取得、auto...型を自動で判別してくれる
	Keyboard::State kb = m_keyboard->GetState();
	//キーボードトラッカーの更新
	m_keyboardTracker.Update(kb);
	if (kb.Back)
	{
		m_str = L"BackSpace";
		// Backspace key is down
	}
	else if (m_keyboardTracker.IsKeyReleased(Keyboard::Keys::A))
	{
		// Space was just pressed down
		m_str = L"A";
	}
	else if (m_keyboardTracker.pressed.B)
	{
		m_str = L"B";
	}
	else
	{
		m_str = L"";
	}
	//マウスの状態を取得
	Mouse::State state = m_mouse->GetState();
	m_tracker.Update(state);

	if (m_tracker.rightButton == Mouse::ButtonStateTracker::PRESSED)
	{
		m_str = L"Right";
	}
	//if (state.leftButton)
	//{
	//	m_str = L"Left";
	//}
	//else
	//{
	//	m_str = L"";
	//}
	//XMFLOART2...Vector2のようなもの
	XMFLOAT2 mousePosInPixels(float(state.x), float(state.y));
	//マウスの座標に猫がついてくるように
	m_screenPos = mousePosInPixels;
	if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
	{
		m_mouse->SetMode(Mouse::MODE_RELATIVE);
	}
	else if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
	}

	//ゲームパッドの状態を取得
	DirectX::GamePad::State padstate = m_gamepad->GetState(0 , GamePad::DEAD_ZONE_CIRCULAR);
	//ゲームパッドトラッカーの更新
	m_padTracker.Update(padstate);
	//コントローラーが接続されているか
	if (padstate.IsConnected())
	{
		//Aボタンが押されているか
		if (padstate.IsAPressed())
		{
			if (m_changeFlag)
				//今押されている
				m_str = L"Attack";
			else if (!m_changeFlag)
				m_str = L"Guard";
		}
		//Bボタンが押されているか
		else if (padstate.IsBPressed())
		{
			if (m_changeFlag)
				//今押されている
				m_str = L"Guard";
			else if (!m_changeFlag)
				m_str = L"Attack";
		}
		//セレクトキーが押されているか
		if (m_padTracker.back == GamePad::ButtonStateTracker::ButtonState::PRESSED)
		{
			if (m_changeFlag)
				m_changeFlag = false;
			else if (!m_changeFlag)
				m_changeFlag = true;
		}
		//方向キーの下が押されているか
		if (padstate.IsDPadDownPressed())
		{

		}
		//左スティックの左右(-1～1の範囲でとれる)
		float posx = padstate.thumbSticks.leftX;
		//左スティックの上下
		float posy = padstate.thumbSticks.leftY;
		//右トリガーがどれだけ押されているか(0～１の範囲でとれる)
		float throttle = padstate.triggers.right;
		//振動の設定
		//m_gamepad->SetVibration(0, 0.5f, 0.25f);
		//デバイスの能力を取得
		GamePad::Capabilities caps = m_gamepad->GetCapabilities(0);
	}
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    // TODO: Add your rendering code here.
	CommonStates states(m_d3dDevice.Get());
	//スプライトバッチの描画開始 デフォルトのオプション　αブレンドを変える
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	//テクスチャの切り取り矩形
	//RECT rect{ 30,30,70,70 };
	//スプライトの描画
	m_spriteBatch->Draw(
		m_texture.Get(),			//テクスチャの情報 
		m_screenPos,				//位置
		nullptr,					//画像の切り取り(AABB)
		Colors::White,				//色
		XMConvertToRadians(90),		//角度(ラジアン)、角度をラジアンにする関数
		m_origin,					//回転中心点
		1.0f);						//大きさの倍率(指定しないとデフォルトで1.0f)
	//スプライトフォントの描画		　スプライトバッチ　　　描画するもの　　x座標,y座標
	m_spriteFont->DrawString(m_spriteBatch.get(), m_str.c_str(), XMFLOAT2(100, 100));
	//スプライトバッチの描画終了
	m_spriteBatch->End();
    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels (see code below related to 11.1 fallback handling).
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
        );

    if (hr == E_INVALIDARG)
    {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it.
        hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            &featureLevels[1],
            _countof(featureLevels) - 1,
            D3D11_SDK_VERSION,
            m_d3dDevice.ReleaseAndGetAddressOf(),
            &m_featureLevel,
            m_d3dContext.ReleaseAndGetAddressOf()
            );
    }

    DX::ThrowIfFailed(hr);

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    // DirectX 11.1 if present
    if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
        (void)m_d3dContext.As(&m_d3dContext1);

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        ComPtr<IDXGIFactory2> dxgiFactory2;
        if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
        {
            // DirectX 11.1 or later

            // Create a descriptor for the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = backBufferWidth;
            swapChainDesc.Height = backBufferHeight;
            swapChainDesc.Format = backBufferFormat;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = backBufferCount;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
            fsSwapChainDesc.Windowed = TRUE;

            // Create a SwapChain from a Win32 window.
            DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
                m_d3dDevice.Get(),
                m_window,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                m_swapChain1.ReleaseAndGetAddressOf()
                ));

            DX::ThrowIfFailed(m_swapChain1.As(&m_swapChain));
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.BufferDesc.Width = backBufferWidth;
            swapChainDesc.BufferDesc.Height = backBufferHeight;
            swapChainDesc.BufferDesc.Format = backBufferFormat;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = m_window;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Windowed = TRUE;

            DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
        }

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain1.Reset();
    m_swapChain.Reset();
    m_d3dContext1.Reset();
    m_d3dContext.Reset();
    m_d3dDevice1.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}