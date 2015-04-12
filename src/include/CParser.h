#ifndef __CPARSER_H__
#define __CPARSER_H__

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

// CParser can be used for parsing command line. It stores options and values to a map.
// It's easy to check whether an option exist, get value with a specified type.

namespace easypr {
    
    // class OptionError
    
    class OptionError : public std::exception
    {
    public:
        
        explicit OptionError(const std::string &msg);
        
        const char *what() const throw();
        
        ~OptionError() throw();
        
    private:
        std::string _msg;
    };
    
    // class CParseItem
    
    class CParseItem
    {
    public:
        
        explicit CParseItem(const std::string &val);
        
        /*
         * dynamic type cast, support base data types including std::string
         * return T
         */
        template<typename T> T as()
        {
            T r;
            std::stringstream buf;
            buf << _val;
            buf >> r;
            return r;
        }
        
        /*
         * alias of as<std::string>()
         * return std::string
         */
        std::string val() const;
        
    private:
        std::string _val;
    };
    
    // class CParser
    
    typedef std::vector<std::string>            ArgList;
    typedef std::map<std::string, CParseItem *> ParseResult;
    
    class CParser
    {
    public:
        
        explicit CParser(int argc, char *argv[]);
        
        /*
         * parse the cmd line
         * return ParseResult*
         */
        ParseResult *parse();
        
        /*
         * check whether a cretain option exist
         * return bool
         */
        bool has(const char *key);
        
        /*
         * check whether a sequence of options exist
         * example: has_or(3, "he", "or", "she");
         * return bool
         */
        bool has_or(int n, ...);
        
        /*
         * check whether a sequence of options exist
         * example: has_and(3, "he", "and", "she");
         * return bool
         */
        bool has_and(int n, ...);
        
        /*
         * get the certain option's value
         * return ParseItem*
         */
        CParseItem *get(const std::string &key);
        
        /*
         * output all ParseResult
         */
        void dump();
        
        /*
         * release memory allocated in parse()
         */
        ~CParser();
        
    private:
        ArgList     _args;
        ParseResult *_pr;
    };
    
}

#endif // __CPARSER_H__
