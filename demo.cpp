#include <memory>
#include <exception>
#include <map>
#include <chrono>

#include "src/make_unique.h"
#include "src/text_analyzer.h"
#include "src/nlp_stringutil.h"

using namespace std;

const char* const STOP_DICT_PATH = "data/symbols.unicode.txt";

int main(int argc, char** argv) {
    map<string, string> dicts;
    // dicts["id"] = "data/dict/id.dict.utf8";
    dicts["cn"] = "data/dict/cn.dict.utf8";
    // dicts["sg"] = "data/dict/sg.dict.utf8";
    // dicts["tw"] = "data/dict/tw.dict.utf8";
    // dicts["my"] = "data/dict/my.dict.utf8";
    // dicts["ph"] = "data/dict/ph.dict.utf8";
    // dicts["pg"] = "data/dict/portuguese.dict.utf8";
    // FIXME(philister): vn gets syllable not word
    // dicts["vn"] = "data/dict/vn.dict.utf8";

    unique_ptr<text_analysis::TextAnalyzer> analyzer;
    analyzer = make_unique<text_analysis::TextAnalyzer>();

    // std::cout << "start init trie ..." << std::endl;
    for (auto&& dic : dicts) {
        // std::cout << "adding dict: " << dic.first << std::endl;
        analyzer->addDict(dic.first, dic.second);
    }

    // stop words
    analyzer->addStopWordsDict(STOP_DICT_PATH);

    analyzer->init();
    // std::cout << "init success" << std::endl;

    vector<string> words;
    string s;
    string country = "cn";
    if (argc > 1) {
        country = argv[1];
    }
    while (getline(std::cin, s)) {
        analyzer->normalize(s, words);
        cout <<  "normlize: " << text_analysis::StringUtil::join(words, " ") << std::endl;
        analyzer->cut(s, country,  words);
        cout <<  "mmseg: " << text_analysis::StringUtil::join(words, " ") << std::endl;
        analyzer->cutMP(s, country, words);
        cout << "mpseg: " << text_analysis::StringUtil::join(words, " ") << std::endl;
    }
    return 0;
}
