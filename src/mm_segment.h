 /**
 * @file mm_segment.h
 * @author philister.zhang
 * @date 2022/01/14 15:01:58
 * @brief 
 *  
 **/
#ifndef TEXT_ANALYSIS_MM_SEGMENT_H
#define TEXT_ANALYSIS_MM_SEGMENT_H

#include "segment_base.h"
#include "dict_trie.h"
#include "normalizer.h"

namespace text_analysis {

class MMSegment : SegmentBase {
public:
    MMSegment(const Normalizer* normalizer);
    ~MMSegment();

    void cut(const std::string& text, std::vector<std::string>& res) const;
    void cut(const std::string& text, const std::string& country, std::vector<std::string>& res) const;

    // default 分词, 不做归一化
    // void cutMM(const std::string& text, const std::string& country, std::vector<std::string>& res) const;
    // void cutRMM(const std::string& text, const std::string& country, std::vector<std::string>& res) const;
    // void cutBMM(const std::string& text, const std::string& country, std::vector<std::string>& res) const;
private:
    enum MMType{
        MM,  // 最大正向
        RMM, // 最大逆向
        BMM  // 最大双向
    };
    void cut(const std::string& text,
            const std::string& country,
            std::vector<std::string>& res,
            size_t max_word_len,
            MMType seg_type) const;

    void cut(const std::string& text,
            const std::string& country,
            std::vector<Word>& words,
            size_t max_word_len,
            MMType seg_type) const;

    void cut(const std::string& text,
            const std::string& country,
            const std::vector<WordRange>& word_ranges, 
            std::vector<Word>& words, 
            size_t max_word_len,
            MMType seg_mode) const;

    // 传入const_iter防止复制,修改等操作
    void cut(const std::string& country,
            RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end,
            std::vector<WordRange>& words,
            size_t max_word_len,
            MMType seg_type) const;

    const DictTrie* getDictTrie(const std::string& country) const; 
    // 核心功能
    void cutByDag(RuneStringArray::const_iterator begin, 
        RuneStringArray::const_iterator end, 
        const std::vector<Dag>& dags, 
        std::vector<WordRange>& words,
        bool need_seg = false) const;
    // MM
    bool calcMM(std::vector<Dag>& dags) const;
    // RMM
    bool calcRMM(std::vector<Dag>& dags) const;
    // BMM
    bool calcBMM(std::vector<Dag>& dags) const;

private:
    // 标准化
    const Normalizer* normalizer_ = NULL;
};

}


#endif  // US_WORD_SEGMENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
