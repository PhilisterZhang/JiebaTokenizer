/*
 * =====================================================================================
 * 
 *       Filename:  trie.cpp 
 *    Description:  
 * 
 *        Created:  2022/01/25 20:06:36
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#include "trie.h"

namespace text_analysis {

Trie::Trie(const std::vector<Unicode>& keys,
        const std::vector<const DictUnit*>& value_pointers) {
    root_ = new TrieNode();
    createTrie(keys, value_pointers);
}

Trie::~Trie() {
    deleteNode(root_);
}

const DictUnit* Trie::find(RuneStringArray::const_iterator begin,
        RuneStringArray::const_iterator end) const {
    if (begin == end) {
        return NULL;
    }

    const TrieNode* p_node = root_;
    TrieNode::NextMap::const_iterator citer;
    for (RuneStringArray::const_iterator it = begin; it != end; it++) {
        if (NULL == p_node->next) {
            return NULL;
        }
        citer = p_node->next->find(it->rune);
        if (p_node->next->end() == citer) {
            return NULL;
        }
        p_node = citer->second;
    }
    return p_node->p_value;
}

// 遍历所有Rune(字), 从root开始查询
// 记录所有可能的路径(词表中的词)
// 不存在该前缀则将NULL记入map
// 返回所有dag
void Trie::find(RuneStringArray::const_iterator begin, 
        RuneStringArray::const_iterator end, 
        std::vector<struct Dag>&res, 
        size_t max_word_len = MAX_WORD_LENGTH) const {
    // assert(root_ != NULL);
    // 这里把所有可能的dag全部记录下来
    res.resize(end - begin);

    const TrieNode *p_node = NULL;
    TrieNode::NextMap::const_iterator citer;
    for (size_t i = 0; i < size_t(end - begin); i++) {
        res[i].runestr = *(begin + i);
        // 根节点查询
        if (root_->next != NULL && root_->next->end() != (citer = root_->next->find(res[i].runestr.rune))) {
            p_node = citer->second;
        } else {
            p_node = NULL;
        }
        // 第一级Rune(字)
        // 字本身可能也是一个词, 不存在该前缀则置为空
        if (p_node != NULL) {
            res[i].nexts.push_back(std::pair<size_t, const DictUnit*>(i, p_node->p_value));
        } else {
            res[i].nexts.push_back(std::pair<size_t, const DictUnit*>(i, static_cast<const DictUnit*>(NULL)));
        }
        // 开始查找词, 按照长度陆续添加
        // example: 0 [0, 1, 4, 7]
        for (size_t j = i + 1; j < size_t(end - begin) && (j - i + 1) <= max_word_len; j++) {
            if (p_node == NULL || p_node->next == NULL) {
                break;
            }
            citer = p_node->next->find((begin + j)->rune);
            if (p_node->next->end() == citer) {
                break;
            }
            p_node = citer->second;
            if (NULL != p_node->p_value) {
                res[i].nexts.push_back(std::pair<size_t, const DictUnit*>(j, p_node->p_value));
            }
        }
    }
}

void Trie::insertNode(const Unicode& key, const DictUnit* p_value) {
    if (key.begin() == key.end()) {
        return;
    }

    TrieNode::NextMap::const_iterator km_iter;
    TrieNode *p_node = root_;
    for (Unicode::const_iterator citer = key.begin(); citer != key.end(); ++citer) {
        if (NULL == p_node->next) {
            p_node->next = new TrieNode::NextMap;
        }
        km_iter = p_node->next->find(*citer);
        if (p_node->next->end() == km_iter) {
            TrieNode *nextNode = new TrieNode;

            p_node->next->insert(std::make_pair(*citer, nextNode));
            p_node = nextNode;
        } else {
            p_node = km_iter->second;
        }
    }
    // assert(p_node != NULL);
    // 最后一个节点记录word信息
    p_node->p_value = p_value;
}

void Trie::createTrie(const std::vector<Unicode>& keys,
        const std::vector<const DictUnit*>& value_pointers) {
    if (value_pointers.empty() || keys.empty()) {
        return;
    }
    // assert(keys.size() == value_pointers.size());
    for (size_t i = 0; i < keys.size(); i++) {
        insertNode(keys[i], value_pointers[i]);
    }
}

void Trie::deleteNode(TrieNode* node) {
    if (NULL == node) {
        return;
    }
    if (NULL != node->next) {
        for (TrieNode::NextMap::iterator it = node->next->begin();
                it != node->next->end(); ++it) {
            deleteNode(it->second);
        }
        delete node->next;
        node->next = NULL;
    }
    delete node;
    node = NULL;
}

}



/* vim: set ts=4 sw=4 sts=4 tw=100 */
