/*
 * =====================================================================================
 * 
 *       Filename:  mp_segment.h 
 *    Description:  
 * 
 *        Created:  2022/01/27 12:11:04
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_MP_SEGMENT_H
#define TEXT_ANALYSIS_MP_SEGMENT_H

#include "segment_base.h"
#include "dict_trie.h"
#include "normalizer.h"

namespace text_analysis {

class MPSegment : SegmentBase {
public:
    // 传入trie词典
    MPSegment(const Normalizer* normalizer);
    ~MPSegment();

    void cut(const std::string& text, std::vector<std::string>& res) const;
    void cut(const std::string& text, const std::string& country, std::vector<std::string>& res) const;
private:
    const DictTrie* getDictTrie(const std::string& country) const;

    void cut(const std::string& text,
            const std::string& country,
            std::vector<std::string>& res,
            size_t max_word_len) const;

    void cut(const std::string& text,
            const std::string& country,
            std::vector<Word>& words,
            size_t max_word_len) const;
    
    void cut(const std::string& text,
            const std::string& country,
            const std::vector<WordRange>& word_ranges, 
            std::vector<Word>& words, 
            size_t max_word_len) const;

    // 传入const_iter防止复制,修改等操作
    void cut(const std::string& country,
            RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end,
            std::vector<WordRange>& words,
            size_t max_word_len) const; 
   
    void calcDP(const std::string& country, std::vector<Dag>& dags) const;
    void cutByDag(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end,
            const std::vector<Dag>& dags,
            std::vector<WordRange>& words) const;
private:
    // 标准化
    const Normalizer* normalizer_ = NULL;
};

}


#endif  // TEXT_ANALYSIS_MP_SEGMENT_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
