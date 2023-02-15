/*
 * =====================================================================================
 * 
 *       Filename:  normalizer.h 
 *    Description:  
 * 
 *        Created:  2022/01/28 11:17:30
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_NORMALIZER_H
#define TEXT_ANALYSIS_NORMALIZER_H

#include "dict_trie.h"

namespace text_analysis {

static const std::string WHITESPACE(" ");

class Normalizer {
public:
    Normalizer(const std::string& stop_words_path);
    Normalizer(const DictTrie* stop_trie);
    ~Normalizer();

    bool normalize(const std::string& text, std::vector<std::string>& res) const;
    std::string normalize(const std::string& text) const;
    bool normalize(const std::string& text, std::vector<Word>& words) const;
    void normalize(const RuneStringArray&runes, std::vector<WordRange>& word_ranges) const;
private:
    // 数字处理, 考虑优化
    void numberSplit(std::vector<WordRange>& word_ranges) const; 
    // 停用词处理
    void removeStopWords(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end,
            std::vector<WordRange>& word_ranges) const; 

    bool callStopWords(std::vector<Dag>& dags) const;
    void removeByDag(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end,
            const std::vector<Dag>& dags,
            std::vector<WordRange>& words,
            bool need_remove) const;
private:
    // 停用词词典
    const DictTrie* stop_trie_ = NULL;
    bool need_destroy_ = false;
};

}


#endif  // TEXT_ANALYSIS_NORMALIZER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
