#include <windows.h>
#include <tchar.h>

#define SCWIDTH  640
#define SCHEIGHT 480
#define INITFILE _SC("init.nut")

#include <sqobject/sqthread.h>

/**
 * squirrel ログ出力用
 * @param v Squirrel VM
 * @param format 書式指定
 */
static void PrintFunc(HSQUIRRELVM v, const SQChar* format, ...)
{
	va_list args;
	va_start(args, format);
	SQChar msg[1024];
	DWORD len = _vsntprintf(msg, 1024, format, args);
	::WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), msg, len, &len, NULL);
	va_end(args);
}

/**
 * ウインドウプロシージャ
 */
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#include "Screen.h"
#include "GameObject.h"
#include "GameRect.h"
#include "GamePad.h"

#include <sqobject/sqratfunc.h>
#include "GameTest.h"

Screen *screen = NULL;

static void printAll(const sqobject::ObjectInfo &key, const sqobject::ObjectInfo &value, void *userData)
{
	HSQUIRRELVM v = (HSQUIRRELVM)userData;
	PrintFunc(v, _SC("key:%s value:%s\n"), key.toString().c_str(), value.toString().c_str());
}

static void test(HSQUIRRELVM v)
{
	sqobject::ObjectInfo table = sqobject::ObjectInfo::createTable();

	table[2] = 1;
	table[_SC("a")] = 2;
	table[1] = _SC("test1");
	table[_SC("b")] = _SC("test2");

	int a = table[2];
	int b = table[_SC("a")];
	const SQChar *c = table[1];
	const SQChar *d = table[_SC("b")];
	sqobject::ObjectInfo e = table[1];
	
	PrintFunc(v, _SC("a:%d b:%d c:%s d:%s e:%s\n"), a, b, c, d, (const SQChar*)e);
	PrintFunc(v, _SC("\"a\" in table:%d \"c\" in table:%d\n"), table.has(_SC("a")) ? 1 : 0 , table.has(_SC("c")) ? 1 : 0);

	PrintFunc(v, _SC("iterator test: function\n"));
	table.foreach(printAll, (void*)v);

	PrintFunc(v, _SC("iterator test: function object\n"));
	struct {
		HSQUIRRELVM v;
		void operator()(sqobject::ObjectInfo &key, sqobject::ObjectInfo &value) {
			PrintFunc(v, _SC("key:%s value:%s\n"), key.toString().c_str(), value.toString().c_str());
		}
	} func = {v};
	table.foreach(func);
	
	sqobject::ObjectInfo array = sqobject::ObjectInfo::createArray();
	array.append(1);
	array.append(_SC("abc"));
	array.append(2);
	array.append(_SC("test"));
	array.append(_SC("test"));
	array.append(_SC("test2"));
	array.append(_SC("test2"));
	array.append((void*)NULL);

	array.removeValue(2);
	array.removeValue(_SC("test"), true);
	array.removeValue(_SC("test2"), false);

	struct {
		void operator()(int idx, sqobject::ObjectInfo &value) {
			sqobject::printf(_SC("idx:%d value:%s\n"), idx, value);
		}
	} afunc;
	array.foreach(afunc);
}

static SQRESULT throwfunc(HSQUIRRELVM v)
{
	sqobject::StackInfo stack(v);
	return sq_throwerror(v, stack.getArg(0));
}

static SQRESULT testfunc(HSQUIRRELVM v)
{
	sqobject::StackInfo stack(v);

#if 0
	sqobject::ObjectInfo engine = stack.getArg(0);
#if 1
	sqobject::ObjectInfo mapTrigger = engine.get(_SC("mapTrigger"));
	mapTrigger.create(_SC("test"), 10);
	mapTrigger.create(_SC("test2"), _SC("hoge"));
#else
	sqobject::ObjectInfo mapTrigger;
	mapTrigger.initTable();
	mapTrigger.create(_SC("test"),  20);
	mapTrigger.create(_SC("test2"), _SC("huga"));
	engine.set(_SC("mapTrigger"), mapTrigger);
#endif
#else
	stack[0][_SC("mapTrigger")][_SC("test")] = 30;
	stack[0][_SC("mapTrigger")][_SC("test2")] = _SC("moge");
#endif
	return 0;
}

/**
 * メイン
 */
int
WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, INT)
{
	//ウインドウクラスの登録
	WNDCLASSEX wc = {
		sizeof wc,
		CS_CLASSDC,
		WndProc, 0, 0,
		GetModuleHandle(NULL), 0, 0, 0, 0,
		_T("WindowClassName"), 0
		};
	if (FAILED(RegisterClassEx(&wc))) {
		return -1;
	}

	// テスト表示用にコンソール割り当て
	::AllocConsole();

	// ウインドウを生成
	RECT rect = {0,0,SCWIDTH,SCHEIGHT};
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	int width  = rect.right-rect.left;
	int height = rect.bottom-rect.top;
	HWND hWnd = CreateWindow(wc.lpszClassName,
							 _T("TEST PROGRAM"),
							 WS_OVERLAPPEDWINDOW,
							 CW_USEDEFAULT, CW_USEDEFAULT, width, height,
							 NULL, NULL, wc.hInstance, NULL);
	int ret = 0;
	if (hWnd) {
		
		screen = new Screen();
		if (screen->init(hWnd)) {
			
			// squirrel 初期化
			HSQUIRRELVM v = sqobject::init();
			
			// print の登録
			sq_setprintfunc(v, PrintFunc);

			{
				test(v);
				sqobject::ObjectInfo root = sqobject::ObjectInfo::getRoot();
				root[_SC("testfunc")] = testfunc;
				root[_SC("throwfunc")] = throwfunc;
			}

			// squirrel 組み込みクラス登録
			sqobject::Object::registerClass();
			sqobject::Thread::registerClass();
			sqobject::Thread::registerGlobal();

			// XXX 自前のクラス登録はここに追加
			GameRect::registerClass();
			GamePad::registerClass();
			GameTest::registerClass();
			
			// squirrel スレッド処理初期化
			sqobject::Thread::init();
			
			// 起動スクリプトを実行
			sqobject::Thread::fork(INITFILE);
			
			// 画面表示開始
			ShowWindow(hWnd,SW_SHOWDEFAULT);
			UpdateWindow(hWnd);
			
			// メインループ
			DWORD prevTick = ::GetTickCount();
			bool working = true;
			while (working) {
				MSG msg;
				if (PeekMessage(&msg,0,0,0,PM_REMOVE)) {
					// ウインドウメッセージの処理
					TranslateMessage(&msg);
					DispatchMessage(&msg);
					if (msg.message == WM_QUIT) {
						working = false;
					}
				} else {

					// 時間更新
					DWORD tick = ::GetTickCount();
					int diff = tick - prevTick;
					prevTick = tick;

					// パッド更新
					GamePad::update();
					
					// オブジェクト状態更新
					GameObject::updateTick(diff);
					GameObject::updateBefore();
					
					// squirrel スレッド更新
					sqobject::Thread::update(diff);
					if (sqobject::Thread::main() == 0) {
						//終了処理排除
						//working = false;
					}

					// オブジェクト状態更新
					GameObject::updateAfter();
					
					// 画面更新
					screen->render();
				}
			}
			
			// squirrel スレッド処理終了
			sqobject::Thread::done();
			// squirrel 終了
			sqobject::done();

			delete screen;
			screen = NULL;
		} else {
			DestroyWindow(hWnd);
			ret = -1;
		}
	} else {
		ret = -1;
	}

	::FreeConsole();
	// ウインドウクラス解放
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	
	return ret;
}
