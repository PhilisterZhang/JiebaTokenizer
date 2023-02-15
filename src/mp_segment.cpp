/*
 * =====================================================================================
 * 
 *       Filename:  mp_segment.cpp 
 *    Description:  
 * 
 *        Created:  2022/01/27 12:11:10
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#include <iostream>
#include <memory>

#include "separator_iterator.h"
#include "nlp_stringutil.h"
#include "mp_segment.h"

namespace text_analysis {

extern std::unordered_map<std::string, std::unique_ptr<DictTrie> > g_dict_tries;

MPSegment::MPSegment(const Normalizer* normalizer) {
    normalizer_ = normalizer;
}

MPSegment::~MPSegment() {
}

void MPSegment::cut(const std::string& text, std::vector<std::string>& res) const {
    res.clear();
    res.push_back(text);
}

void MPSegment::cut(const std::string& text,
        const std::string& country,
        std::vector<std::string>& res) const {
    // lower
    std::string lower_text(text);
    StringUtil::toLowerCase(lower_text);
    // 默认分词, 保持一致
    if (g_dict_tries.find(country) == g_dict_tries.end()) {
        cut(lower_text, res);
        return;
    }
    res.clear();
    std::vector<WordRange> word_ranges;
    std::vector<Word> words;
    RuneStringArray runes;
    if (!decodeRunesInString(lower_text, runes)) {
        // TODO(philister): add log
        cut(lower_text, res);
        return;
    }
    if (runes.empty()) {
        return;
    }
    if (normalizer_ != NULL) {
        normalizer_->normalize(runes, word_ranges);
        cut(lower_text, country, word_ranges, words, MAX_WORD_LENGTH);
    } else {
        cut(lower_text, country, words, MAX_WORD_LENGTH);
    }
    getStringsFromWords(words, res);
}

void MPSegment::cut(const std::string& text,
        const std::string& country,
        std::vector<std::string>& words,
        size_t max_word_len) const {
    std::vector<Word> tmp;
    cut(text, country, tmp, max_word_len);
    getStringsFromWords(tmp, words);
}

void MPSegment::cut(const std::string& text,
        const std::string& country,
        std::vector<Word>& words, 
        size_t max_word_len) const {
    // 这里需要单例或者static方法优化吗?
    SeparatorIter siter(symbols_, text);
    WordRange range;
    std::vector<WordRange> word_ranges;
    word_ranges.reserve(text.size());
    // 先分句(trunk) 再分词, 不考虑合并的问题
    while (siter.hasNext()) {
        range = siter.next();
        cut(country, range.left, range.right+1, word_ranges, max_word_len);
    }
    words.clear();
    words.reserve(word_ranges.size());
    getWordsFromWordRanges(text, word_ranges, words);
}

// word_ranges, 在原数据上切分减少开销
void MPSegment::cut(const std::string& text,
        const std::string& country,
        const std::vector<WordRange>& word_ranges, 
        std::vector<Word>& words, 
        size_t max_word_len) const {
    std::vector<WordRange> new_word_ranges;
    for (std::vector<WordRange>::const_iterator it = word_ranges.begin();
            it != word_ranges.end(); it++) {
        // default模式下, 数字不处理
        if (it->isALLUnicodeDigit()) {
            new_word_ranges.push_back(*it);
            continue;
        }
        // 非ascii 不处理
        /*****************
        if (!it->isAllAscii()) {
            new_word_ranges.push_back(*it);
            continue;
        }
        // 太短的不处理
        if (it->length() <= limit_length_) {
            new_word_ranges.push_back(*it);
            continue;
        }
        ********************/
        cut(country, it->left, it->right+1, new_word_ranges, max_word_len);
    }
    words.clear();
    words.reserve(new_word_ranges.size());
    getWordsFromWordRanges(text, new_word_ranges, words);
}

void MPSegment::cut(const std::string& country,
        RuneStringArray::const_iterator begin,
        RuneStringArray::const_iterator end,
        std::vector<WordRange>& words,
        size_t max_word_len) const {
    // 获取当前输入text的DAG
    std::vector<Dag> dags;
    const DictTrie* dict_trie = g_dict_tries.at(country).get();
    dict_trie->find(begin, end, dags, max_word_len);
    calcDP(country, dags);
    cutByDag(begin, end, dags, words);
}

const DictTrie* MPSegment::getDictTrie(const std::string& country) const {
    if (g_dict_tries.find(country) == g_dict_tries.end()) {
        return NULL;
    }
    return g_dict_tries.at(country).get();
}

// jieba动态规划计算路径
void MPSegment::calcDP(const std::string& country, std::vector<Dag>& dags) const {
    size_t next_pos = 0;
    const DictUnit* p;
    double val = 0.0;
    const DictTrie* dict_trie = g_dict_tries.at(country).get();

    // route[idx] = max((log(self.FREQ.get(sentence[idx:x + 1]) or 1) - logtotal + route[x + 1][0], x) for x in DAG[idx])
    for (std::vector<Dag>::reverse_iterator rit = dags.rbegin(); rit != dags.rend(); rit++) {
        // 默认为空, 不存在route
        rit->p_info = NULL;
        rit->weight = MIN_DOUBLE;
        // assert(!rit->nexts.empty());
        // 从后向前, DP 计算最大路径
        // p_info 记录了当前节点weights最大的route节点
        for (std::vector<std::pair<size_t, const DictUnit*> >::const_iterator it = rit->nexts.begin();
                it != rit->nexts.end(); it++) {
            next_pos = it->first;
            p = it->second;
            val = 0.0;
            if (next_pos + 1 < dags.size()) {
                val += dags[next_pos + 1].weight;
            }

            if (p) {
                val += p->weight;
            } else {
                val += dict_trie->getMinWeight();
            }
            // 取最大log(freq)
            if (val > rit->weight) {
                rit->p_info = p;
                rit->weight = val;
            }
        }
    }
}

void MPSegment::cutByDag(RuneStringArray::const_iterator begin, 
        RuneStringArray::const_iterator end,
        const std::vector<Dag>& dags, 
        std::vector<WordRange>& words) const {
    size_t i = 0;
    while (i < dags.size()) {
        const DictUnit* p = dags[i].p_info;
        if (p) {
            // assert(p->word.size() >= 1);
            WordRange wr(begin + i, begin + i + p->word.size() - 1);
            words.push_back(wr);
            i += p->word.size();
        } else { // single word
            WordRange wr(begin + i, begin + i);
            words.push_back(wr);
            i++;
      }
    }
}

}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
