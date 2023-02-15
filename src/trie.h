/*
 * =====================================================================================
 * 
 *       Filename:  trie.h 
 *    Description:  
 * 
 *        Created:  2022/01/25 20:06:31
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_TRIE_H
#define TEXT_ANALYSIS_TRIE_H

#include <unordered_map>

#include "unicode.h"

namespace text_analysis {

const size_t MAX_WORD_LENGTH = 50;

struct DictUnit {
    Unicode word;
    double weight;
    // std::string tag;  // postag
}; // struct DictUnit

struct Dag {
    RuneString runestr;
    // 前缀词典
    std::vector<std::pair<size_t, const DictUnit*> > nexts;
    // 后缀词典 (暂不需要)
    // std::vector<std::pair<size_t, const DictUnit*> > lasts;
    // 记录route的指针
    const DictUnit* p_info;
    double weight;
    size_t next_pos;
    Dag():runestr(), p_info(NULL), weight(0.0), next_pos(0) {
    }
}; // struct Dag

class TrieNode {
public:
    TrieNode(): next(NULL), p_value(NULL) {
    }
public:
    // 后续考虑优化, 这样内存占用较高
    typedef std::unordered_map<uint32_t, TrieNode*> NextMap;
    NextMap *next;
    const DictUnit* p_value;
};

class Trie {
public:
    Trie(const std::vector<Unicode>& keys, const std::vector<const DictUnit*>& valuePointers);
    ~Trie();

    // 返回全部可能的dag
    void find(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end,
            std::vector<struct Dag>& res,
            size_t max_word_len) const;

    // 基本的find
    const DictUnit* find(RuneStringArray::const_iterator begin,
            RuneStringArray::const_iterator end) const; 
private:
    // 对外不暴露构造与删除
    void createTrie(const std::vector<Unicode>& keys, const std::vector<const DictUnit*>& value_pointers);
    void insertNode(const Unicode& key, const DictUnit* p_value);
    void deleteNode(TrieNode* node);

private:
    TrieNode* root_ = NULL;
};

}

#endif  // TEXT_ANALYSIS_TRIE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
