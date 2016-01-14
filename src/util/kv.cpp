#include "easypr/util/kv.h"
#include "easypr/util/util.h"

namespace easypr {

Kv::Kv() { }

void Kv::load(const std::string &file) {
  this->clear();

  std::ifstream reader(file);
  while (!reader.eof()) {
    std::string line;
    std::getline(reader, line);
    if (line.empty()) continue;

    const auto parse = [](const std::string &str) {
      std::string tmp, key, value;
      for (size_t i = 0, len = str.length(); i < len; ++i) {
        const char ch = str[i];
        if (ch == ' ') {
          if (i > 0 && str[i - 1] != ' ' && key.empty()) {
            key = tmp;
            tmp.clear();
          }
        } else {
          tmp.push_back(ch);
        }
        if (i == len - 1) {
          value = tmp;
        }
      }
      return std::make_pair(key, value);
    };

    auto kv = parse(line);
    this->add(kv.first, kv.second);
  }
  reader.close();
}

std::string Kv::get(const std::string &key) {
  if (data_.find(key) == data_.end()) {
    std::cerr << "[Kv] cannot find " << key << std::endl;
    return "";
  }
  return data_.at(key);
}

void Kv::add(const std::string &key, const std::string &value) {
  if (data_.find(key) != data_.end()) {
    fprintf(stderr,
            "[Kv] find duplicate: %s = %s , ignore\n",
            key.c_str(),
            value.c_str());
  } else {
    std::string v(value);
#ifdef OS_WINDOWS
    v = utils::utf8_to_gbk(value.c_str());
#endif
    data_[key] = v;
  }
}

void Kv::remove(const std::string &key) {
  if (data_.find(key) == data_.end()) {
    std::cerr << "[Kv] cannot find " << key << std::endl;
    return;
  }
  data_.erase(key);
}

void Kv::clear() {
  data_.clear();
}

}
