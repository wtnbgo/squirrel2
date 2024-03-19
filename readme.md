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

●コンパイル

cmake でビルドできます

    cmake -B build
    cmake --build build

●ライセンス

zlibライセンスの squirrel 2.2.4 からの fork になります

squirrel/sqrat 同様 に
zlibライセンスに従って利用してください。

copyright (c)2024 Go Watanabe go@wamsoft.jp
zlib license
