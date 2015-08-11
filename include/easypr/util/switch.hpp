#ifndef PROGRAM_OPTIONS_SWITCH_HPP_
#define PROGRAM_OPTIONS_SWITCH_HPP_

#include <functional>
#include <map>

namespace program_options {

/**
* @brief The Switch template class.
* @param Ty The target type.
*/
template <typename Ty>
class Switch {
 public:
  typedef std::function<void(void)> Scope;
  typedef std::map<const Ty, Scope> Reflections;

  Switch() : has_others_scope_(false) {}

  explicit Switch(const Ty& target)
      : target_(target), has_others_scope_(false) {}

  /**
   * @brief Create a case block with an expression and a callback function.
   * @param _case The case expression, variable is allowed.
   * @param callback The callback function, can be a lambda expression.
   * @return The current Switch instance.
   */
  Switch& found(const Ty& _case, const Scope& callback) {
    reflections_[_case] = callback;
    return *this;
  }

  /**
   * @brief Create a default block with a callback function,
   *        if no cases matched, this block will be called.
   * @param callback
   */
  inline void others(const Scope& callback) {
    has_others_scope_ = true;
    others_ = callback;
    this->done();
  }

  /**
   * @brief Finish the cases,
   * others() will call this method automatically.
   */
  inline void done() {
    auto kv = reflections_.find(target_);
    if (kv != reflections_.end()) {
      // found
      auto scope = kv->second;
      scope();
    } else if (has_others_scope_) {
      // not found, call others
      others_();
    }
  }

  /**
   * @brief Combine the cases to this Switch from another Switch.
   *        Note that this two Switch should be the same template.
   * @param _switch Another Switch instance.
   * @return
   */
  inline Switch& combine(const Switch& _switch) {
    for (auto kv : _switch.reflections()) {
      this->reflections_[kv.first] = kv.second;
    }
    return *this;
  }

  /**
   * @brief Return the case-callback pairs.
   * @return
   */
  inline Reflections reflections() const { return reflections_; }

 private:
  const Ty& target_;
  bool has_others_scope_;
  Scope others_;
  Reflections reflections_;
};

/**
 * @brief Define which expression does the Switch match.
 * @param expression
 * @return
 */
template <typename Ty>
Switch<Ty> select(const Ty& expression) {
  return Switch<Ty>(expression);
}
}

#endif  // PROGRAM_OPTIONS_SWITCH_HPP_
