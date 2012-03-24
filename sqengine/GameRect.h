#ifndef __GAMERECT_H_
#define __GAMERECT_H_

#include "GameObject.h"
#include "Screen.h"

/**
 * ‹éŒ`•`‰æˆ—
 */	
class GameRect : public GameObject, ScreenObject {

	friend class Screen;
public:
	static void registerClass();
	
	GameRect(HSQUIRRELVM vm);

	~GameRect();

	void setLeft(float left) {
		this->left = left;
	}

	float getLeft() {
		return left;
	}

	void setTop(float top) {
		this->top = top;
	}

	float getTop() {
		return top;
	}

	void setWidth(float width) {
		this->width = width;
	}

	float getWidth() {
		return width;
	}

	void setHeight(float height) {
		this->height = height;
	}

	float getHeight() {
		return height;
	}

	void setColor(int color) {
		this->color = color;
	}

	int getColor() {
		return color;
	}

	bool getVisible() const {
		return visible;
	}

	void setVisible(bool visible) {
		this->visible = visible;
	}

	int getIndex() const {
		return index;
	}

	void setIndex(int index) {
		ScreenObject::setIndex(index);
	}
	
protected:
	float left;
	float top;
	float width;
	float height;
	int color;

	// •`‰æˆ—
	virtual void render(LPDIRECT3DDEVICE9 pD3DDevice);
};
#endif
