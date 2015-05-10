#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 GetStringHashValue関数
 文字列のハッシュ値を取得する
 引数
 key	: ハッシュ値を取得する文字列
 返り値
    ハッシュ値
*/
extern unsigned int GetStringHashValue(const char* key);

/*
 BinarySearch関数
 二分探索を実行する
 引数
 search_array	: 探索される配列
 target			: 探索を行うデータ
 block_size		: 配列の1つ分の要素のバイト数
 num_blocks		: 配列のサイズ
 compare_func	: データ比較用の関数
                    (同値:0 大なり:1 小なり:-1を返す)
 user_data		: compare_funcの第3引数に入るユーザーデータ
 返り値
    発見した要素のID (見つからない場合は最も近い左の負数)
*/
extern int BinarySearch(
    void* search_array,
    void* target,
    int block_size,
    int num_blocks,
    int(*compare_func)(void*, void*, void*),
    void* user_data
);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H

