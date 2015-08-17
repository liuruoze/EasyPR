#include "easypr/util/program_options.h"

namespace program_options {

// class ParseError

ParseError::ParseError(const std::string& msg) : _msg(msg) {}

const char* ParseError::what() const throw() {
  std::string msg;
  msg.append("Command line parse error: ").append(_msg).push_back('.');
  return msg.c_str();
}

ParseError::~ParseError() throw() {}

// class Generator

Generator::Generator() : parser_(nullptr) {
  current_subroutine_ = Subroutine::get_default_name();
  add_subroutine(current_subroutine_.c_str());
}

Generator::~Generator() {
  if (parser_) {
    delete parser_;
    parser_ = nullptr;
  }
  for (auto it = subroutines_.begin(); it != subroutines_.end(); ++it) {
    if (it->second) {
      delete it->second;
      it->second = nullptr;
    }
  }
}

Generator& Generator::make_usage(const char* first_line) {
  get_subroutine()->set_first_line(first_line);
  return *this;
}

Parser* Generator::make_parser() {
  if (parser_) delete parser_;
  parser_ = new Parser;
  parser_->set_usage_subroutines(&subroutines_);
  return parser_;
}

Generator& Generator::add_subroutine(const char* name) {
  add_subroutine(name, "");
  return *this;
}

Generator& Generator::add_subroutine(const char* name,
                                     const char* description) {
  if (subroutines_.find(name) == subroutines_.end()) {
    // a new subroutine
    current_subroutine_ = name;
    Subroutine* routine = new Subroutine(name, description);
    subroutines_.insert({current_subroutine_, routine});
  }
  return *this;
}

std::map<std::string, std::string> Generator::get_subroutine_list() {
  std::map<std::string, std::string> kv;
  for (auto pr : subroutines_) {
    Subroutine* subroutine = pr.second;
    if (subroutine->get_name() != Subroutine::get_default_name())
      kv[subroutine->get_name()] = subroutine->get_description();
  }
  return std::move(kv);
}

bool Generator::add_usage_line(const char* option, const char* default_value,
                               const char* description) {
  std::string option_str(option);
  auto delimiter_pos = option_str.find(kDelimiter);

  std::string option_short;
  std::string option_long;

  if (delimiter_pos != std::string::npos) {
    option_short.assign(std::move(option_str.substr(0, delimiter_pos)));
    option_long.assign(std::move(option_str.substr(delimiter_pos + 1)));

    Row row;
    row.oshort(option_short);
    row.olong(option_long);
    row.value(default_value);
    row.desc(description);

    get_subroutine()->add_usage_line(row);
    return true;
  }
  return false;
}

std::ostream& operator<<(std::ostream& out, Generator& generator) {
  for (auto pr : generator.subroutines_) {
    Subroutine* subroutine = pr.second;
    if (subroutine->get_name() != Subroutine::get_default_name()) {
      out << subroutine->get_name() << "\t";
    }
    out << subroutine->get_description();
    if (!subroutine->get_usage().empty()) {
      out << std::endl;
    }
    out << *subroutine;
  }
  return out;
}

// class ParseItem

ParseItem::ParseItem(const std::string& value) : value_(value) {}

// class Parser

ParseItem* Parser::get(const std::string& key) {
  if (pr_->find(key) != pr_->end()) {
    return (*pr_)[key];
  }
  return nullptr;
}

Parser::Parser() : subroutines_(nullptr), pr_(nullptr) {}

Parser::~Parser() { this->cleanup(); }

Parser::ParseResult* Parser::parse(const int argc, const char** argv) {
  if (!this->init(argc, argv)) {
    return nullptr;
  }
  auto ibegin = args_.begin() + 1;  // ignore the first cmd name
  auto iend = args_.end();
  auto it = ibegin;

  if (argc >= 2 && args_[1][0] != '-') {
    // the second block may be a subroutine name
    // e.g., ./exec pull --option
    if (subroutines_ && (subroutines_->find(args_[1]) != subroutines_->end())) {
      subroutine_name_ = args_[1];
      it++;  // ignore the subroutine name
    } else {
      subroutine_name_ = args_[1];
    }
  } else {
    // there is no options as well as subroutine name
    // e.g., ./exec
    subroutine_name_ = Subroutine::get_default_name();
  }

  std::string block;
  std::string previous(*ibegin);

  for (; it != iend; ++it) {
    block.assign(*it);

    switch (block.size()) {
      case 1:
        if (block == "-") {
          throw ParseError("single '-' is not allowed");
        }
        break;
      case 2:
        if (block[0] == '-') {
          if (block[1] == '-') {
            throw ParseError("option '--' is incomplete");
          } else if (block[1] == '=') {
            throw ParseError("option '-=' is invalid");
          } else {
            // single option
            // e.g., ./exec -s
            (*pr_)[block.substr(1)] = nullptr;
          }
        }
        break;
      default:  // >=3
        if (block[0] == '-') {
          if (block[1] == '-') {
            size_t pos_equal = block.find('=');
            if (pos_equal == std::string::npos) {
              // a long format option
              // e.g., ./exec --option
              (*pr_)[block.substr(2)] = nullptr;
            } else {
              if (pos_equal > 3) {
                // e.g, ./exec --op[..=]value
                std::string key(block.substr(2, pos_equal - 2));
                if (block.size() > 5)
                  // e.g, ./exec --op=v
                  (*pr_)[key] = new ParseItem(block.substr(pos_equal + 1));
                else
                  (*pr_)[key] = nullptr;
              } else {
                // a long format option but = is illegal
                // e.g., ./exec --o=[...]
                (*pr_)[block.substr(2)] = nullptr;
              }
            }
          } else if (block[2] == '=') {
            // a single option with =
            // e.g., ./exec -o=[...]
            std::string key;
            key.push_back(block[1]);
            if (block.size() > 3)
              (*pr_)[key] = new ParseItem(block.substr(3));
            else
              (*pr_)[key] = nullptr;
          } else {
            // a combination options
            // e.g., ./exec -ab[...]
            auto tbegin = block.begin() + 1;  // ignore the first '-'
            auto tend = block.end();
            auto t = tbegin;

            for (; t != tend; ++t) {
              std::string key;
              key.push_back(*t);
              (*pr_)[key] = nullptr;
            }
          }
        }
        break;
    }  // switch

    if (block[0] != '-' && previous != block  // not the first option
        ) {
      if (previous[0] != '-') {
        // previous is not an option, error occur
        // e.g., ./exec abc def
        throw ParseError("'" + block + "' is not allowed here");
      }

      std::string key;

      if (previous[0] == '-' && previous[1] == '-') {
        // previous is a long format option.
        // e.g., ./exec --option value
        key = previous.substr(2);
      } else {
        // it's the value of previous option.
        // e.g., ./exec -o   [...]
        // e.g., ./exec -opq [...]
        key.push_back(*(previous.end() - 1));
      }

      if (pr_->find(key) != pr_->end()) {
        (*pr_)[key] = new ParseItem(block);
      }
    }

    previous = block;
  }  // for

  if (subroutines_) {
    this->set_addition();
  }

  return pr_;
}

Parser::ParseResult* Parser::parse(const char* command_line) {
  int i = 0;
  std::string block;
  std::vector<std::string> blocks;
  char c;
  while ((c = command_line[i++]) != '\0') {
    if (c != ' ') {
      block.push_back(c);
    } else {
      if (!block.empty()) {
        blocks.push_back(block);
      }
      block.clear();
    }
  }
  if (!block.empty()) {
    blocks.push_back(block);
  }
  size_t size = blocks.size();  // argc
  char** argv = new char*[size];
  i = 0;
  std::for_each(blocks.begin(), blocks.end(), [argv, &i](const std::string& b) {
    argv[i++] = const_cast<char*>(b.c_str());
  });
  auto pr =
      this->parse(static_cast<const int>(size), const_cast<const char**>(argv));

  delete[] argv;
  argv = nullptr;

  return pr;
}

bool Parser::has(const char* key) {
  std::string skey(key);

  if (pr_ && !pr_->empty() && !skey.empty()) {
    if (skey[0] == '-') {
      // check combination options, e.g., Parser::has("-xyz")
      for (size_t i = 1; i < skey.size(); ++i) {
        std::string tkey;
        tkey.push_back(skey[i]);
        if (pr_->find(tkey) == pr_->end()) {
          return false;
        }
      }
      return true;
    } else {
      // check single option, e.g., Parser::has("x")
      return pr_->find(skey) != pr_->end();
    }
  }
  return false;
}

bool Parser::has_or(std::initializer_list<const char*> options) {
  if (options.size() == 0) {
    return false;
  }
  for (auto key : options) {
    if (this->has(key)) return true;
  }
  return false;
}

bool Parser::has_and(std::initializer_list<const char*> options) {
  if (options.size() == 0) {
    return false;
  }
  for (auto key : options) {
    if (!this->has(key)) return false;
  }
  return true;
}

bool Parser::init(const int argc, const char** argv) {
  argc_ = argc;
  // argv_ = argv;
  // don't save it, point to a local var in parse(const char* command_line).
  // use member var args_ instead.
  if (argc > 0) {
    this->cleanup();

    args_.reserve(static_cast<size_t>(argc_));

    for (int i = 0; i < argc_; ++i) {
      args_.push_back(argv[i]);
    }

    pr_ = new Parser::ParseResult;
    return true;
  }
  return false;
}

void Parser::cleanup() {
  args_.clear();
  if (pr_) {
    auto ibegin = pr_->begin();
    auto iend = pr_->end();
    auto it = ibegin;
    for (; it != iend; ++it) {
      ParseItem* item = it->second;
      if (item) delete item;
    }
    delete pr_;
    pr_ = nullptr;
  }
}

void Parser::set_addition() {
  if (subroutines_->find(subroutine_name_) != subroutines_->end()) {
    for (const Row& row : *(subroutines_->at(subroutine_name_))) {
      // assume both -o and --option are allowed,
      // but only provide -o,
      // then set the another --option.
      // vice versa.
      const std::string& def = row.value();
      const std::string& ops = row.oshort();
      const std::string& opl = row.olong();
      ParseResult& pr = *pr_;

      bool has_short = this->has(ops.c_str());
      bool has_long = this->has(opl.c_str());

      // assume -o [ --option ] arg = 1
      // but not provide option value,
      // then set to default 1.
      // otherwise, both set to user defined value

      if (!ops.empty()) {
        if (has_short) {
          if (pr[ops] != nullptr && !opl.empty()) {
            pr[opl] = new ParseItem(std::move(pr[ops]->val()));
          } else if (pr[ops] == nullptr && !def.empty()) {
            pr[ops] = new ParseItem(std::move(def));
            if (!opl.empty()) pr[opl] = new ParseItem(std::move(def));
          } else {
            pr[opl] = nullptr;
          }
        }
      }

      if (!opl.empty()) {
        if (has_long) {
          if (pr[opl] != nullptr && !ops.empty()) {
            pr[ops] = new ParseItem(std::move(pr[opl]->val()));
          } else if (pr[opl] == nullptr && !def.empty()) {
            if (!ops.empty()) pr[ops] = new ParseItem(std::move(def));
            pr[opl] = new ParseItem(std::move(def));
          } else {
            pr[ops] = nullptr;
          }
        }
      }

      if (!has_long && !has_short && !def.empty()) {
        if (!opl.empty()) pr[opl] = new ParseItem(std::move(def));
        if (!ops.empty()) pr[ops] = new ParseItem(std::move(def));
      }
    }  // for
  }    // if
}

// class Row

Row::Row() : require_value(true) {}

// class Subroutine

Subroutine::Subroutine() : first_line_("") {}

Subroutine::Subroutine(const char* name, const char* description)
    : first_line_(""), description_(description), name_(name) {
  usages_.reserve(5);
}

void Subroutine::print_with_row(std::ostream& out) {
  // print the subroutine name and its description
  if (strcmp(get_first_line(), "") != 0) {
    // print the first line
    out << get_first_line();
    if (!usages_.empty()) {
      out << std::endl;
    }
  }
  auto begin = usages_.begin();
  auto end = usages_.end();

  std::vector<std::string> row_list;
  row_list.reserve(usages_.size());

  // build usage rows without description field,
  // find the max-len row at the same time.
  size_t max_len = 0;
  std::for_each(begin, end, [&max_len, &row_list](const Row& row) {
    std::stringstream ss;
    ss << "  ";
    if (!row.oshort().empty()) {
      ss << "-" << row.oshort() << " ";
    }
    if (!row.olong().empty()) {
      if (!row.oshort().empty())
        ss << "[ --" << row.olong() << " ] ";
      else
        ss << "--" << row.olong() << " ";
    }

    if (row.required()) {
      ss << "arg ";
      if (!row.value().empty()) {
        ss << "= " << row.value() << " ";
      }
    }

    max_len = std::max(max_len, ss.str().size());
    row_list.push_back(std::move(ss.str()));
  });

  // show all rows and align description field
  size_t row_count = usages_.size();
  for (size_t i = 0; i < row_count; ++i) {
    std::string str_row(std::move(row_list[i]));
    // print row without description
    out << str_row;
    // print spaces
    size_t spaces = 0;
    size_t len = str_row.size();
    if (max_len > len) spaces = max_len - len;

    while (spaces--) {
      out << " ";
    }
    // print description
    out << usages_.at(i).desc() << std::endl;
  }
}

void Subroutine::print_with_template(std::ostream& out) {
  for (auto usage : usages_) {
    size_t i = 0;
    for (auto t = template_str_.begin(); t != template_str_.end(); ++t) {
      if (*t == '%') {
        switch (*(order_.begin() + i)) {
          case Row::kShort:
            out << usage.oshort();
            break;
          case Row::kLong:
            out << usage.olong();
            break;
          case Row::kDefault:
            out << usage.value();
            break;
          case Row::kDescription:
            out << usage.desc();
            break;
          default:
            break;
        }
        ++i;
      } else {
        out << *t;
      }  // if %
    }    // for template_str_
    out << std::endl;
  }  // for usages_
}

std::ostream& operator<<(std::ostream& out, Subroutine& subroutine) {
  if (subroutine.template_str_.empty()) {
    subroutine.print_with_row(out);
  } else {
    subroutine.print_with_template(out);
  }
  return out;
}
}