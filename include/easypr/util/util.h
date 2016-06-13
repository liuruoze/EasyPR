#ifndef EASYPR_UTIL_UTIL_H_
#define EASYPR_UTIL_UTIL_H_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"

#if defined(WIN32) || defined(_WIN32)
#define OS_WINDOWS
#elif defined(__APPLE__) || defined(APPLE)
#define OS_UNIX
#elif defined(__linux__) || defined(linux)
#define OS_LINUX
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
  if ((p)) {            \
    delete (p);         \
    (p) = NULL;         \
  }
#endif

namespace easypr {
class Utils {
 public:
  static long getTimestamp();

  /*
   * Get file name from a given path
   * bool postfix: including the postfix
   */
  static std::string getFileName(const std::string &path,
                                 const bool postfix = false);

  /*
   * Split the given string into segements by a delimiter
   */
  static std::vector<std::string> splitString(const std::string &str,
                                              const char delimiter);

  /*
   * returns the smaller of the two numbers
   */
  template<typename T>
  static T min(const T &v1, const T &v2) {
    return (v1 < v2) ? v1 : v2;
  }

  /*
   * Get files from a given folder
   * all: including all sub-folders
   */
  static std::vector<std::string> getFiles(const std::string &folder,
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
  static void print_str_lines(const std::initializer_list<const char*> &lines) {
    for (auto line : lines) {
      std::cout << line << std::endl;
    }
  }

  /*
   * Read and print by line.
   */
  static void print_file_lines(const std::string &file) {
    std::ifstream fs(file);
    if (fs.good()) {
      while (!fs.eof()) {
        std::string line;
        std::getline(fs, line);
#ifdef OS_WINDOWS
        line = utf8_to_gbk(line.c_str());
#endif
        std::cout << line << std::endl;
      }
      fs.close();
    } else {
      std::cerr << "cannot open file: " << file << std::endl;
    }
  }

  template<class T>
  static unsigned int levenshtein_distance(const T &s1, const T &s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

    for (unsigned int i = 0; i < prevCol.size(); i++) prevCol[i] = i;
    for (unsigned int i = 0; i < len1; i++) {
      col[0] = i + 1;
      for (unsigned int j = 0; j < len2; j++)
        col[j + 1] = easypr::Utils::min(
            easypr::Utils::min(prevCol[1 + j] + 1, col[j] + 1),
            prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
      col.swap(prevCol);
    }
    return prevCol[len2];
  }

  /*
   * Create multi-level directories by given folder.
   */
  static bool mkdir(const std::string folder);

  /*
   * Make sure the destination folder exists,
   * if not, create it, then call cv::imwrite.
   */
  static bool imwrite(const std::string &file, const cv::Mat &image);

#ifdef OS_WINDOWS
  static std::string utf8_to_gbk(const char* utf8);
#endif

 private:
  /*
   * Get the last slash from a path, compatible with Windows and *unix.
   */
  static std::size_t get_last_slash(const std::string &path);
};

typedef Utils utils;

}  // namespace easypr

#endif  // EASYPR_UTIL_UTIL_H_