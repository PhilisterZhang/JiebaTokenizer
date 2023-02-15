# JiebaTokenizer

基于jieba 分词增加了一些多语言支持, 归一化操作.                

jieba分词见: https://github.com/fxsjy/jieba            
jieba分词cpp代码见: https://github.com/yanyiwu/cppjieba


### (1) 基本信息

(1) 分词工具, 支持多语言分词             

(2) 目前支持 MP\MM 两种分词模式                    

(3) 支持基本的normalize;                      
- 去除控制字符                   
- 去除emoji (共4702种)                     
- 去除Mn (存在风险, accent 对于很多语言是必须的, 需要根据具体的语言谨慎选择)                              
- 根据标点分割                    
- 数字单独分割                      
- 多语言按国家地区分别加载词典

(4) 暂不支持:                   
- 数字、单位的标准化处理               
- 相同意义字符的转化: 如 ⑩->10                  
- 多语言混杂的情况 (目前会按unicode输出, 或者分块输出)                      
- 动态更新词典

### (2) 接口说明

```c++
// 初始化词典, 按地区添加
// param1: dict_path 某地区词频统计词典
// example: text_analyzer->addDict("id", "id.dict.utf8")
void TextAnalyzer::addDict(const std::string& country, const std::string& dict_path);

// 返回归一化后的结果 (见(1) 中描述)
bool TextAnalyzer::normalize(const std::string& sentence, std::vector<std::string>& res) const;
std::string TextAnalyzer::normalize(const std::string& sentence) const;
// 动态规划分词 
void TextAnalyzer::cutMP(const std::string& sentence, const std::string& country, std::vector<std::string>& res) const; 
```

### (3) 词典获取

- 可以自己统计词频构建词典                              
- https://wortschatz.uni-leipzig.de/en/download 有部分网络抓取的语料