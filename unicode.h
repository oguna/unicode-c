#ifndef UNICODE_H
#define UNICODE_H

#include <uchar.h>
#include <stdbool.h>

/***
 * 文字エンコーディングをUTF32からUTF8に変換する
 * @param out 変換した文字の出力先
 * @param in 変換する文字
 * @return 変換後のバイト数
 */
int utf32ctoutf8c(char32_t in, char* out);

/***
 * 文字エンコーディングをUTF8からUTF32に変換する
 */
int utf8ctoutf32c(const char* in, char32_t* out);

/***
 * UTF32の文字をUTF16に変換する。
 * @param u32Ch 変換する文字
 * @param u16Ch 変換された文字の格納先
 * @return 変換された文字の配列の長さ。0なら変換が行われていない。
 */
int utf32ctoutf16c(char32_t u32Ch, char16_t* u16Ch);

/***
 * UTF16の文字をUTF32に変換する
 * @param u16Ch
 * @param u32Ch
 * @return
 */
bool utf16ctoutf32c(const char16_t *u16Ch, char32_t* u32Ch);

int utf8ctoutf16c(char* u8Ch, char16_t* u16Ch);
int utf16ctoutf8c(char16_t* u16Ch, char* u8Ch);

typedef struct unicodeconv {
    int code_units;
    int utf8num;
    int utf16num;
} unicodeconv_t;

/***
 * UTF8の文字列をUTF16に変換する
 * @param u8str 変換元の文字列
 * @param u8strlen 変換元の文字列の長さ。-1ならNULL文字まで変換する。
 * @param u16str 変換先の文字列
 * @param u16strlen 変換先の文字列の長さ。-1なら制限を設けず変換する。
 * @param state 変換過程で得られた情報
 * @return dummy
 */
int utf8stoutf16s(const char* u8str, int u8strlen, char16_t* u16str, int u16strlen, unicodeconv_t* state);

int utf16stoutf8s(const char16_t* u16str, int u16strlen, char* u8str, int u8strlen, unicodeconv_t* state);

#endif //UNICODE_H
