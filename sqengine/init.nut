// UTF-8

printf("---- test for exception\n");

// 例外クラスサンプル
class Exception {

	message = null; //< メッセージ記録
	callstacks = null; //< スタック情報記録

	/**
	 * コンストラクタ
	 * @param msg 例外メッセージ
	 */
	constructor(msg) {
		message = msg;
		callstacks = [];
		// 0:getstackinfos
		// 1:このコンストラクタ
		// 2:例外が発生したポイント←ここから取得する
		local level=2;
		local stack;
		while ((stack = ::getstackinfos(level++)) != null) {
			callstacks.append(stack);
		}
	}
};

// 例外クラスを登録
setexceptionclass(Exception);

// 例外階層構造実験用
function nest() {
	// メッセージを渡して例外を起こすだけの関数
	//throwfunc("exception test");
	abc = 10;
}

try {
	nest();
} catch (e) {
	if (e instanceof Exception) {
		printf("exception msg:%s\n", e.message);
		foreach (i,stack in e.callstacks) {
			printf("%d: %s:%04d:%s\n", i, stack.src, stack.line, stack.func);
		}
	} else {
		printf("exception:%s\n", e);
	}
}

printf("---- test for vector/map\n");

// 配列・辞書引き渡しテスト
local t = GameTest();

local a = [3,2,1,2,3];
t.printValue(a);
t.printVector(a);
t.printMap(a);

local m = {a=123,b=456}
t.printValue(m);
t.printVector(m);
t.printMap(m);

printf("---- done\n");

class TestObject extends Object {
	constructor() {
		mapTrigger = {test=999,test2=999};
	}
	mapTrigger = null;
};

local test = TestObject();
testfunc(test);
printf("test:%s\n", test);
printf("test.mapTrigger:%s\n", test.mapTrigger);
printf("test.mapTrigger.test:%s\n",test.mapTrigger.test);
printf("test.mapTrigger.test2:%s\n",test.mapTrigger.test2);

function showArray(name, a) {
	for (local i=0;i<a.len();i++) {
		printf("%s:%d:%s\n", name, i, a[i]);
	}
}

local a = ["a", "b", "c", "d", "e"];
local b = a.slice(1,3);
local c = a.splice(1,3,"v", "w", "x","y","z");
showArray("a", a);
showArray("b", b);
showArray("c", c);

enum Pad {
	UP     = 1,
	RIGHT  = 2,
	DOWN   = 4,
	LEFT   = 8,
	SELECT = 16,
	CANCEL = 32,
};

printf("-- test instance type --\n");

local a = 10;
GamePad.testObject(a);
GamePad.testGamePad(a);

local pad = GamePad();
GamePad.testObject(pad);
GamePad.testGamePad(pad);

local r = GameRect(0,0,10,10,0xffffffff);
GamePad.testObject(r);
GamePad.testGamePad(r);

printf("-- test arg count --\n");

pad.testArgCount(1,2,3,4);
GamePad.testArgCountStatic(1,2,3);

{
	print("キー入力テスト:上下左右で移動,ENTERで終了\n");
	local rect = GameRect(10,10,50,50,0xffffffff);
	rect.visible = true;
	local working = true;
	while (working) {
		local key = pad.getKey();
		switch (key) {
		case Pad.SELECT:
		case Pad.CANCEL:
			working = false;
			break;
		case Pad.UP:
			rect.top -= 10;
			break;
		case Pad.DOWN:
			rect.top += 10;
			break;
		case Pad.LEFT:
			rect.left -= 10;
			break;
		case Pad.RIGHT:
			rect.left += 10;
			break;
		}
		::suspend();
	}
}

function rand1()
{
	return rand().tofloat() / RAND_MAX;
}

function showRect(x, y, size, color, time) {
	local rect = GameRect(x, y, size, size, color);
	rect.visible = true;
	local start = ::getCurrentTick();
	while (true) {
		local now = ::getCurrentTick() - start;
		if (now > time) {
			now = time;
		}
		local opacity = (255 * now / time).tointeger();
		rect.setColor(opacity << 24 | color);
		::suspend();
		if (now >= time) {
			break;
		}
	}
	return rect;
}

{
	print("ランダムで矩形をフェード表示\n");
	local rects = [];
	for (local i=0;i<20;i++) {
		local x = rand1() * 640;
		local y = rand1() * 480;
		local size = 10 + rand1() * 20;
		local color = (rand1() * 0xffffff).tointeger();
		rects.append(showRect(x, y, size, color, 100));
	}
}

function moveFunc(target, left, top, time) {
	local xstart = target.left;
	local ystart = target.top;
	local xdiff = left - target.left;
	local ydiff = top - target.top;
	local start = ::getCurrentTick();
	while (true) {
		local now = ::getCurrentTick() - start;
		if (now > time) {
			now = time;
		}
		target.left = xstart + xdiff * now / time;
		target.top  = ystart + ydiff * now / time;
		::suspend();
		if (now >= time) {
			break;
		}
	}
}

{
	print("forkのテスト：複数のオブジェクト制御関数が平行動作＆キー待ち\n");
	local r1 = GameRect(0,0,100,100,0xffff0000); r1.visible = true;
	local r2 = GameRect(0,0,100,100,0xff00ff00); r2.visible = true;
	local r3 = GameRect(0,0,100,100,0xff0000ff); r3.visible = true;
	::fork(moveFunc, r1, 400,  0, 1000);
	::fork(moveFunc, r2, 400,400, 1000);
	::fork(moveFunc, r3, 0,  400, 1000);
	while (pad.getKey() == 0) {
		::suspend();
	}
}

print("テスト終了");
