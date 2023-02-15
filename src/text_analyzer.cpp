/*
 * =====================================================================================
 * 
 *       Filename:  text_analyzer.cpp 
 *    Description:  
 * 
 *        Created:  2022/01/28 10:37:37
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#include "make_unique.h"
#include "nlp_stringutil.h"
#include "text_analyzer.h"

namespace text_analysis {

std::unordered_map<std::string, std::unique_ptr<DictTrie>> g_dict_tries;

TextAnalyzer::~TextAnalyzer() {
}

bool TextAnalyzer::addDict(const std::string& country, const std::string& dict_path) {
    std::unique_ptr<DictTrie> dict_trie = std::make_unique<DictTrie>();
    if (!dict_trie->init(dict_path)) {
        return false;
    }
    g_dict_tries.insert(std::make_pair(country, std::move(dict_trie)));
    return true;
}

bool TextAnalyzer::addStopWordsDict(const std::string& stop_words_path) {
    stop_trie_ = std::make_unique<DictTrie>(); 
    if (!stop_trie_->initStopWords(stop_words_path)) {
        return false;
    }
    return true;
}

// 暂时不区分地区stop_words
void TextAnalyzer::init() {
    // Normalizer
    normalizer_ = std::make_unique<Normalizer>(stop_trie_.get());
    // 基于最大匹配的再分词
    mm_seg_ = std::make_unique<MMSegment>(normalizer_.get());
    // 基于动态规划的再分词
    mp_seg_ = std::make_unique<MPSegment>(normalizer_.get());
}

bool TextAnalyzer::needCut(const std::string& country) const {
    if (g_dict_tries.find(country) != g_dict_tries.end()) {
        return true;
    }
    return false;
}

// 对外提供归一化功能(小写, 去除emoji以及标点)
bool TextAnalyzer::normalize(const std::string& sentence, std::vector<std::string>& res) const {
    return normalizer_->normalize(sentence, res);
}

// 默认空格切分的字符串
std::string TextAnalyzer::normalize(const std::string& sentence) const {
    return normalizer_->normalize(sentence);
}

void TextAnalyzer::cut(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const  {
    mm_seg_->cut(sentence, country, res);
}

void TextAnalyzer::cutMP(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const {
    mp_seg_->cut(sentence, country, res);
}

}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
