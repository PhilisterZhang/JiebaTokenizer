/*
 * =====================================================================================
 * 
 *       Filename:  unicode.h 
 *    Description:  
 * 
 *        Created:  2022/01/25 16:39:13
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_UNICODE_H
#define TEXT_ANALYSIS_UNICODE_H

#include <stdint.h>
#include <stdlib.h>
#include <ostream>
#include <string>
#include <vector>

namespace text_analysis {

// ucs-2 16位即可,但是ucs-4需要32, 这里先用更大的
// 如果有内存限制再考虑16位
typedef uint32_t Rune;  // 卢恩符文
typedef std::vector<uint32_t> Unicode;

// 对外暴露为string的unicode单词
struct Word {
    std::string word;
    uint32_t offset;  // 原始输入数据中的字节位置
    uint32_t unicode_offset;  // 原始输入数据中的unicode位置
    uint32_t unicode_length;

    Word(const std::string& w, uint32_t o) : word(w), offset(o) {
    }
    Word(const std::string& w, uint32_t o, uint32_t unicode_offset, uint32_t unicode_length)
          : word(w), offset(o), unicode_offset(unicode_offset), unicode_length(unicode_length) {
    }
}; // struct Word

inline std::ostream& operator << (std::ostream& os, const Word& w) {
    return os << "{\"word\": \"" << w.word << "\", \"offset\": " << w.offset << "}";
}

// string中一个unicode字符的基本信息
struct RuneString {
    Rune rune;
    uint32_t offset;
    uint32_t len;
    uint32_t unicode_offset;
    uint32_t unicode_length; // 暂时没用, 默认为1

    RuneString(): rune(0), offset(0), len(0), unicode_offset(0), unicode_length(0) {
    }
    RuneString(Rune r, uint32_t o, uint32_t l) 
        : rune(r), offset(o), len(l), unicode_offset(0), unicode_length(0) {
    }
    RuneString(Rune r, uint32_t o, uint32_t l, uint32_t unicode_offset, uint32_t unicode_length)
        : rune(r), offset(o), len(l), unicode_offset(unicode_offset), unicode_length(unicode_length) {
    }
}; // struct RuneStr

inline std::ostream& operator << (std::ostream& os, const RuneString& r) {
    return os << "{\"rune\": \"" << r.rune << "\", \"offset\": " << r.offset << ", \"len\": " << r.len << "}";
}

typedef std::vector<struct RuneString> RuneStringArray;

// 减少消耗
/**********
struct RuneStringLite {
    uint32_t rune;
    uint32_t len;
    RuneStringLite(): rune(0), len(0) {
    }
    RuneStringLite(uint32_t r, uint32_t l): rune(r), len(l) {
    }
}; // struct RuneStrLite
***********/

// 只记录const iter, 防止string复制等操作
struct WordRange {
    RuneStringArray::const_iterator left;
    RuneStringArray::const_iterator right;

    WordRange() : left(), right() {
    }
    WordRange(RuneStringArray::const_iterator l, RuneStringArray::const_iterator r)
        : left(l), right(r) {
    }
    size_t length() const {
        return right - left + 1; 
    }
    bool isAllAscii() const {
        for (RuneStringArray::const_iterator iter = left; iter <= right; ++iter) {
            if (iter->rune >= 0x80) {
                return false;
            }
        }
        return true;
    }
    bool isALLUnicodeDigit() const {
        for (RuneStringArray::const_iterator iter = left; iter <= right; ++iter) {
            if (iter->rune <  0x30 ||  iter->rune > 0x39) {
                return false;
            }
        }
        return true;
    }
}; // struct WordRange

// 获取一个unicode字符的编码值以及length(字节长度)
inline RuneString decodeRuneFromUtf8(const char* str, size_t len) {
    // 不考虑更细的unicode标准, 如RFC 3629
    // 不考虑5,6字节的非unicode标准数据
    RuneString rp(0, 0, 0);
    if (str == NULL || len == 0) {
        return rp;
    }
    if (!(str[0] & 0x80)) { // 0xxxxxxx
        // 7bit, total 7bit
        rp.rune = (uint8_t)(str[0]) & 0x7f;
        rp.len = 1;
    } else if ((uint8_t)str[0] <= 0xdf &&  1 < len) { 
        // 110xxxxxx
        // 5bit, total 5bit
        rp.rune = (uint8_t)(str[0]) & 0x1f;

        // 6bit, total 11bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[1]) & 0x3f;
        rp.len = 2;
    } else if((uint8_t)str[0] <= 0xef && 2 < len) {
        // 1110xxxxxx
        // 4bit, total 4bit
        rp.rune = (uint8_t)(str[0]) & 0x0f;

        // 6bit, total 10bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[1]) & 0x3f;

        // 6bit, total 16bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[2]) & 0x3f;

        rp.len = 3;
    } else if((uint8_t)str[0] <= 0xf7 && 3 < len) {
        // 11110xxxx
        // 3bit, total 3bit
        rp.rune = (uint8_t)(str[0]) & 0x07;

        // 6bit, total 9bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[1]) & 0x3f;

        // 6bit, total 15bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[2]) & 0x3f;

        // 6bit, total 21bit
        rp.rune <<= 6;
        rp.rune |= (uint8_t)(str[3]) & 0x3f;

        rp.len = 4;
    } else {
        // 非正规unicode
        rp.rune = 0;
        rp.len = 0;
    }
    return rp;
}

inline bool isSingleWord(const std::string& str) {
    RuneString rp = decodeRuneFromUtf8(str.c_str(), str.size());
    return rp.len == str.size();
}

inline bool isUnicodeDigit(const Rune rune) {
    // 0-9
    if (rune >= 48 && rune <= 57) {
        return true;
    }
    // TODO(philister): 补充其他符号数字
    return false;
}

// trans string => unicode
bool decodeRunesInString(const char* s, size_t len, std::vector<RuneString>& runes);
bool decodeRunesInString(const std::string& s, RuneStringArray& runes);
bool decodeRunesInString(const char* s, size_t len, Unicode& unicode);
bool decodeRunesInString(const std::string& s, Unicode& unicode);
Unicode decodeRunesInString(const std::string& s);

Word getWordFromRunes(const std::string& s, RuneStringArray);
std::vector<Word> getWordsFromWordRanges(const std::string& s);
void getWordsFromWordRanges(const std::string& s,
        const std::vector<WordRange>& word_ranges,
        std::vector<Word>& words);
void getStringsFromWords(const std::vector<Word>& words, std::vector<std::string>& strs);

}

#endif  // TEXT_ANALYSIS_UNICODE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
