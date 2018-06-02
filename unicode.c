#include <stdbool.h>
#include "unicode.h"

int utf32ctoutf8c(char32_t in, char *out) {
    if (in < 0 || in > 0x10FFFF) {
        return -1;
    }
    if (in < 128) {
        out[0] = (char) in;
        return 1;
    } else if (in < 2048) {
        out[0] = 0xC0 | (char) (in >> 6);
        out[1] = 0x80 | (char) (in & 0x3F);
        return 2;
    } else if (in < 65536) {
        out[0] = 0xE0 | (char) (in >> 12);
        out[1] = 0x80 | (char) ((in >> 6) & 0x3F);
        out[2] = 0x80 | (char) (in & 0x3F);
        return 3;
    } else {
        out[0] = 0xF0 | (char) (in >> 18);
        out[1] = 0x80 | (char) ((in >> 12) & 0x3F);
        out[2] = 0x80 | (char) ((in >> 6) & 0x3F);
        out[3] = 0x80 | (char) (in & 0x3F);
        return 4;
    }
}


static int GetU8ByteCount(char ch) {
    uint8_t c = (uint8_t) ch;
    if (0 <= c && c < 0x80) {
        return 1;
    }
    if (0xC2 <= c && c < 0xE0) {
        return 2;
    }
    if (0xE0 <= c && c < 0xF0) {
        return 3;
    }
    if (0xF0 <= c && c < 0xF8) {
        return 4;
    }
    return 0;
}

static bool IsU8LaterByte(char ch) {
    return 0x80 <= (uint8_t) ch && (uint8_t) ch < 0xC0;
}

int utf8ctoutf32c(const char *u8Ch, char32_t *u32Ch) {
    int numBytes = GetU8ByteCount(u8Ch[0]);
    if (numBytes == 0) {
        return false;
    }
    switch (numBytes) {
        case 1:
            *u32Ch = (char32_t) (uint8_t) u8Ch[0];
            break;
        case 2:
            if (!IsU8LaterByte(u8Ch[1])) {
                return false;
            }
            if (((uint8_t) (u8Ch[0]) & 0x1E) == 0) {
                return false;
            }

            *u32Ch = (char32_t) (u8Ch[0] & 0x1F) << 6;
            *u32Ch |= (char32_t) (u8Ch[1] & 0x3F);
            break;
        case 3:
            if (!IsU8LaterByte(u8Ch[1]) || !IsU8LaterByte(u8Ch[2])) {
                return false;
            }
            if (((uint8_t) (u8Ch[0]) & 0x0F) == 0 &&
                ((uint8_t) (u8Ch[1]) & 0x20) == 0) {
                return false;
            }

            *u32Ch = (char32_t) (u8Ch[0] & 0x0F) << 12;
            *u32Ch |= (char32_t) (u8Ch[1] & 0x3F) << 6;
            *u32Ch |= (char32_t) (u8Ch[2] & 0x3F);
            break;
        case 4:
            if (!IsU8LaterByte(u8Ch[1]) || !IsU8LaterByte(u8Ch[2]) ||
                !IsU8LaterByte(u8Ch[3])) {
                return false;
            }
            if (((uint8_t) (u8Ch[0]) & 0x07) == 0 &&
                ((uint8_t) (u8Ch[1]) & 0x30) == 0) {
                return false;
            }

            *u32Ch = (char32_t) (u8Ch[0] & 0x07) << 18;
            *u32Ch |= (char32_t) (u8Ch[1] & 0x3F) << 12;
            *u32Ch |= (char32_t) (u8Ch[2] & 0x3F) << 6;
            *u32Ch |= (char32_t) (u8Ch[3] & 0x3F);
            break;
        default:
            return false;
    }

    return numBytes;
}

int utf32ctoutf16c(char32_t u32Ch, char16_t *u16Ch) {
    if (u32Ch < 0 || u32Ch > 0x10FFFF) {
        return 0;
    }
    if (u32Ch < 0x10000) {
        u16Ch[0] = (char16_t) u32Ch;
        return 1;
    } else {
        u16Ch[0] = (char16_t) ((u32Ch - 0x10000) / 0x400 + 0xD800);
        u16Ch[1] = (char16_t) ((u32Ch - 0x10000) % 0x400 + 0xDC00);
        return 2;
    }
}

static bool IsU16HighSurrogate(char16_t ch) { return 0xD800 <= ch && ch < 0xDC00; }

static bool IsU16LowSurrogate(char16_t ch) { return 0xDC00 <= ch && ch < 0xE000; }

bool utf16ctoutf32c(const char16_t *u16Ch, char32_t *u32Ch) {
    if (IsU16HighSurrogate(u16Ch[0])) {
        if (IsU16LowSurrogate(u16Ch[1])) {
            *u32Ch = 0x10000 + ((char32_t) (u16Ch[0] - 0xD800) * 0x400 +
                                (char32_t) u16Ch[1] - 0xDC00);
        } else if (u16Ch[1] == 0) {
            *u32Ch = u16Ch[0];
        } else {
            return false;
        }
    } else if (IsU16LowSurrogate(u16Ch[0])) {
        if (u16Ch[1] == 0) {
            *u32Ch = u16Ch[0];
        } else {
            return false;
        }
    } else {
        *u32Ch = u16Ch[0];
    }

    return true;
}

int utf8ctoutf16c(char *u8Ch, char16_t *u16Ch) {
    char32_t u32Ch;
    utf8ctoutf32c(u8Ch, &u32Ch);
    utf32ctoutf16c(u32Ch, u16Ch);
    return 0;
}

int utf16ctoutf8c(char16_t *u16Ch, char *u8Ch) {
    char32_t u32Ch;
    utf16ctoutf32c(u16Ch, &u32Ch);
    utf32ctoutf8c(u32Ch, u8Ch);
    return 0;
}

int utf8stoutf16s(const char *u8str, int u8strlen, char16_t *u16str, int u16strlen, struct unicodeconv *state) {
    int units = 0;
    int u8offset = 0;
    int u16offset = 0;
    if (u8strlen == -1) {
        u8strlen = INT32_MAX;
    }
    while (u8offset < u8strlen && u16offset < u16strlen && u8str[u8offset] != '\0') {
        char32_t c32;
        int u8len = utf8ctoutf32c(u8str + u8offset, &c32);
        int u16len = utf32ctoutf16c(c32, u16str + u16offset);
        u8offset += u8len;
        u16offset += u16len;
        units++;
    }
    if (state != NULL) {
        state->code_units = units;
        state->utf8num = u8offset;
        state->utf16num = u16offset;
    }
    return 0;
}

int utf16stoutf8s(const char16_t *u16str, int u16strlen, char *u8str, int u8strlen, struct unicodeconv *state) {
    int units = 0;
    int u8offset = 0;
    int u16offset = 0;
    if (u16strlen == -1) {
        u16strlen = INT32_MAX;
    }
    while (u8offset < u8strlen && u16offset < u16strlen && u16str[u16offset] != u'\0') {
        char32_t c32;
        int u16len = utf16ctoutf32c(u16str + u16offset, &c32);
        int u8len = utf32ctoutf8c(c32, u8str + u8offset);
        u8offset += u8len;
        u16offset += u16len;
        units++;
    }
    if (state != NULL) {
        state->code_units = units;
        state->utf8num = u8offset;
        state->utf16num = u16offset;
    }
    return 0;
}

