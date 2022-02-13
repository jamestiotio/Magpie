#include "pch.h"
#include "FrameRateDrawer.h"
#include "App.h"
#include "resource.h"
#include "Renderer.h"


bool FrameRateDrawer::Initialize(winrt::com_ptr<ID3D11Texture2D> renderTarget, const RECT& destRect) {
	Renderer& renderer = App::GetInstance().GetRenderer();
	_d3dDC = renderer.GetD3DDC();
	if (!renderer.GetRenderTargetView(renderTarget.get(), &_rtv)) {
		return false;
	}

	_vp.MaxDepth = 1.0f;
	_vp.TopLeftX = (FLOAT)destRect.left;
	_vp.TopLeftY = (FLOAT)destRect.top;
	_vp.Width = FLOAT(destRect.right - destRect.left);
	_vp.Height = FLOAT(destRect.bottom - destRect.top);

	_spriteBatch.reset(new SpriteBatch(renderer.GetD3DDC().get()));

	// 从资源文件获取字体
	HMODULE hInst = App::GetInstance().GetHInstance();
	HRSRC hRsrc = FindResource(hInst, MAKEINTRESOURCE(IDR_FRAME_RATE_FONT), RT_RCDATA);
	if (!hRsrc) {
		return false;
	}
	HGLOBAL hRes = LoadResource(hInst, hRsrc);
	if (!hRes) {
		return false;
	}

	_spriteFont.reset(new SpriteFont(renderer.GetD3DDevice().get(),
		(const uint8_t*)LockResource(hRes), SizeofResource(hInst, hRsrc)));
	return true;
}

void FrameRateDrawer::Draw() {
	const GPUTimer& timer = App::GetInstance().GetRenderer().GetTimer();

	_d3dDC->OMSetRenderTargets(1, &_rtv, nullptr);
	_d3dDC->RSSetViewports(1, &_vp);

	_spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Immediate);

	constexpr float posX = 10.0f, posY = 10.0f;
	std::string fpsStr = fmt::format("{} FPS", timer.GetFramesPerSecond());

	// 右下角浓阴影，左上角淡阴影
	_spriteFont->DrawString(_spriteBatch.get(), fpsStr.c_str(),
		XMFLOAT2(posX + 1.0f, posY + 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.8f));
	_spriteFont->DrawString(_spriteBatch.get(), fpsStr.c_str(),
		XMFLOAT2(posX - 0.1f, posY - 0.1f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.4f));
	_spriteFont->DrawString(_spriteBatch.get(), fpsStr.c_str(),
		XMFLOAT2(posX, posY), Colors::White);

	_spriteBatch->End();
}
