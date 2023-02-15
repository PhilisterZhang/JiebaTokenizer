#ifndef TEXT_ANALYSIS_STRING_UTIL_H
#define TEXT_ANALYSIS_STRING_UTIL_H

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>

namespace text_analysis {

class StringUtil {
public:
    static const std::string NULL_STRING;
public:
    StringUtil() {}
    ~StringUtil() {}
public:
    static void trim(std::string &str);
    static bool startsWith(const std::string &str, const std::string &prefix);
    static bool endsWith(const std::string &str, const std::string &suffix);

    static std::vector<std::string> split(const std::string &text,
                                          const std::string &sepStr,
                                          bool ignoreEmpty = true);
    static void split(std::vector<std::string> &vec,
                      const std::string &text,
                      const std::string &sepStr,
                      bool ignoreEmpty = true);
    static bool isSpace(const std::string &text);

    static void toUpperCase(char *str);
    static void toUpperCase(const char *str, std::string &retStr);
    static void toUpperCase(std::string &str);

    static void toLowerCase(char *str);
    static void toLowerCase(std::string &str);
    static void toLowerCase(const char *str, std::string &retStr);

    static std::string &replaceAll(std::string &str, const std::string &oldValue, const std::string &newValue);

    static void join(const std::vector<std::string>& text_list,
            std::string& output, std::string hyphen, size_t begin, size_t end);
    static void join(const std::vector<std::string>& text_list,
            std::string& output, std::string hyphen);
    static std::string join(const std::vector<std::string>& text_list,
            std::string hyphen, size_t begin, size_t end);
    static std::string join(const std::vector<std::string>& text_list, std::string hyphen);

    static size_t getUtf8Len(const unsigned char& byte);
    static void trans2Unicode(const std::string& ori_text, std::vector<std::string>& output); 
};

inline size_t StringUtil::getUtf8Len(const unsigned char& byte) {
    if (byte >= 0xFC) {
        return 6;
    } else if (byte >= 0xF8) {
        return 5;
    } else if (byte >= 0xF0) {
        return 4;
    } else if (byte >= 0xE0) {
        return 3;
    } else if (byte >= 0xC0) {
        return 2;
    } else if (byte < 0x80) {
        return 1;
    }
    return 1;  //default
}

}
#endif

/*  vim: set ts=4 sw=4 sts=4 tw=100 : */
