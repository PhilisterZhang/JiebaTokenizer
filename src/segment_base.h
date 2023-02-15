/*
 * =====================================================================================
 * 
 *       Filename:  segment_base.h 
 *    Description:  
 * 
 *        Created:  2022/01/27 10:36:39
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#ifndef TEXT_ANALYSIS_SEGMENT_BASE_H
#define TEXT_ANALYSIS_SEGMENT_BASE_H

#include <string>
#include <vector>
#include <unordered_set>

#include "unicode.h"

namespace text_analysis {

// 默认切分符号
const char* const SEPARATORS = " \n\t-";

class SegmentBase {
public:
    SegmentBase() {
        resetSeparators(SEPARATORS);
    }
    virtual ~SegmentBase() {
    }
    virtual void cut(const std::string& text, std::vector<std::string>& res) const = 0;

    bool resetSeparators(const std::string& s) {
        symbols_.clear();
        RuneStringArray runes;
        if (!decodeRunesInString(s, runes)) {
            // TODO(philister): 需要增加log
            return false;
        }
        for (size_t i = 0; i < runes.size(); i++) {
            if (!symbols_.insert(runes[i].rune).second) {
                return false;
            }
        }
        return true;
    }
protected:
    std::unordered_set<Rune> symbols_;
};

}
#endif  // TEXT_ANALYSIS_SEGMENT_BASE_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
