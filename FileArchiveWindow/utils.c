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
unsigned int GetStringHashValue(const char* key)
{
#define INITIAL_FNV (2166136261u)
#define FNV_MULTIPLE (16777619u)
    unsigned int hash_value = INITIAL_FNV;
    const char *str = key;

    while(*str != '\0')
    {
        hash_value = hash_value ^ (*str);
        hash_value *= FNV_MULTIPLE;
        str++;
    }

    return hash_value;
}

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
int BinarySearch(
    void* search_array,
    void* target,
    int block_size,
    int num_blocks,
    int(*compare_func)(void*, void*, void*),
    void* user_data
)
{
    unsigned char *bytes = (unsigned char*)search_array;
    int compare;
    int minimum = 0;
    int maximum = num_blocks - 1;
    int midium;

    while(minimum <= maximum)
    {
        midium = (minimum + maximum) / 2;
        compare = compare_func((void*)&bytes[midium*block_size], target, user_data);
        if(compare == 0)
        {
            while(midium < num_blocks-1
                && compare_func(&bytes[(midium+1)*block_size], target, user_data) == 0)
            {
                midium++;
            }
            return midium;
        }
        else if(compare < 0)
        {
            minimum = midium + 1;
        }
        else
        {
            maximum = midium - 1;
        }
    }

    if(maximum < 0)
    {
        maximum = 0;
    }
    while(maximum > 0
        && compare_func(&bytes[(maximum-1)*block_size],
        &bytes[maximum*block_size], user_data) > 0)
    {
        maximum--;
    }
    while(maximum < num_blocks
        && compare_func(&bytes[(maximum+1)*block_size],
        &bytes[maximum*block_size], user_data) < 0)
    {
        maximum++;
    }

    return - maximum;
}

#ifdef __cplusplus
}
#endif
