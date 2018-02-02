### Class:
 - [Class Row](#Row)
 - [Class Subroutine](#Subroutine)
 - [Class Generator](#Generator)

<a id="Row"></a>
### Class Row

#### Properties
 - `(private)`:
   - [`bool require_value`](#require_value)
   - [`std::string option_short`](#option_short)
   - [`std::string option_long`](#option_long)
   - [`std::string default_value`](#default_value)
   - [`std::string description`](#description)
 - `(public)`:
   - [`enum Field { kShort, kLong, kDefault, kDescription }`](#Field)
   - [`typedef std::initializer_list<Field> Order`](#Order)

#### Functions
 - `(public)`:
   - [`Row()`](#Row)
   - [`inline std::string oshort() const { return option_short; }`](#oshort_get)
   - [`inline std::string olong() const { return option_long; }`](#olong_get)
   - [`inline std::string value() const { return default_value; }`](#value_get)
   - [`inline std::string desc() const { return description; }`](#desc_get)
   - [`inline bool required() const { return require_value; }`](#required_get)
   - [`inline void oshort(const std::string& oshort) { option_short = oshort; }`](#oshort_set)
   - [`inline void olong(const std::string& olong) { option_long = olong; }`](#olong_set)
   - [`inline void value(const std::string& value) { default_value = value; }`](#value_set)
   - [`inline void desc(const std::string& desc) { description = desc; }`](#desc_set)
   - [`inline void required(bool required) { require_value = required; }`](#required_set)
***
 -
***

<a id="Subroutine"></a>
### Class Subroutine

#### Properties
 - `(public)`:
   - [`typedef std::vector<Row> Usages`](#Usages)
   - [`typedef std::initializer_list<const char*> TemplateValue`](#TemplateValue)
   - [`typedef std::vector<TemplateValue> TemplateValues`](#TemplateValues)
 - `(private)`:
   - [`Usages usages_`](#usages_)
   - [`TemplateValues templates_`](#templates_)
   - [`const char* first_line_`](#first_line_)
   - [`const char* description_`](#description_)
   - [`std::string name_`](#name_)
   - [`std::string template_str_`](#template_str_)
   - [`Row::Order order_`](#order_)

#### Functions
 - `(public)`:
   - [`Subroutine()`](#Subroutine1)
   - [`Subroutine(const char* name, const char* description)`](#Subroutine2)
   - [`inline void add_usage_line(const Row& row) { usages_.push_back(row); }`](#add_usage_line1)
   - [`inline void add_usage_line(const TemplateValue& row) {templates_.push_back(row);}`](#add_usage_line2)
   - [`inline void set_first_line(const char* line) { first_line_ = line; }`](#set_first_line)
   - [`inline void set_description(const char* desc) { description_ = desc; }`](#set_description)
   - [`inline void set_template(const char* tstr, const Row::Order& order)`](#set_template)
   - [`inline std::string to_string()`](#to_string)
   - [`inline std::string get_name() const { return name_; }`](#get_name)
   - [`inline const char* get_description() const { return description_; }`](#get_description)
   - [`inline const char* get_first_line() const { return first_line_; }`](#get_first_line)
   - [`inline Usages::iterator begin() { return usages_.begin(); }`](#begin)
   - [`inline Usages::iterator end() { return usages_.end(); }`](#end)
   - [`inline size_t size() { return usages_.size(); }`](#size)
   - [`inline Row& at(size_t i) { return usages_.at(i); }`](#at)
   - [`inline const Usages& get_usage() const { return usages_; }`](#get_usage)
   - [`inline static const char* get_default_name() { return "EmptySubroutine"; }`](#get_default_name)
 - `(private)`:
   - [`friend std::ostream& operator<<(std::ostream& out, Subroutine& subroutine)`](#operator_subroutine)
   - [`void print_with_row(std::ostream& out)`](#print_with_row)
   - [`void print_with_template(std::ostream& out)`](#print_with_template)
***

***
<a id="Subroutine1"></a>
#### Subroutine()

<a id="Subroutine2"></a>
#### Subroutine(const char* name, const char* description)
 - //need to confirm

<a id="Generator"></a>
### Class Generator

#### Properties
 - [`typedef std::map<std::string, Subroutine*> SubroutineCollection`](#SubroutineCollection)
 - [`(private)const char kDelimiter`](#kDelimiter)
 - [`(private)SubroutineCollection subroutines_`](#subroutines)
 - [`(private)std::string current_subroutine_`](#current_subroutine_)
 - [`(private)Parser* parser_`](#parser_)

#### Functions
 - (`public:`)
   - [`Generator()`](#Generator)
   - [`~Generator()`](#~Generator)
   - [`Generator& make_usage(const char* first_line)`](#make_usage)
   - [`Parser* make_parser()`](#make_parser)
   - [`Generator& add_subroutine(const char* name)`](#add_subroutine1`)
   - [`Generator& add_subroutine(const char* name, const char* description)`](#add_subroutine2)
   - [`std::map<std::string, std::string> get_subroutine_list()`](#get_subroutine_list)
   - [`inline std::string to_string()`](#to_string)
   - [`inline Generator& operator()(const char* option, const char* description)`](#operator1)
   - [`inline Generator& operator()(const char* option, const char* default_value,const char* description)`](#operator2)
   - [`inline Subroutine& operator()(const char* name)`](#operator3)
   - [`inline Generator& make_template(const char* template_str,const Row::Order& order)`](#operator4)
 - (`private:`)
   - [`(private)inline Subroutine* get_subroutine()`](#get_subroutine)
   - [`(private)friend std::ostream& operator<<(std::ostream& out, Generator& generator)`](#operator5)
   - [`(private)bool add_usage_line(const char* option,const char* default_value,const char* description)`](#add_usage_line)

***
<a id="SubroutineCollection"></a>
#### typedef std::map<std::string, Subroutine*> SubroutineCollection
 - 用于存储`[string:Subroutine]`的`map`:`SubroutineCollection`

<a id="subroutines"></a>
#### (private)SubroutineCollection subroutines_
 - `SubroutineCollection`类型,用来存储`Subroutine`集合

<a id="current_subroutine_"></a>
#### (private)std::string current_subroutine_
 - 表示当前子程序

<a id="parser_"></a>
#### (private)Parser* parser_
 - 在构造方法中被初始化为`nullptr`

***
<a id="Generator"></a>
#### Generator()
 - 向`subroutines_`添加一条["EmptySubroutine",new Subroutine("EmptySubroutine","")]

<a id="make_usage"></a>
#### Generator& make_usage(const char* first_line)
 - 获取当前`current_subroutine_`在`subroutines_`中对应的`Subroutine`
 - 将上述的`Subroutine`的属性`first_line`设置成输入的参数`first_line`
 - 返回`*this`
<a id="add_subroutine1"></a>
#### Generator& add_subroutine(const char* name)
 - 执行`add_subroutine(name, "")`
 - 执行`return *this`,返回当前`Generator`

<a id="add_subroutine2"></a>
#### Generator& add_subroutine(const char* name, const char* description)
 - 查找`subroutines_`中有没有子程序`name`
 - 如果有,则`return *this`
 - 如果没有,则将`[name:new Subroutine(name,description)]`添加到`subroutines_`中

<a id="get_subroutine"></a>
#### inline Subroutine* get_subroutine(){return subroutines_.at(current_subroutine_);}
 - 输入`current_subroutine_`
 - 返回`subroutines`中`current_subroutine_`对应的`Subroutine`,即当前指令
