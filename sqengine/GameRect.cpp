#include "GameRect.h"
#include "Screen.h"

#include <sqobject/sqratfunc.h>

extern Screen *screen;

/**
 * コンストラクタ
 */
GameRect::GameRect(HSQUIRRELVM vm) : GameObject(), ScreenObject(::screen, 1), left(0), top(0), width(1), height(1), color(0xffffffff)
{
	sqobject::StackInfo st(vm);
	int n = st.len();
	if (n > 0) left   = st[0];
	if (n > 1) top    = st[1];
	if (n > 2) width  = st[2];
	if (n > 3) height = st[3];
	if (n > 4) color  = st[4];
}

/**
 * デストラクタ
 */
GameRect::~GameRect()
{
}


// 構造体
struct CUSTOMVERTEX
{
	FLOAT x, y, z;
	FLOAT rhw;
	D3DCOLOR color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

// 描画処理
void
GameRect::render(LPDIRECT3DDEVICE9 pD3DDevice)
{
	float right  = left + width;
	float bottom = top + height;
	CUSTOMVERTEX v[4] = {
		// x, y, z, color
		{ left,  top,    0.0f, 1.0f, color },
		{ right, top,    0.0f, 1.0f, color },
		{ right, bottom, 0.0f, 1.0f, color },
		{ left,  bottom, 0.0f, 1.0f, color }
	};
	// alpha
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pD3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pD3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof v[0]);
}

/**
 * クラス登録
 */
void
GameRect::registerClass()
{
	SQCLASSOBJ_VCONSTRUCTOR(GameRect, _SC("GameRect"));
	SQFUNC(GameRect, setIndex);
	SQFUNC(GameRect, getIndex);
	SQFUNC(GameRect, setVisible);
	SQFUNC(GameRect, getVisible);
	SQFUNC(GameRect, setLeft);
	SQFUNC(GameRect, getLeft);
	SQFUNC(GameRect, setTop);
	SQFUNC(GameRect, getTop);
	SQFUNC(GameRect, setWidth);
	SQFUNC(GameRect, getWidth);
	SQFUNC(GameRect, setHeight);
	SQFUNC(GameRect, getHeight);
	SQFUNC(GameRect, setColor);
	SQFUNC(GameRect, getColor);
}
