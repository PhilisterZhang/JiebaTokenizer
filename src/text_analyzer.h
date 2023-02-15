/*
 * =====================================================================================
 * 
 *       Filename:  text_analyzer.h 
 *    Description:  nlp 基础文本分析工具 
 * 
 *        Created:  2022/01/28 10:37:31
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_TEXT_ANALYZER_H
#define TEXT_ANALYSIS_TEXT_ANALYZER_H

#include <memory>

#include "normalizer.h"
#include "mm_segment.h"
#include "mp_segment.h"

namespace text_analysis {
class TextAnalyzer {

public:
    TextAnalyzer() {}
    ~TextAnalyzer();

    // 初始化
    bool addDict(const std::string& country, const std::string& dict_path);
    bool addStopWordsDict(const std::string& stop_words_path); 
    void init();
    void destroy() {
    }

    bool needCut(const std::string& country) const;
    // 清洗数据
    std::string normalize(const std::string& text) const;
    bool normalize(const std::string& text, std::vector<std::string>& res) const;

    // 默认的分词方法
    // (1) 归一化
    // (2) mm分词
    void cut(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const;
    // 动态规划分词
    void cutMP(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const;
    // MM分词
    // void cutMM(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const;
    // void cutRMM(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const;
    // void cutBMM(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const;
private:
    // 停用词词典
    std::unique_ptr<DictTrie> stop_trie_;
    // normalizer
    std::unique_ptr<Normalizer> normalizer_;
    // 分词器
    std::unique_ptr<MPSegment> mp_seg_;
    std::unique_ptr<MMSegment> mm_seg_;
};

}


#endif  // TEXT_ANALYSIS_TEXT_ANALYZER_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
