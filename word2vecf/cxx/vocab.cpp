// author: Konstantin Arkhipenko

#include <algorithm>
#include <cwctype>
#include <iostream>
#include <map>
#include <string>

int main(int argc, char **argv)
{
  int threshold = std::stoi(std::string(argv[1]));
  std::map<std::string, int> vocabulary;
  std::string line;

  while (std::getline(std::cin, line)) {
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    std::transform(line.begin(), line.end(), line.begin(), std::towlower);

    if (vocabulary.find(line) == vocabulary.end()) {
      vocabulary[line] = 1;
    }
    else {
      ++vocabulary[line];
    }
  }

  for (auto i = vocabulary.begin(); i != vocabulary.end(); ++i) {
    if (i->second >= threshold) {
      std::cout << i->first << ' ' << i->second << '\n';
    }
  }

  return 0;
}
