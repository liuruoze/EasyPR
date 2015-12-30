/***********************
 * This file is a packed library of
 * ProgramOptions(http://github.com/Micooz/ProgramOptions).
 * ProgramOptions is a library for generating and parsing command line.
 ***********************/

#ifndef UTIL_PROGRAM_OPTIONS_H
#define UTIL_PROGRAM_OPTIONS_H

#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <initializer_list>
#include <stdarg.h>

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#endif

namespace program_options {

class Row {
 public:
  Row();

  enum Field { kShort, kLong, kDefault, kDescription };

  typedef std::initializer_list<Field> Order;

  // getter

  inline std::string oshort() const { return option_short; }

  inline std::string olong() const { return option_long; }

  inline std::string value() const { return default_value; }

  inline std::string desc() const { return description; }

  inline bool required() const { return require_value; }

  // setter

  inline void oshort(const std::string& oshort) { option_short = oshort; }

  inline void olong(const std::string& olong) { option_long = olong; }

  inline void value(const std::string& value) { default_value = value; }

  inline void desc(const std::string& desc) { description = desc; }

  inline void required(bool required) { require_value = required; }

 private:
  bool require_value;
  std::string option_short;
  std::string option_long;
  std::string default_value;
  std::string description;
};

class Subroutine {
 public:
  typedef std::vector<Row> Usages;
  typedef std::initializer_list<const char*> TemplateValue;
  typedef std::vector<TemplateValue> TemplateValues;

  Subroutine();

  Subroutine(const char* name, const char* description);

  inline void add_usage_line(const Row& row) { usages_.push_back(row); }

  inline void add_usage_line(const TemplateValue& row) {
    templates_.push_back(row);
  }

  inline void set_first_line(const char* line) { first_line_ = line; }

  inline void set_description(const char* desc) { description_ = desc; }

  inline void set_template(const char* tstr, const Row::Order& order) {
    template_str_ = tstr;
    order_ = order;
  }

  // getters

  inline std::string to_string() {
    std::stringstream ss;
    ss << *this;
    return std::move(ss.str());
  }

  inline std::string get_name() const { return name_; }

  inline const char* get_description() const { return description_; }

  inline const char* get_first_line() const { return first_line_; }

  inline Usages::iterator begin() { return usages_.begin(); }

  inline Usages::iterator end() { return usages_.end(); }

  inline size_t size() { return usages_.size(); }

  inline Row& at(size_t i) { return usages_.at(i); }

  inline const Usages& get_usage() const { return usages_; }

  inline static const char* get_default_name() { return "EmptySubroutine"; }

 private:
  friend std::ostream& operator<<(std::ostream& out, Subroutine& subroutine);

  void print_with_row(std::ostream& out);

  void print_with_template(std::ostream& out);

  Usages usages_;
  TemplateValues templates_;
  const char* first_line_;
  const char* description_;
  std::string name_;
  std::string template_str_;
  Row::Order order_;
};

class Parser;

class Generator {
 public:
  typedef std::map<std::string, Subroutine*> SubroutineCollection;

  Generator();

  ~Generator();

  // begin a new usage generate process and
  // create the first line that you will see in the result.
  // if you don't want to create the first line,
  // it's ok to create your usage lines without calling make_usage() first.
  Generator& make_usage(const char* first_line);

  Parser* make_parser();

  // generate usage lines for a subroutine.
  Generator& add_subroutine(const char* name);

  Generator& add_subroutine(const char* name, const char* description);

  // get pairs of subroutine name and its description.
  std::map<std::string, std::string> get_subroutine_list();

  // generate and return the result as std::string
  inline std::string to_string() {
    std::stringstream ss;
    ss << *this;
    return std::move(ss.str());
  }

  // add an usage line, it doesn't require a value
  inline Generator& operator()(const char* option, const char* description) {
    this->add_usage_line(option, "", description);
    (get_subroutine()->end() - 1)->required(false);
    return *this;
  }

  // add an usage line, it requires a value(may has a default value)
  inline Generator& operator()(const char* option, const char* default_value,
                               const char* description) {
    this->add_usage_line(option, default_value, description);
    return *this;
  }

  // this method is for subroutine usage printing.
  // e.g., cout << generator("subroutine_name");
  inline Subroutine& operator()(const char* name) {
    assert(subroutines_.find(name) != subroutines_.end());
    return *subroutines_.at(name);
  }

  inline Generator& make_template(const char* template_str,
                                  const Row::Order& order) {
    get_subroutine()->set_template(template_str, order);
    return *this;
  }

 private:
  inline Subroutine* get_subroutine() {
    return subroutines_.at(current_subroutine_);
  }

  friend std::ostream& operator<<(std::ostream& out, Generator& generator);

  // add a usage line.
  // if delimiter doesn't exist, return false
  bool add_usage_line(const char* option, const char* default_value,
                      const char* description);

  const char kDelimiter = ',';
  SubroutineCollection subroutines_;
  std::string current_subroutine_;
  Parser* parser_;
};

class ParseError : public std::exception {
 public:
  explicit ParseError(const std::string& msg);

  const char* what() const throw();

  ~ParseError() throw();

 private:
  std::string _msg;
};

class ParseItem {
 public:
  ParseItem(const std::string& value);

  /*
   * dynamic type cast, support base data types including std::string
   */
  template <typename T>
  T as() {
    T r;
    std::stringstream buf;
    buf << value_;
    buf >> r;
    return r;
  }

  /*
   * alias of as<std::string>()
   */
  inline std::string val() const { return value_; }

  /*
   * get C string directly
   */
  inline const char* c_str() const { return value_.c_str(); }

 private:
  std::string value_;
};

class Parser {
 public:
  typedef std::vector<std::string> ParameterList;
  typedef std::map<std::string, ParseItem*> ParseResult;

  Parser();

  /*
  * release memory allocated in parse()
  */
  ~Parser();

  /*
   * parse the command line by given argc and argv
   */
  ParseResult* parse(const int argc, const char** argv);

  /*
   * parse the command line by given command string
   */
  ParseResult* parse(const char* command_line);

  /*
   * check whether a certain option exist
   */
  bool has(const char* key);

  /*
   * check whether a sequence of options exist
   * example: has_or(3, "he", "or", "she");
   */
  DEPRECATED(bool has_or(int n, ...)) {
    va_list keys;
    va_start(keys, n);
    while (n--) {
      const char* key = va_arg(keys, const char*);
      if (this->has(key)) {
        return true;
      }
    }
    va_end(keys);
    return false;
  }

  /*
   * check whether a sequence of options exist
   * example: has_and(3, "he", "and", "she");
   */
  DEPRECATED(bool has_and(int n, ...)) {
    va_list keys;
    va_start(keys, n);
    while (n--) {
      const char* key = va_arg(keys, const char*);
      if (!this->has(key)) {
        return false;
      }
    }
    va_end(keys);
    return true;
  }

  /*
   * check whether a sequence of options exist using std::initializer_list
   * example: has_or({"he", "or", "she"});
   */
  bool has_or(std::initializer_list<const char*> options);

  /*
   * check whether a sequence of options exist using std::initializer_list
   * example: has_and({"he", "and", "she"});
   */
  bool has_and(std::initializer_list<const char*> options);

  /*
   * get the specified option value
   */
  ParseItem* get(const std::string& key);

  inline void set_usage_subroutines(
      const Generator::SubroutineCollection* subroutines) {
    subroutines_ = subroutines;
  }

  inline std::string get_subroutine_name() const { return subroutine_name_; }

 private:
  bool init(const int argc, const char** argv);

  void cleanup();

  void set_addition();

  const Generator::SubroutineCollection* subroutines_;
  std::string subroutine_name_;
  int argc_;
  ParameterList args_;
  ParseResult* pr_;
};
}

#endif  // UTIL_PROGRAM_OPTIONS_H
