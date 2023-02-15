 /**
 * @file mm_segment.cpp
 * @author philister.zhang
 * @date 2022/01/14 15:02:03
 * @brief 
 *  
 **/
#include <memory>

#include "separator_iterator.h"
#include "nlp_stringutil.h"
#include "mm_segment.h"

namespace text_analysis {

extern std::unordered_map<std::string, std::unique_ptr<DictTrie> > g_dict_tries;

MMSegment::MMSegment(const Normalizer* normalizer) {
    normalizer_ = normalizer;
}

MMSegment::~MMSegment() {
}

// 不作操作
void MMSegment::cut(const std::string& text, std::vector<std::string>& res) const {
    res.clear();
    res.push_back(text);
}

// default cut: normalizer + bmm
void MMSegment::cut(const std::string& text, const std::string& country, std::vector<std::string>& res) const {
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
        cut(lower_text, country, word_ranges, words, MAX_WORD_LENGTH, BMM);
    } else {
        cut(lower_text, country, words, MAX_WORD_LENGTH, BMM);
    }
    getStringsFromWords(words, res);
}

void MMSegment::cut(const std::string& text,
        const std::string& country,
        std::vector<std::string>& res,
        size_t max_word_len,
        MMType seg_mode) const {
    std::vector<Word> words;
    cut(text, country, words, max_word_len, seg_mode);
    // words(uint32) => string(char*)
    getStringsFromWords(words, res);
}

// 默认segment
void MMSegment::cut(const std::string& text,
        const std::string& country,
        std::vector<Word>& words, 
        size_t max_word_len,
        MMType seg_mode) const {
    // 默认这里已经做过normalize
    // 直接切分即可
    SeparatorIter siter(symbols_, text);
    WordRange range;
    std::vector<WordRange> word_ranges;
    word_ranges.reserve(text.size());
    // 先分句 再分词, 不考虑合并的问题
    while (siter.hasNext()) {
        range = siter.next();
        cut(country, range.left, range.right+1, word_ranges, max_word_len, seg_mode);
    }
    words.clear();
    words.reserve(word_ranges.size());
    getWordsFromWordRanges(text, word_ranges, words);
}

// word_ranges, 在原数据上切分减少开销
void MMSegment::cut(const std::string& text,
        const std::string& country,
        const std::vector<WordRange>& word_ranges, 
        std::vector<Word>& words, 
        size_t max_word_len,
        MMType seg_mode) const {
    // 再次切分, 先分块再分词
    std::vector<WordRange> new_word_ranges;
    for (std::vector<WordRange>::const_iterator it = word_ranges.begin();
            it != word_ranges.end(); it++) {
        // id地区 非ascii 不处理
        if (country == "id" && !it->isAllAscii()) {
            new_word_ranges.push_back(*it);
            continue;
        }
        // default模式下, 数字不处理
        if (it->isALLUnicodeDigit()) {
            new_word_ranges.push_back(*it);
            continue;
        }
        cut(country, it->left, it->right+1, new_word_ranges, max_word_len, seg_mode);
    }
    words.clear();
    words.reserve(new_word_ranges.size());
    getWordsFromWordRanges(text, new_word_ranges, words);
}

void MMSegment::cut(const std::string& country,
        RuneStringArray::const_iterator begin,
        RuneStringArray::const_iterator end,
        std::vector<WordRange>& word_ranges,
        size_t max_word_len,
        MMType seg_mode) const {
    // 获取当前输入text的DAG
    std::vector<Dag> dags;
    const DictTrie* dict_trie = g_dict_tries.at(country).get();
    dict_trie->find(begin, end, dags, max_word_len);
    bool need_seg = true;
    switch (seg_mode) {
        case MM:
            need_seg = calcMM(dags);
            break;
        case RMM:
            need_seg = calcRMM(dags);
            break;
        case BMM:
            need_seg = calcBMM(dags);
            break;
        default:
            need_seg = calcMM(dags);
            break;
    }
    cutByDag(begin, end, dags, word_ranges, need_seg);
}

const DictTrie* MMSegment::getDictTrie(const std::string& country) const {
    if (g_dict_tries.find(country) == g_dict_tries.end()) {
        return NULL;
    }
    return g_dict_tries.at(country).get();
}

bool MMSegment::calcMM(std::vector<Dag>& dags) const {
    // route[idx] = DAG[idx][-1]
    for (std::vector<Dag>::iterator it = dags.begin(); it != dags.end(); it++) {
        // basic MM 暂时不需要weight 规则
        it->p_info = NULL;
        // 不生效
        it->weight = 0.0; 
        // assert(!it->nexts.empty());
        if (it->nexts.empty()) {
            return false;
        }
        // 直接取最后一个item
        it->next_pos = it->nexts.back().first;
        it->p_info = it->nexts.back().second;
    }
    return true;
}

// TODO(philister): 循环多次, 考虑优化
bool MMSegment::calcRMM(std::vector<Dag>& dags) const {
    // [(0, [0, 1, 2, 3, 4]), (1, NULL), (4, [4, 5, 6, 7, 8, 9]), (9, [9])]
    size_t length = dags.size();
    size_t i = length - 1;
    for (std::vector<Dag>::reverse_iterator rit = dags.rbegin(); rit != dags.rend(); rit++) {
        i = dags.rend() - rit - 1;
        // 置空
        rit->p_info = NULL;
        rit->weight = 0.0;
        // assert(!rit->nexts.empty());
        if (rit->nexts.empty()) {
            return false;
        }
        // 0-4 1-0 4-9 5-7 6-9 8-8 9-9
        rit->next_pos = rit->nexts.back().first;
        rit->p_info = rit->nexts.back().second;
        // 指向自己不用调整
        if (rit == dags.rbegin() || rit->next_pos == i) {
            continue;
        }
        
        // 向后遍历
        // [length-1, i)
        for (size_t j = length-1; j > i; --j) {
            // std::cout << "j: " << j << "-" << dags[j].next_pos << std::endl;
            if (dags[i].next_pos >= j && dags[i].next_pos < dags[j].next_pos) {
                // 0-(3,3) 1-(1, null) 4-(9, 9) 9-(9,9)
                for (std::vector<std::pair<size_t, const DictUnit*> >::const_reverse_iterator nrit = dags[i].nexts.rbegin();
                        nrit != dags[i].nexts.rend(); nrit++) {
                    // 合法的词典一定会找到该index
                    if (nrit->first < j) {
                        dags[i].next_pos = nrit->first;
                        dags[i].p_info = nrit->second;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

bool MMSegment::calcBMM(std::vector<Dag>& dags) const {
    calcMM(dags);
    // 记录mm index/pos_info
    std::vector<size_t> pos_index;
    size_t item_num = dags.size();
    for (const auto& dag : dags) {
        pos_index.push_back(dag.next_pos);
    }

    // 继续计算RMM
    calcRMM(dags);
    if (dags.size() != item_num) {
        return false;
    }
    size_t i = 0;
    while (i < item_num) {
        if (dags[i].next_pos != pos_index[i]) { 
            return false;
        }
        i = pos_index[i] + 1;
    }
    return true;
}

// TODO(philister): 取消cppjieba中的wordrange逻辑, 直接遍历
void MMSegment::cutByDag(RuneStringArray::const_iterator begin, 
        RuneStringArray::const_iterator end,
        const std::vector<Dag>& dags, 
        std::vector<WordRange>& words,
        bool need_seg) const {

    if (!need_seg) {
        WordRange wr(begin, end-1);
        words.push_back(wr);
        return;
    }
    size_t i = 0;
    while (i < dags.size()) {
        const DictUnit* p_word = dags[i].p_info;
        if (p_word != NULL) {
            // assert(p_word->word.size() >= 1);
            WordRange wr(begin + i, begin + i + p_word->word.size() - 1);
            words.push_back(wr);
            i += p_word->word.size();
        } else {
             // single word 未登录词单独切分
             WordRange wr(begin + i, begin + i);
             words.push_back(wr);
            i++;
        }
    }
}

}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
