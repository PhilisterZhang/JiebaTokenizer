/*
 * =====================================================================================
 * 
 *       Filename:  normalizer.cpp 
 *    Description:  
 * 
 *        Created:  2022/01/28 11:17:35
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#include "nlp_stringutil.h"
#include "normalizer.h"

namespace text_analysis {

Normalizer::Normalizer(const std::string& stop_words_path) {
    stop_trie_ = new DictTrie(stop_words_path);
    need_destroy_ = true;
}

Normalizer::Normalizer(const DictTrie* stop_trie) {
    stop_trie_ = stop_trie;
    need_destroy_ = false;
}

Normalizer::~Normalizer() {
    if (need_destroy_) {
        delete stop_trie_;
        stop_trie_ = NULL;
    }
}

std::string Normalizer::normalize(const std::string& text) const {
    std::string norm_string(text);
    std::vector<std::string> strs;
    if (normalize(text, strs)) {
        norm_string = StringUtil::join(strs, WHITESPACE);
    }
    return norm_string;
}

bool Normalizer::normalize(const std::string& text, std::vector<std::string>& res) const {
    std::vector<Word> words;
    res.clear();
    if (!normalize(text, words)) {
        return false;
    }
    getStringsFromWords(words, res);
    return true;
}

bool Normalizer::normalize(const std::string& text, std::vector<Word>& words) const {
    words.clear();
    std::vector<WordRange> word_ranges;
    word_ranges.reserve(text.size());
    // lower
    std::string lower_text(text);
    StringUtil::toLowerCase(lower_text);
    RuneStringArray runes;
    if (!decodeRunesInString(lower_text, runes)) {
        return false;
    }
    if (runes.empty()) {
        return true;
    }
    // 去除标点符号语表情包
    removeStopWords(runes.begin(), runes.end(), word_ranges);
    // TODO(philister): 数字先独立出来, 后续根据需求处理各种特殊数字以及单位
    numberSplit(word_ranges);
    getWordsFromWordRanges(lower_text, word_ranges, words);
    return true;
}

void Normalizer::normalize(const RuneStringArray& runes, std::vector<WordRange>& word_ranges) const {
    // decode Rune from string
    removeStopWords(runes.begin(), runes.end(), word_ranges);
    numberSplit(word_ranges);
}

void Normalizer::numberSplit(std::vector<WordRange>& word_ranges) const {
    std::vector<WordRange> new_word_ranges;
    for (const auto& iter : word_ranges) {
        int old_str = 0, last_str= -1;
        int old_num = 0, last_num = -1;
        // 0abc4
        for (int i = 0; i <= iter.right - iter.left; ++i) {
            // 当前为数字
            if (isUnicodeDigit((iter.left+i)->rune)) {
                if(i > 0 && last_str == i-1) {
                    WordRange wr(iter.left+old_str, iter.left+last_str);
                    new_word_ranges.push_back(wr);
                    old_num = i;
                }
                last_num = i;
            } else {
                if (i > 0 && last_num == i-1) {
                    WordRange wr(iter.left+old_num, iter.left+last_num);
                    new_word_ranges.push_back(wr);
                    old_str = i;
                }
                last_str = i;
            }
        }
        // assert(iter.right - iter.left >=0)
        int old = (old_num > old_str) ? old_num : old_str;
        WordRange wr(iter.left+old, iter.right);
        new_word_ranges.push_back(wr);
    } 
    word_ranges.clear();
    word_ranges.resize(new_word_ranges.size());
    for (size_t i = 0; i < new_word_ranges.size(); ++i) {
        word_ranges[i] = new_word_ranges[i];
    }
} 

void Normalizer::removeStopWords(RuneStringArray::const_iterator begin,
        RuneStringArray::const_iterator end,
        std::vector<WordRange>& word_ranges) const {
    std::vector<Dag> dags;
    // 默认长度与分词字典一致
    bool need_remove = false;
    if (stop_trie_ != NULL) {
        stop_trie_->find(begin, end, dags, MAX_WORD_LENGTH);
        need_remove = callStopWords(dags);
    }
    removeByDag(begin, end, dags, word_ranges, need_remove);
} 

bool Normalizer::callStopWords(std::vector<Dag>& dags) const {
    for (std::vector<Dag>::iterator it = dags.begin(); it != dags.end(); it++) {
        it->p_info = NULL;
        it->weight = 0.0; 

        if (it->nexts.empty()) {
            return false;
        }
        // 直接取最后一个item
        it->next_pos = it->nexts.back().first;
        it->p_info = it->nexts.back().second;
    }
    return true;
} 

void Normalizer::removeByDag(RuneStringArray::const_iterator begin, 
        RuneStringArray::const_iterator end,
        const std::vector<Dag>& dags, 
        std::vector<WordRange>& word_ranges,
        bool need_remove) const {
    word_ranges.clear();
    if (!need_remove) {
        WordRange wr(begin, end-1);
        word_ranges.push_back(wr);
        return;
    }
    size_t i = 0;
    size_t j = 0;
    while (i < dags.size()) {
        const auto& p_word = dags[i].p_info;
        // 停用词
        if (p_word != NULL) {
            if (i != 0 and i > j) {
                WordRange wr(begin+j, begin+i-1);
                word_ranges.push_back(wr);
            }
            i += p_word->word.size();
            j = i;
        } else {
            ++i;
        }
    }
    if (j < dags.size()) {
        WordRange wr(begin+j, end-1);
        word_ranges.push_back(wr);
    }
}

}



/* vim: set ts=4 sw=4 sts=4 tw=100 */
