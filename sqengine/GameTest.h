#ifndef __GAMETEST_H_
#define __GAMETEST_H_

#include "GameObject.h"
#include <vector>
#include <map>

/**
 * テスト用
 */	
class GameTest : public GameObject {

	friend class Screen;
public:

	static void registerClass() {
		SQCLASSOBJ(GameTest, _SC("GameTest"));
		SQFUNC(GameTest, printValue);
		SQFUNC(GameTest, printVector);
		SQFUNC(GameTest, printMap);
	}

	// コンストラクタ
	GameTest() {
	}

	// ObjectInfo を引数にとるテスト
	void printValue(const sqobject::ObjectInfo &obj) {
		sqobject::printf(_SC("value type:%x:%s\n"), obj.type(), obj);
	}
	
	// ベクタを引数にとるテスト
	typedef std::vector<int> V;
	void printVector(const V &value) {
		if (value.size() > 0) {
			V::const_iterator it = value.begin();
			while (it != value.end()) {
				sqobject::printf(_SC("vector %d\n"), *it);
				it++;
			}
		} else {
			sqobject::printf(_SC("vector empty\n"));
		}
	}

	// mapを引数にとるテスト
	typedef std::map<sqobject::sqstring, int> M;
	void printMap(const M &value) {
		if (value.size() > 0) {
			M::const_iterator it = value.begin();
			while (it != value.end()) {
				sqobject::printf(_SC("map %s:%d\n"), it->first, it->second);
				it++;
			}
		} else {
			sqobject::printf(_SC("map empty\n"));
		}
	}
};
#endif
