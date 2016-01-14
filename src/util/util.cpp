#include "easypr/util/util.h"
#include <string>

#ifdef OS_WINDOWS
#include <windows.h>
#include <direct.h>
#include <io.h>
#define PATH_DELIMITER '\\'
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#elif defined(OS_LINUX) || defined(OS_UNIX)

#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define PATH_DELIMITER '/'
#endif

#ifdef OS_UNIX

#include <sys/timeb.h>

#endif

#include <list>
#include <opencv2/highgui/highgui.hpp>

namespace easypr {

long Utils::getTimestamp() {
#ifdef OS_WINDOWS
  return static_cast<long>(cv::getTickCount());
#endif

#ifdef OS_LINUX
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (ts.tv_sec * 1e3 + ts.tv_nsec / 1e6);
#endif

#ifdef OS_UNIX
  // there is no function provided by osx to get system tick count.
  // but considering the purpose by using this function,
  // we can simply return a millisecond since 1970/1/1 to calc the time elapse.
  struct timeb tb;
  ftime(&tb);
  return long(tb.time * 1e3 + tb.millitm);
#endif
}

std::string Utils::getFileName(const std::string &path,
                               const bool postfix /* = false */) {
  if (!path.empty()) {
    size_t last_slash = utils::get_last_slash(path);
    size_t last_dot = path.find_last_of('.');

    if (last_dot < last_slash || last_dot == std::string::npos) {
      // not found the right dot of the postfix,
      // return the file name directly
      return path.substr(last_slash + 1);
    } else {
      // the path has a postfix
      if (postfix) {
        // return the file name including postfix
        return path.substr(last_slash + 1);
      }
      // without postfix
      return path.substr(last_slash + 1, last_dot - last_slash - 1);
    }
  }
  return "";
}

std::vector<std::string> Utils::splitString(const std::string &str,
                                            const char delimiter) {
  std::vector<std::string> splited;
  std::string s(str);
  size_t pos;

  while ((pos = s.find(delimiter)) != std::string::npos) {
    std::string sec = s.substr(0, pos);

    if (!sec.empty()) {
      splited.push_back(s.substr(0, pos));
    }

    s = s.substr(pos + 1);
  }

  splited.push_back(s);

  return splited;
}

std::vector<std::string> Utils::getFiles(const std::string &folder,
                                         const bool all /* = true */) {
  std::vector<std::string> files;
  std::list<std::string> subfolders;
  subfolders.push_back(folder);
#ifdef OS_WINDOWS
  while (!subfolders.empty()) {
    std::string current_folder(subfolders.back());

    if (*(current_folder.end() - 1) != '/') {
      current_folder.append("/*");
    } else {
      current_folder.append("*");
    }

    subfolders.pop_back();

    struct _finddata_t file_info;
    auto file_handler = _findfirst(current_folder.c_str(), &file_info);

    while (file_handler != -1) {
      if (all &&
          (!strcmp(file_info.name, ".") || !strcmp(file_info.name, ".."))) {
        if (_findnext(file_handler, &file_info) != 0) break;
        continue;
      }

      if (file_info.attrib & _A_SUBDIR) {
        // it's a sub folder
        if (all) {
          // will search sub folder
          std::string folder(current_folder);
          folder.pop_back();
          folder.append(file_info.name);

          subfolders.push_back(folder.c_str());
        }
      } else {
        // it's a file
        std::string file_path;
        // current_folder.pop_back();
        file_path.assign(current_folder.c_str()).pop_back();
        file_path.append(file_info.name);

        files.push_back(file_path);
      }

      if (_findnext(file_handler, &file_info) != 0) break;
    }  // while
    _findclose(file_handler);
  }
#elif defined(OS_LINUX) || defined(OS_UNIX)
  while (!subfolders.empty()) {
    std::string current_folder(subfolders.back());

    if (*(current_folder.end() - 1) != '/') {
      current_folder.push_back('/');
    }

    DIR* pdir = opendir(current_folder.c_str());

    subfolders.pop_back();

    if (!pdir) {
      continue;
    }

    dirent* dir = NULL;

    while ((dir = readdir(pdir)) != NULL) {
      // iterates the current folder, search file & sub folder
      struct stat st;

      if (all && (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))) {
        // must ignore . & ..
        continue;
      }

      if (!strcmp(dir->d_name, ".DS_Store")) {
        // in OSX, 'finder' will create .DS_Store
        continue;
      }

      std::string file_path;

      file_path.append(current_folder.c_str());
      file_path.append(dir->d_name);

      if (lstat(file_path.c_str(), &st) < 0) {
        // perror("lstat");
        continue;
      }

      if (S_ISDIR(st.st_mode)) {
        // it's a sub folder
        if (all) {
          // will search sub folder
          std::string subfolder(current_folder);
          subfolder.append(dir->d_name);

          subfolders.push_back(subfolder.c_str());
        }
      } else {
        // it's a file
        files.push_back(file_path);
      }
    }  // while
    closedir(pdir);
  }
#endif
  return files;
}

bool Utils::mkdir(const std::string folder) {
  std::string folder_builder;
  std::string sub;
  sub.reserve(folder.size());
  for (auto it = folder.begin(); it != folder.end(); ++it) {
    const char c = *it;
    sub.push_back(c);
    if (c == PATH_DELIMITER || it == folder.end() - 1) {
      folder_builder.append(sub);
#ifdef OS_WINDOWS
      if (0 != ::_access(folder_builder.c_str(), 0)) {
#else
      if (0 != ::access(folder_builder.c_str(), 0)) {
#endif
        // this folder not exist
#ifdef OS_WINDOWS
        if (0 != ::_mkdir(folder_builder.c_str())) {
#else
        if (0 != ::mkdir(folder_builder.c_str(), S_IRWXU)) {
#endif
          // create failed
          return false;
        }
      }
      sub.clear();
    }
  }
  return true;
}

bool Utils::imwrite(const std::string &file, const cv::Mat &image) {
  auto folder = file.substr(0, utils::get_last_slash(file));
  Utils::mkdir(folder);
  return cv::imwrite(file, image);
}

#ifdef OS_WINDOWS
std::string Utils::utf8_to_gbk(const char* utf8) {
  int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
  wchar_t* wszGBK = new wchar_t[len + 1];
  memset(wszGBK, 0, len * 2 + 2);
  MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wszGBK, len);
  len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
  char* szGBK = new char[len + 1];
  memset(szGBK, 0, len + 1);
  WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
  std::string strTemp(szGBK);
  if (wszGBK)
    delete[] wszGBK;
  if (szGBK)
    delete[] szGBK;
  return strTemp;
}
#endif

std::size_t Utils::get_last_slash(const std::string &path) {
#ifdef OS_WINDOWS
  size_t last_slash_1 = path.find_last_of("\\");
  size_t last_slash_2 = path.find_last_of("/");
  size_t last_slash;

  if (last_slash_1 != std::string::npos && last_slash_2 != std::string::npos) {
    // C:/path\\to/file.postfix
    last_slash = std::max(last_slash_1, last_slash_2);
  } else {
    // C:\\path\\to\\file.postfix
    // C:/path/to/file.postfix
    last_slash =
        (last_slash_1 == std::string::npos) ? last_slash_2 : last_slash_1;
  }
#else
  size_t last_slash = path.find_last_of('/');
#endif
  return last_slash;
}

}  // namespace easypr