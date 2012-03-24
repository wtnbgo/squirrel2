#pragma comment(lib, "d3d9.lib")
#include "Screen.h"

/**
 * コンストラクタ
 */
Screen::Screen() : pD3D(0), pD3DDevice(0)
{
}

/**
 * デストラクタ
 */
Screen::~Screen()
{
	if (pD3DDevice) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}
	if (pD3D) {
		pD3D->Release();
		pD3D = NULL;
	}
}

/**
 * 初期化
 * @param hWnd ウインドウハンドル
 * @return 成功したら true
 */
bool
Screen::init(HWND hWnd)
{
	if ((pD3D=Direct3DCreate9(D3D_SDK_VERSION))){

		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed   = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.MultiSampleQuality = 0;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.Flags = 0;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		
		if (SUCCEEDED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice)) ||
			SUCCEEDED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3DDevice))) {
			return true;
		}
	}
	
	return false;
}

// 画面レンダリング
void
Screen::render()
{
	if (pD3DDevice){
		pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
		if (SUCCEEDED(pD3DDevice->BeginScene())) {
			// 描画処理
			ObjSet::const_iterator it = all.begin();
			while (it != all.end()) {
				ScreenObject *obj = *it;
				if (obj->visible) {
					obj->render(pD3DDevice);
				}
				it++;
			}
			pD3DDevice->EndScene();
		}
		pD3DDevice->Present( NULL, NULL, NULL, NULL );
	}
}

// ------------------------------------------------

void
Screen::add(ScreenObject *obj) {
	all.insert(obj);
}

void
Screen::remove(ScreenObject *obj)
{
	ObjSet::iterator it = all.begin();
	while (it != all.end()) {
		if (*it == obj) {
			all.erase(it);
			break;
		}
		it++;
	}
}

// ------------------------------------------------

ScreenObject::ScreenObject(Screen *screen, int index) : screen(screen), index(index), visible(false)
{
	screen->add(this);
}

ScreenObject::~ScreenObject()
{
	screen->remove(this);
}

void
ScreenObject::setIndex(int index)
{
	if (this->index != index) {
		screen->remove(this);
		this->index = index;
		screen->add(this);
	}
}
