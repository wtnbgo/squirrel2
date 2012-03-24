#ifndef __GAMEPAD_H_
#define __GAMEPAD_H_

#include "GameObject.h"

enum {
	PAD_UP     = 1<<0,
	PAD_RIGHT  = 1<<1,
	PAD_DOWN   = 1<<2,
	PAD_LEFT   = 1<<3,
	PAD_SELECT = 1<<4,
	PAD_CANCEL = 1<<5,
};

/**
 * キーパッド情報クラス
 * カウント処理未実装
 */
class GamePad : public GameObject {

public:
	static void update();
	static void registerClass();

	/**
	 * コンストラクタ
	 */
	GamePad();

	/**
	 * デストラクタ
	 */
	~GamePad();

	/**
	 * @param key 対象キー
	 * @return 押されているか
	 */
	bool isPress(int key);

	/**
	 * @param key 対象キー
	 * @return 押された瞬間か
	 */
	bool isTrigger(int key);

	/**
	 * @param key 対象キー
	 * @return 離された瞬間か
	 */
	bool isRelease(int key);

	/**
	 * 押されたキーを１つだけ返す（優先度つき）
	 * @return 押されたキー
	 */
	int getKey();

	// テスト用
	static void testObject(const Object *obj);
	static void testGamePad(const GamePad *pad);

	SQRESULT testArgCount(HSQUIRRELVM v);
	static SQRESULT testArgCountStatic(HSQUIRRELVM v);
	
protected:
	static int state;
	static int prevState;
	static int tstate;
	static int rstate;
};

#endif
