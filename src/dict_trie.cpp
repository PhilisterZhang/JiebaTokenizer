/*
 * =====================================================================================
 * 
 *       Filename:  dict_trie.cpp 
 *    Description:  
 * 
 *        Created:  2022/01/26 18:22:24
 *         Author:  philister.zhang
 *   Organization:  
 * 
 * =====================================================================================
 */
#include <exception>
#include <algorithm>

#include "dict_trie.h"
#include "nlp_stringutil.h"

namespace text_analysis {

DictTrie::DictTrie(const std::string& dict_path) {
    init(dict_path);
}

// load词典, 区分语言
// 一个实例只支持一个语言,暂不支持多国词典混合
bool DictTrie::init(const std::string& dict_path) {
    // 加载默认字典
    if (!loadDict(dict_path)) {
        return false;
    }
    // 计算词频
    freq_sum_ = calcFreqSum(node_infos_);
    // 默认字典的freq赋值
    setDefaultWordWeights();
    // 计算weights
    calculateWeight(node_infos_, freq_sum_);
    // 缩短
    shrink(node_infos_);
    // 构建trie树
    createTrie(node_infos_);
    return true;
}

bool DictTrie::initStopWords(const std::string& stop_words_path) {
    // load stop words
    if (!loadStopWordsDict(stop_words_path)) {
        return false;
    }
    // 构建trie树
    createTrie(node_infos_);
    return true;
}

void DictTrie::createTrie(const std::vector<DictUnit>& dict_units) {
    // assert(dict_units.size());

    std::vector<Unicode> words;
    std::vector<const DictUnit*> value_pointers;
    for (size_t i = 0 ; i < dict_units.size(); i++) {
        words.push_back(dict_units[i].word);
        value_pointers.push_back(&dict_units[i]);
    }
    trie_ = new Trie(words, value_pointers);
}

// 不支持postag
bool DictTrie::makeNodeInfo(DictUnit& node_info,
        const std::string& word, 
        double weight) { 
    if (!decodeRunesInString(word, node_info.word)) {
        return false;
    }
    node_info.weight = weight;
    // TODO(philister): 补上postag功能
    return true;
}

bool DictTrie::makeNodeInfo(DictUnit& node_info,
        const std::vector<std::string>& runes,
        double weight) {
    auto&& unicode = node_info.word;
    unicode.clear();
    unicode.reserve(runes.size());
    for (const auto& rune : runes) {
        unicode.push_back(std::stoull(rune));
    }
    node_info.weight = weight;
    return true;
}

bool DictTrie::loadDict(const std::string& file_path) {
    std::ifstream infile;
    infile.open(file_path.c_str());   //将文件流对象与文件连接起来 
    if(infile.fail()){
        return false;
    }
    std::string line;
    DictUnit node_info;
    while(getline(infile, line)) {
        StringUtil::trim(line);
        if (StringUtil::startsWith(line, "#")) {
            continue;
        }
        auto buf = StringUtil::split(line, "\t", true);
        if (buf.size() < 2) {continue;}
        // 暂不支持postag: word\tfreq\tpostag
        try {
            makeNodeInfo(node_info,
                buf[0],
                std::stod(buf[1])
            );
            node_infos_.push_back(node_info);
        } catch (const std::exception& e) {
            continue;     
        }
    }
    infile.close();
    return true;
}

bool DictTrie::loadStopWordsDict(const std::string& file_path) {
    std::ifstream infile;
    infile.open(file_path.c_str());   //将文件流对象与文件连接起来 
    if(infile.fail()){
        return false;
    }
    std::string line;
    DictUnit node_info;
    while(getline(infile, line)) {
        StringUtil::trim(line);
        // 注释
        if (StringUtil::startsWith(line, "#")) {
            continue;
        }
        auto buf = StringUtil::split(line, " ", true);
        if (buf.size() < 1) {continue;}
        try {
            // 停用词权重默认为 0.0
            makeNodeInfo(node_info, buf, 0.0);
            node_infos_.push_back(node_info);
        } catch (const std::exception& e) {
            continue;     
        }
    }
    infile.close();
    return true;
}

void DictTrie::setDefaultWordWeights() {
    // assert(node_infos_.size() > 0);
    // 默认顺序是字典序, 这样在建立trie树时会加快速度, 所以计算freq需要重排 
    // 可能会存在内存和效率的损耗, 看具体情况处理
    std::vector<DictUnit> temp_nodes = node_infos_;
    std::sort(temp_nodes.begin(), temp_nodes.end(), weightCompare);
    min_weight_ = temp_nodes.front().weight;
    max_weight_ = temp_nodes.back().weight;
}

}


/* vim: set ts=4 sw=4 sts=4 tw=100 */
