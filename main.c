#include <stdio.h>
#include <assert.h>
#include <uchar.h>
#include <string.h>
#include "unicode.h"

int main() {
    {
        // UTF32 -> UTF8
        char32_t c32a = U'あ';
        char u8str[4] = {0};
        int size = utf32ctoutf8c(c32a, u8str);
        assert(size == 3);
        assert(memcmp(u8str, u8"あ", 3) == 0);
    }
    {
        // UTF8 -> UTF32
        char32_t c32a = 0;
        utf8ctoutf32c(u8"あ", &c32a);
        assert(c32a == u'あ');
    }
    {
        // UTF32 -> UTF16
        char32_t c32take = 0x2000B;
        char16_t c16take[] = {0xD840, 0xDC0B, 0};
        char16_t c16[3] = {0};
        utf32ctoutf16c(c32take, c16);
        assert(memcmp(c16take, c16, sizeof(c16take)) == 0);
    }

    {
        // UTF16 -> UTF32
        char32_t c32take = 0x2000B;
        char16_t c16take[] = {0xD840, 0xDC0B, 0};
        char32_t c32;
        utf16ctoutf32c(c16take, &c32);
        assert(c32 == c32take);
    }

    {
        // UTF16 -> UTF8
        char a[100];
        utf16ctoutf8c(u"あ", a);
        assert(memcmp(u8"あ", a, 3) == 0);
    }

    {
        // UTF8 -> UTF16
        char16_t a[100];
        utf8ctoutf16c(u8"あ", a);
        assert(memcmp(u"あ", a, 2) == 0);
    }

    {
        // UTF8STR -> UTF16STR
        char u8s[] = u8"こんにちは";
        char16_t u16s[100];
        struct unicodeconv state;
        utf8stoutf16s(u8s, -1, u16s, 100, &state);
        assert(memcmp(u16s, u"こんにちは", 10) == 0);
        assert(state.code_units == 5);
        assert(state.utf8num == 15);
        assert(state.utf16num == 5);
    }

    {
        // UTF16STR -> UTF8STR
        char16_t u16s[] = u"こんにちは";
        char u8s[100];
        struct unicodeconv state;
        utf16stoutf8s(u16s, -1, u8s, 100, &state);
        assert(memcmp(u8s, u8"こんにちは", 15) == 0);
        assert(state.code_units == 5);
        assert(state.utf8num == 15);
        assert(state.utf16num == 5);
    }
    printf("TEST OK!");
    return 0;
}