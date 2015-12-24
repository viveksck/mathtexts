// author: Konstantin Arkhipenko

#include <algorithm>
#include <clocale>
#include <cwctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

typedef std::map<std::wstring, int> vocabulary_t;
typedef std::tuple<int, std::wstring, int, std::wstring> token_t;
typedef std::vector<token_t> sentence_t;

vocabulary_t loadVocabulary(const std::string &filename)
{
  std::wifstream file;
  std::wstring line;
  std::wstring word; int count;
  vocabulary_t result;

  file.imbue(std::locale("ru_RU.UTF-8"));
  file.open(filename);

  while (std::getline(file, line)) {
    std::wistringstream stream(line);

    stream >> word >> count;
    result[word] = count;
  }

  file.close();

  return result;
}

bool isWord(const std::wstring &str)
{
  for (auto i = str.begin(); i != str.end(); ++i) {
    if (!(std::iswalnum(*i) || *i == L'-')) {
      return false;
    }
  }

  return true;
}

void printPairs(const vocabulary_t &vocabulary, const sentence_t &sentence)
{
  for (sentence_t::size_type i = 1; i < sentence.size(); ++i) {
    auto token = sentence[i];
    int parentIndex;
    std::wstring tokenWord, parentTokenWord, relation;

    std::tie(std::ignore, tokenWord, parentIndex, relation) = token;
    auto parentToken = sentence[parentIndex];
    std::tie(std::ignore, parentTokenWord,
             std::ignore, std::ignore) = parentToken;

    if (vocabulary.find(tokenWord) == vocabulary.end() ||
        vocabulary.find(parentTokenWord) == vocabulary.end()) {
      continue;
    }

    if (relation == L"undef" || relation == L"theme" || relation == L"ex" ||
        relation == L"punct" || relation == L"lexmod") {
      continue;
    }

    if (!(isWord(tokenWord) && isWord(parentTokenWord))) {
      continue;
    }

    std::wcout << parentTokenWord << L' ' << relation <<
                  L'_' << tokenWord << L'\n';
    std::wcout << tokenWord << L' ' << relation <<
                  L"I_" << parentTokenWord << L'\n';
  }
}

int main(int argc, char **argv)
{
  std::setlocale(LC_ALL, "ru_RU.UTF-8");

  auto vocabulary = loadVocabulary(argv[1]);
  token_t root = std::make_tuple( 0, std::wstring(L"*root*"),
                                 -1, std::wstring(L"rroot"));
  auto sentence = sentence_t(1, root);
  std::wstring line;

  while (std::getline(std::wcin, line)) {
    line.erase(std::remove(line.begin(), line.end(), L'\n'), line.end());
    std::transform(line.begin(), line.end(), line.begin(), std::towlower);
    
    std::wistringstream stream(line);
    std::vector<std::wstring> rawSentence;

    std::copy(std::istream_iterator<std::wstring, wchar_t>(stream),
              std::istream_iterator<std::wstring, wchar_t>(),
              std::back_inserter(rawSentence));

    if (rawSentence.empty()) {
      if (sentence.size() > 1) {
        printPairs(vocabulary, sentence);
      }

      sentence = sentence_t(1, root);
    }
    else {
      int index = std::stoi(rawSentence[0]);
      auto tokenWord = rawSentence[1];
      int parentIndex = std::stoi(rawSentence[rawSentence.size() - 2]);
      auto relation = rawSentence[rawSentence.size() - 1];

      sentence.push_back(std::make_tuple(index, tokenWord,
                                         parentIndex, relation));
    }
  }

  if (sentence.size() > 1) {
    printPairs(vocabulary, sentence);
  }

  return 0;
}
