#ifndef __INCLUDE_GAMEOBJ__
#define __INCLUDE_GAMEOBJ__

#include <set>
#include <sqobject/sqobjectclass.h>

/**
 * ゲーム用オブジェクト基底クラス
 */
class GameObject : public sqobject::Object {

public:
	/// コンストラクタ
	GameObject();
	/// デストラクタ
	virtual ~GameObject();

	/**
	 * 状態を更新
	 * @param diff 経過時間
	 */
	static void updateTick(int diff);

	/**
	 * スクリプト実行前更新処理
	 */
	static void updateBefore();

	/**
	 * スクリプト実行後更新処理
	 */
	static void updateAfter();

	/**
	 * @return オブジェクト数
	 */
	static int getCount();

protected:
	static unsigned int currentTick; //< 現在の tick値記録用
	typedef std::set<GameObject*> GameObjectSet; //< オブジェクト一覧
	static GameObjectSet objects; //< オブジェクト一覧
	static void add(GameObject *obj); //< オブジェクト追加
	static void remove(GameObject *obj); //< オブジェクト削除

	/**
	 * 状態を更新
	 */
	virtual void before(){};
	virtual void after(){};
};

#endif
