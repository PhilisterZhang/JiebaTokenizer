/*
 * =====================================================================================
 * 
 *       Filename:  dict_trie.h 
 *    Description:  
 * 
 *        Created:  2022/01/26 18:22:29
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_DICT_TRIE_H
#define TEXT_ANALYSIS_DICT_TRIE_H

#include <iostream>
#include <fstream>
#include <cmath>

#include "unicode.h"
#include "trie.h"

namespace text_analysis {

// cppjieba const
const double MIN_DOUBLE = -3.14e+100;
const double MAX_DOUBLE = 3.14e+100;

class DictTrie {

public:
    // 暂不支持用户自定义词典
    DictTrie() {
    }
    ~DictTrie() {
        delete trie_;
        trie_ = NULL;
    }

    DictTrie(const std::string& dict_path);

    bool init(const std::string& dict_path);
    bool initStopWords(const std::string& stop_words_path);

    const DictUnit* find(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end) const {
        return trie_->find(begin, end);
    }

    void find(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end, 
            std::vector<struct Dag>&res,
            size_t max_word_len = MAX_WORD_LENGTH) const {
        trie_->find(begin, end, res, max_word_len);
    }

    bool find(std::string& word) const {
        const DictUnit *tmp = NULL;
        RuneStringArray runes;
        if (!decodeRunesInString(word, runes)) {
            // add log
            return false;
        }
        tmp = find(runes.begin(), runes.end());
        if (tmp == NULL) {
            return false;
        } else {
            return true;
        }
    }

    double getMinWeight() const {
        return min_weight_;
    }
private:
    void createTrie(const std::vector<DictUnit>& dictUnits);
    // word freq
    bool makeNodeInfo(DictUnit& node_info,
            const std::string& word,
            double weight);
    bool makeNodeInfo(DictUnit& node_info,
            const std::vector<std::string>& runes,
            double weight);

    bool loadDict(const std::string& filePath);
    bool loadStopWordsDict(const std::string& filePath);

    void setDefaultWordWeights();

    static bool weightCompare(const DictUnit& lhs, const DictUnit& rhs) {
        return lhs.weight < rhs.weight;
    }
    
    // 计算freq, 考虑后续支持jieba等中文/泰文分词
    double calcFreqSum(const std::vector<DictUnit>& node_infos) const {
        double sum = 0.0;
        for (size_t i = 0; i < node_infos.size(); i++) {
            sum += node_infos[i].weight;
        }
        return sum;
    }
    // 预留weights
    void calculateWeight(std::vector<DictUnit>& node_infos, double sum) const {
        // assert(sum > 0.0);
        for (size_t i = 0; i < node_infos.size(); i++) {
            DictUnit& node_info = node_infos[i];
            // assert(node_info.weight > 0.0);
            node_info.weight = log(double(node_info.weight)/sum);
        }
    }
    
    void shrink(std::vector<DictUnit>& units) const {
        std::vector<DictUnit>(units.begin(), units.end()).swap(units);
    }
private:
    std::vector<DictUnit> node_infos_;
    Trie * trie_ = NULL;
    double freq_sum_ = 0.0;
    double min_weight_ = 0.0;
    double max_weight_ = 0.0;
};

}



#endif  // TEXT_ANALYSIS_DICT_TRIE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
