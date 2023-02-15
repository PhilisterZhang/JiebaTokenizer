/*
 * =====================================================================================
 * 
 *       Filename:  unicode.cpp 
 *    Description:  
 * 
 *        Created:  2022/01/25 17:07:02
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */

#include <iostream>
#include "unicode.h"

namespace text_analysis {

bool decodeRunesInString(const char* s, size_t len, RuneStringArray& runes) {
    runes.clear();
    runes.reserve(len);
    for (uint32_t i = 0, j = 0; i < len;) {
        RuneString rp = decodeRuneFromUtf8(s + i, len - i);
        if (rp.len == 0) {
            runes.clear();
            return false;
        }
        // 赋值真实offset/length
        rp.offset = i;
        rp.unicode_offset = j;
        rp.unicode_length = 1;
        runes.push_back(rp);
        i += rp.len;
        ++j;
    }
    return true;
}

bool decodeRunesInString(const std::string& s, RuneStringArray& runes) {
    return decodeRunesInString(s.c_str(), s.size(), runes);
}

bool decodeRunesInString(const char* s, size_t len, Unicode& unicode) {
    unicode.clear();
    RuneStringArray runes;
    if (!decodeRunesInString(s, len, runes)) {
        return false;
    }
    unicode.reserve(runes.size());
    for (size_t i = 0; i < runes.size(); i++) {
        unicode.push_back(runes[i].rune);
    }
    return true;
}

bool decodeRunesInString(const std::string& s, Unicode& unicode) {
    return decodeRunesInString(s.c_str(), s.size(), unicode);
}

Unicode decodeRunesInString(const std::string& s) {
    Unicode result;
    decodeRunesInString(s, result);
    return result;
}

// [left, right]
Word getWordFromRunes(const std::string& s,
        RuneStringArray::const_iterator left,
        RuneStringArray::const_iterator right) {
    // assert(right->offset >= left->offset);
    uint32_t len = right->offset - left->offset + right->len;
    uint32_t unicode_length = right->unicode_offset - left->unicode_offset + right->unicode_length;
    return Word(s.substr(left->offset, len), left->offset, left->unicode_offset, unicode_length);
}

std::string getStringFromRunes(const std::string& s,
        RuneStringArray::const_iterator left,
        RuneStringArray::const_iterator right) {
    // assert(right->offset >= left->offset);
    uint32_t len = right->offset - left->offset + right->len;
    return s.substr(left->offset, len);
}

// wordrange 存在风险, getword前不能析构
void getWordsFromWordRanges(const std::string& s,
        const std::vector<WordRange>& wrs,
        std::vector<Word>& words) {

    for (const auto& wr : wrs) {
        words.push_back(getWordFromRunes(s, wr.left, wr.right));
    }
}

std::vector<Word> getWordsFromWordRanges(const std::string& s, const std::vector<WordRange>& wrs) {
    std::vector<Word> result;
    getWordsFromWordRanges(s, wrs, result);
    return result;
}

void getStringsFromWords(const std::vector<Word>& words, std::vector<std::string>& strs) {
    strs.clear();
    strs.resize(words.size());
    for (size_t i = 0; i < words.size(); ++i) {
        strs[i] = words[i].word;
    }
}

}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
