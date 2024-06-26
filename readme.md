# squirrel wamsoft custom

Author: 渡邊剛(go@wamsoft.jp)
Date: 2024/3/19

## 解説

組み込み言語 squirrel (http://squirrel-lang.org/) のカスタム版です。
実際にゲームエンジンなどに組み込む際に調整した部分などをまとめて
あります。

以下のコードも内包します

sqrat  C++用のsquirrelバインドツール
 http://sourceforge.net/projects/scrat/
 ※改造されています

## MANIFEST

    include/	squirrelヘッダファイル
    squirrel/	squirrel本体ソース
    sqstdlib/	squirrel標準ライブラリソース
    
    sq/			sq コマンド
    sqrat/		sqrat 機能追加版
    sqobject/	Object/Threadライブラリ
    
    機能追加.txt	squirrelの仕様変更/追加機能の解説
    読んでね.txt	このファイル
    
    CMakeLists.txt  ビルド用
    CMakePresets.json プリセット定義

●コンパイル

cmake でビルドできます

実行例(preset利用)
    cmake --preset x86-windows
    cmake --build build/x86-windows
    cmake --install build/x86-windows --prefix install



●ライセンス

zlibライセンスの squirrel 2.2 系からの fork になります

squirrel/sqrat 同様 に
zlibライセンスに従って利用してください。

copyright (c)2024 Go Watanabe go@wamsoft.jp
zlib license
