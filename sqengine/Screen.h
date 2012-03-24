#ifndef __GAME_SCREEN
#define __GAME_SCREEN

#include <d3d9.h>
#include <set>
#include <algorithm>

class Screen;

/**
 * 描画対象オブジェクト
 */
class ScreenObject { 
	friend class Screen;
public:
	ScreenObject(Screen *screen, int index);
	~ScreenObject();
	
	void setIndex(int index);

protected:
	Screen *screen;
	int index;
	bool visible;
	virtual void render(LPDIRECT3DDEVICE9 pD3DDevice) = 0;
};

/**
 * 描画ベース
 */
class Screen {

	friend class ScreenObject;
public:
	/**
	 * コンストラクタ
	 */
	Screen();

	/**
	 * デストラクタ
	 */
	~Screen();

	/**
	 * 初期化
	 * @param hWnd ウインドウハンドル
	 * @return 成功したら true
	 */
	bool init(HWND hWnd);

	/**
	 * 画面レンダリング実行
	 */
	void render();
	
protected:
	LPDIRECT3D9 pD3D;
	LPDIRECT3DDEVICE9 pD3DDevice;

	typedef ScreenObject *OBJP;

	// 順序比較関数: index でソートする
	struct lessObj : std::binary_function <ScreenObject*,ScreenObject*,bool> {
		bool operator() (const OBJP &obj1, const OBJP &obj2) {
			return obj1->index < obj2->index;
		}
	};
	typedef std::multiset<OBJP,lessObj> ObjSet;
	// オブジェクト登録用
	ObjSet all;

	void add(ScreenObject *obj);
	void remove(ScreenObject *obj);
};

#endif
