#include "GameObject.h"

GameObject::GameObjectSet GameObject::objects;
unsigned int GameObject::currentTick;

/**
 * @return オブジェクト数
 */
int
GameObject::getCount()
{
	return objects.size();
}

/// オブジェクト追加
void
GameObject::add(GameObject *object)
{
	objects.insert(object);
}

/// オブジェクト削除
void
GameObject::remove(GameObject *object)
{
	objects.erase(object);
}

/// コンストラクタ
GameObject::GameObject()
{
	add(this);
}

/// デストラクタ
GameObject::~GameObject()
{
	remove(this);
}

/**
 * 状態を更新
 * @param diff 経過時間
 */
void
GameObject::updateTick(int diff)
{
	currentTick += diff;
}

/**
 * スクリプト実行前更新処理
 */
void
GameObject::updateBefore()
{
	// XXX 必要ならオブジェクトクラス毎の初期化処理をここで呼び出す
	GameObjectSet::iterator i = objects.begin();
	while (i != objects.end()) {
		(*i)->before();
		i++;
	}
}

/**
 * スクリプト実行後更新処理
 */
void
GameObject::updateAfter()
{
	GameObjectSet::iterator i = objects.begin();
	while (i != objects.end()) {
		(*i)->after();
		i++;
	}
}

