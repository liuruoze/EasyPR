#include "easypr/util.h"

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#include <io.h>
#elif defined(linux) || defined(__linux__) || defined(__APPLE__)

#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

#endif

#if defined(__APPLE__)

#include <sys/timeb.h>

#endif

#include <list>

using namespace std;
using namespace easypr;

long Utils::getTimestamp() {
#if defined(WIN32) || defined(_WIN32)
  return GetTickCount();
#endif

#if (linux) || defined(__linux__)
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);

  return (ts.tv_sec * 1e3 + ts.tv_nsec / 1e6);
#endif

#if defined(__APPLE__)
  // there is no function provided by osx to get system tick count.
  // but considering the purpose by using this function,
  // we can simply return a millisecond since 1970/1/1 to calc the time elapse.
  struct timeb tb;
  ftime(&tb);
  return long(tb.time * 1e3 + tb.millitm);
#endif
}

std::string Utils::getFileName(const string& path,
                               const bool postfix /* = false */) {
  if (!path.empty()) {
#if defined(WIN32) || defined(_WIN32)
    size_t last_slash_1 = path.find_last_of("\\");
    size_t last_slash_2 = path.find_last_of("/");
    size_t last_slash;

    if (last_slash_1 != std::string::npos &&
        last_slash_2 != std::string::npos) {
      // C:/path\\to/file.postfix
      last_slash = max(last_slash_1, last_slash_2);
    } else {
      // C:\\path\\to\\file.postfix
      // C:/path/to/file.postfix
      last_slash =
          (last_slash_1 == std::string::npos) ? last_slash_2 : last_slash_1;
    }
#else
    size_t last_slash = path.find_last_of('/');
#endif
    size_t last_dot = path.find_last_of('.');

    if (last_dot < last_slash || last_dot == string::npos) {
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

vector<string> Utils::splitString(const string& str, const char delimiter) {
  vector<string> splited;
  string s(str);
  size_t pos;

  while ((pos = s.find(delimiter)) != string::npos) {
    string sec = s.substr(0, pos);

    if (!sec.empty()) {
      splited.push_back(s.substr(0, pos));
    }

    s = s.substr(pos + 1);
  }

  splited.push_back(s);

  return splited;
}

vector<string> Utils::getFiles(const string& folder,
                               const bool all /* = true */) {
  vector<string> files;
  list<string> subfolders;
  subfolders.push_back(folder);
#if defined(WIN32) || defined(_WIN32)
  while (!subfolders.empty()) {
    string current_folder(subfolders.back());

    if (*(current_folder.end() - 1) != '/') {
      current_folder.append("/*");
    } else {
      current_folder.append("*");
    }

    subfolders.pop_back();

    struct _finddata_t file_info;
    long file_handler = _findfirst(current_folder.c_str(), &file_info);

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
          string folder(current_folder);
          folder.pop_back();
          folder.append(file_info.name);

          subfolders.push_back(folder.c_str());
        }
      } else {
        // it's a file
        string file_path;
        // current_folder.pop_back();
        file_path.assign(current_folder.c_str()).pop_back();
        file_path.append(file_info.name);

        files.push_back(file_path);
      }

      if (_findnext(file_handler, &file_info) != 0) break;
    }  // while
    _findclose(file_handler);
  }
#elif defined(linux) || defined(__linux__) || defined(__APPLE__)
  while (!subfolders.empty()) {
    string current_folder(subfolders.back());

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

      string file_path;

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
          string subfolder(current_folder);
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
