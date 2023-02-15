#include <stdlib.h>
#include <stdint.h>
#include <algorithm>
#include <string.h>

#include "nlp_stringutil.h"

namespace text_analysis {

const std::string StringUtil::NULL_STRING = "";

void StringUtil::trim(std::string &str) {
    str.erase(str.find_last_not_of(' ') + 1);
    str.erase(0, str.find_first_not_of(' '));
}

bool StringUtil::startsWith(const std::string &str, const std::string &prefix)
{
    return (str.size() >= prefix.size())
           && (str.compare(0, prefix.size(), prefix) == 0);
}

bool StringUtil::endsWith(const std::string &str, const std::string &suffix)
{
    size_t s1 = str.size();
    size_t s2 = suffix.size();
    return (s1 >= s2) && (str.compare(s1 - s2, s2, suffix) == 0);
}


std::vector<std::string> StringUtil::split(const std::string &text,
        const std::string &sepStr, bool ignoreEmpty) {
    std::vector<std::string> vec;
    std::string str(text);
    std::string sep(sepStr);
    size_t n = 0, old = 0;
    while (n != std::string::npos) {
        n = str.find(sep, n);
        if (n != std::string::npos) {
            if (!ignoreEmpty || n != old) {
                vec.push_back(str.substr(old, n - old));
            }
            n += sep.length();
            old = n;
        }
    }
    if (!ignoreEmpty || old < str.length()) {
        vec.push_back(str.substr(old, str.length() - old));
    }
    return vec;
}

void StringUtil::split(std::vector<std::string> &vec,
                       const std::string &text,
                       const std::string &sepStr,
                       bool ignoreEmpty) {
    vec.clear();
    std::string str(text);
    std::string sep(sepStr);
    size_t n = 0, old = 0;
    while (n != std::string::npos) {
        n = str.find(sep, n);
        if (n != std::string::npos) {
            if (!ignoreEmpty || n != old)
                vec.push_back(str.substr(old, n - old));
            n += sep.length();
            old = n;
        }
    }
    if (!ignoreEmpty || old < str.length()) {
        vec.push_back(str.substr(old, str.length() - old));
    }
}

bool StringUtil::isSpace(const std::string &text) {
    if (text == std::string("　")) {
        return true;
    }
    if (text.length() > 1) {
        return false;
    }
    return isspace(text[0]);
}

void StringUtil::toUpperCase(char *str) {
    if (str) {
        while (*str) {
            if (*str >= 'a' && *str <= 'z') {
                *str += 'A' - 'a';
            }
            str++;
        }
    }
}

void StringUtil::toUpperCase(std::string &str) {
    for (size_t i = 0; i < str.size(); i++) {
        str[i] = toupper(str[i]);
    }
}

void StringUtil::toUpperCase(const char *str, std::string &retStr) {
    retStr = str;
    for (size_t i = 0; i < retStr.size(); i++) {
        retStr[i] = toupper(str[i]);
    }
}

void StringUtil::toLowerCase(std::string &str) {
    for (size_t i = 0; i < str.size(); i++) {
        str[i] = tolower(str[i]);
    }
}

void StringUtil::toLowerCase(char *str) {
    if (str) {
        while (*str) {
            if (*str >= 'A' && *str <= 'Z') {
                *str -= 'A' - 'a';
            }
            str++;
        }
    }
}

void StringUtil::toLowerCase(const char *str, std::string &retStr) {
    retStr = str;
    toLowerCase(retStr);
}

std::string &StringUtil::replaceAll(std::string &str, const std::string &old_value, const std::string &new_value) {
    while (true) {
        std::string::size_type pos(0);
        if ((pos = str.find(old_value)) != std::string::npos) {
            str.replace(pos, old_value.length(), new_value);
        }
        else
            break;
    }
    return str;
}

void StringUtil::join(const std::vector<std::string>& text_list, std::string& output, std::string hyphen, size_t begin, size_t end) {
    output.clear();
    for (std::vector<std::string>::const_iterator p = text_list.begin() + begin; p != text_list.begin() + end; ++p) {
        output.append(*p);
        if (p != text_list.end() - 1) {
            output.append(hyphen);
        }
    }
}

void StringUtil::join(const std::vector<std::string>& text_list, std::string& output, std::string hyphen) {
    output.clear();
    for (std::vector<std::string>::const_iterator p = text_list.begin(); p != text_list.end(); ++p) {
        output.append(*p);
        if (p != text_list.end() - 1) {
            output.append(hyphen);
        }
    }
}

std::string StringUtil::join(const std::vector<std::string>& text_list, std::string hyphen, size_t begin, size_t end) {
    std::string output("");
    for (std::vector<std::string>::const_iterator p = text_list.begin() + begin; p != text_list.begin() + end; ++p) {
        output.append(*p);
        if (p != text_list.end() - 1) {
            output.append(hyphen);
        }
    }
    return output;
}

std::string StringUtil::join(const std::vector<std::string>& text_list, std::string hyphen) {
    std::string output("");
    for (std::vector<std::string>::const_iterator p = text_list.begin(); p != text_list.end(); ++p) {
        output.append(*p);
        if (p != text_list.end() - 1) {
            output.append(hyphen);
        }
    }
    return output;
}

void StringUtil::trans2Unicode(const std::string& ori_text, std::vector<std::string>& unicode_texts) {
    for (size_t i = 0, len = 0; i < ori_text.length(); i+=len) {
        len = getUtf8Len(ori_text[i]);
        unicode_texts.insert(unicode_texts.end(), ori_text.substr(i, len));
    }
}

}

/*  vim: set ts=4 sw=4 sts=4 tw=100 : */
