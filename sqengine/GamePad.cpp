#include <windows.h>
#include "GamePad.h"
#include <sqobject/sqratfunc.h>

// 入力状態(全オブジェクト共通)
int GamePad::state     = 0;
int GamePad::prevState = 0;
int GamePad::tstate    = 0;
int GamePad::rstate    = 0;

/**
 * コンストラクタ
 */
GamePad::GamePad()
{
	sqobject::printf(_SC("pad addr:%08x\n"), (int)this);
};

/**
 * デストラクタ
 */
GamePad::~GamePad()
{
}

/**
 * @param key 対象キー
 * @return 押されているか
 */
bool
GamePad::isPress(int key)
{
	return (state & key) != 0;
}

/**
 * @param key 対象キー
 * @return 押された瞬間か
 */
bool
GamePad::isTrigger(int key)
{
	return (tstate & key) != 0;
}

/**
 * @param key 対象キー
 * @return 離されているか
 */
bool
GamePad::isRelease(int key)
{
	return (rstate & key) != 0;
}

// キー情報
struct KeyInfo {
	int key;
	int vkey;
};
static KeyInfo keyinfos[] = {
	{PAD_SELECT, VK_RETURN},
	{PAD_CANCEL, VK_ESCAPE},
	{PAD_UP,     VK_UP},
	{PAD_RIGHT,  VK_RIGHT},
	{PAD_DOWN,   VK_DOWN},
	{PAD_LEFT,   VK_LEFT},
};

/**
 * 押されたキーを１つだけ返す（優先度つき）
 * @return 押されたキー
 */
int
GamePad::getKey()
{
	for (int i=0;i<sizeof(keyinfos)/sizeof(keyinfos[0]);i++) {
		KeyInfo *info = keyinfos + i;
		if (isTrigger(info->key)) {
			return info->key;
		}
	}
	return 0;
}

/**
 * 状態更新用
 */
void
GamePad::update()
{
	prevState = state;
	// 入力状態更新
	if (GetFocus() != NULL) {
		state = 0;
		for (int i=0;i<sizeof(keyinfos)/sizeof(keyinfos[0]);i++) {
			KeyInfo *info = keyinfos + i;
			if (::GetAsyncKeyState(info->vkey) & 0x8000) {
				state |= info->key;
			}
		}
	}
	tstate = (~prevState & state);  // トリガ
	rstate = ~(prevState & ~state); // リリース
}

void
GamePad::testObject(const Object *obj)
{
	int addr = (int)obj;
	sqobject::printf(_SC("test object:%08x\n"), addr);
}

void
GamePad::testGamePad(const GamePad *pad)
{
	int addr = (int)pad;
	sqobject::printf(_SC("test gamepad:%08x\n"), addr);
}

SQRESULT
GamePad::testArgCount(HSQUIRRELVM v)
{
	int n = sq_gettop(v);
	sqobject::printf(_SC("argnum:%d\n"), n);
	for (int i=1;i<=n;i++) {
		sqobject::printf(_SC(" %d:%s\n"), i, sqobject::StackValue(v,i));
	}
	return SQ_OK;
}

SQRESULT
GamePad::testArgCountStatic(HSQUIRRELVM v)
{
	int n = sq_gettop(v);
	sqobject::printf(_SC("argnum:%d\n"), n);
	for (int i=1;i<=n;i++) {
		sqobject::printf(_SC(" %d:%s\n"), i, sqobject::StackValue(v,i));
	}
	return SQ_OK;
}

/**
 * クラス登録
 */
void
GamePad::registerClass()
{
	SQCLASSOBJ(GamePad, _SC("GamePad"));
	SQFUNC(GamePad, isPress);
	SQFUNC(GamePad, isTrigger);
	SQFUNC(GamePad, isRelease);
	SQFUNC(GamePad, getKey);
	SQSFUNC(GamePad, testObject);
	SQSFUNC(GamePad, testGamePad);
	SQVFUNC(GamePad, testArgCount);
	SQSVFUNC(GamePad, testArgCountStatic);
}
