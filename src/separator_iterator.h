/*
 * =====================================================================================
 * 
 *       Filename:  separator_iterator.h 
 *    Description:  
 * 
 *        Created:  2022/01/28 14:47:22
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_SEPARATOR_ITERATOR_H
#define TEXT_ANALYSIS_SEPARATOR_ITERATOR_H

#include <unordered_set>
#include "trie.h"

namespace text_analysis {

class SeparatorIter {
public:
    SeparatorIter(const std::unordered_set<Rune>& symbols, const std::string& sentence)
        : symbols_(symbols) {
        // add check
        decodeRunesInString(sentence, sentence_);
        cursor_ = sentence_.begin();
    }
    ~SeparatorIter() {
    }

    bool hasNext() const {
        return cursor_ != sentence_.end();
    }

    WordRange next() {
        WordRange range;
        range.left = cursor_;
        while (cursor_ != sentence_.end()) {
            if (symbols_.find(cursor_->rune) == symbols_.end()) {
                cursor_++;
            } else {
                if (range.left == cursor_) {
                    range.left = ++cursor_;
                } else {
                    range.right = cursor_-1;
                    return range;
                }
            }
            /**
            if (symbols_.find(cursor_->rune) != symbols_.end()) {
                if (range.begin == cursor_) {
                    cursor_++;
                }
                range.end = cursor_; 
                return range;
            }
            cursor_++;
            **/
        }
        range.right = sentence_.end()-1;
        return range;
    }

private:
    RuneStringArray sentence_;
    RuneStringArray::const_iterator cursor_;
    const std::unordered_set<Rune>& symbols_;
};

} 



#endif  // TEXT_ANALYSIS_SEPARATOR_ITERATOR_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
