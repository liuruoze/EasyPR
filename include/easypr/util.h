#ifndef EASYPR_UTIL_H_
#define EASYPR_UTIL_H_

#include <iostream>
#include <string>
#include <vector>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace easypr {

class Utils {
 public:
  static long getTimestamp();

  /*
   * Get file name from a given path
   * bool postfix: including the postfix
   */
  static std::string getFileName(const std::string& path,
                                 const bool postfix = false);

  /*
   * Split the given string into segements by a delimiter
   */
  static std::vector<std::string> splitString(const std::string& str,
                                              const char delimiter);

  /*
   * returns the smaller of the two numbers
   */
  template<typename T>
  static T min(const T& v1, const T& v2) {
    return (v1 < v2) ? v1 : v2;
  }

  /*
   * Get files from a given folder
   * all: including all sub-folders
   */
  static std::vector<std::string> getFiles(const std::string& folder,
                                           const bool all = true);

  /*
   * Print string lines to std::out from an array of const char*,
   * this function is used for displaying command tips.
   * lines: should be end with (const char*)NULL.
   */
  static void print_str_lines(const char** lines) {
    int index = 0;
    while (lines[index++]) {
      std::cout << lines[index - 1] << std::endl;
    }
  }

  /*
   * Print string lines using {"string1", "string2"},
   * this is a easier way benefit from C++11.
   */
  static void print_str_lines(const std::initializer_list<const char*>& lines) {
    for (auto line : lines) {
      std::cout << line << std::endl;
    }
  }

  //! levenshtein距离，用于计算两个车牌的距离
  //！EasyPR中用levenshtein距离衡量车牌识别与真实车牌的误差
  template<class T>
  static unsigned int levenshtein_distance(const T& s1, const T& s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

    for (unsigned int i = 0; i < prevCol.size(); i++)
      prevCol[i] = i;
    for (unsigned int i = 0; i < len1; i++) {
      col[0] = i + 1;
      for (unsigned int j = 0; j < len2; j++)
        col[j + 1] = easypr::Utils::min(
                easypr::Utils::min(prevCol[1 + j] + 1, col[j] + 1), \
                prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
      col.swap(prevCol);
    }
    return prevCol[len2];
  }


};

} // namespace easypr

/// general_test_prepare.cpp中方法
int general_test();

#endif // EASYPR_UTIL_H_
