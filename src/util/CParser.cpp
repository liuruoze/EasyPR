#include <iostream>
#include <stdarg.h>
#include "../include/CParser.h"

using namespace easypr;
using namespace std;

// class OptionError

OptionError::OptionError(const string &msg)
    : _msg(msg)
{

}

const char *
OptionError::what() const throw()
{
    string msg;
    msg.append("Command line parse error: ").append(_msg).push_back('.');
    return msg.c_str();
}

OptionError::~OptionError() throw()
{

}


// class CCParseItem

CParseItem::CParseItem(const string &val)
    : _val(val)
{

}

CParseItem *
CParser::get(const string &key)
{
    if (_pr->count(key)) {
        return (*_pr)[key];
    }
    return NULL;
}

string
CParseItem::val() const
{
    return _val;
}


// class CParser

CParser::CParser(int argc, char *argv[])
    : _pr(NULL)
{
    _args.reserve(argc);

    for (int i = 0; i < argc; ++i) {
        _args.push_back(argv[i]);
    }
    // to make it easier, parse automatically
    this->parse();
}

ParseResult *
CParser::parse()
{
    if (_pr) {
        return _pr;
    }

    ArgList::iterator ibegin = _args.begin() + 1; // ignore the first cmd name
    ArgList::iterator iend   = _args.end();
    ArgList::iterator it     = ibegin;

    _pr = new ParseResult;

    string block;
    string previous(*ibegin);

    for ( ; it != iend; ++it) {
        block.assign(*it);

        switch (block.size()) {
        case 1:
            if (block == "-") {
                throw OptionError("single '-' is not allowed");
            }
            break;
        case 2:
            if (block[0] == '-' ) {
                if (block[1] == '-') {
                    throw OptionError("option '--' is incomplete");
                } else {
                    // single option
                    // etc: ./exec -s
                    (*_pr)[block.substr(1)] = NULL;
                }
            }
            break;
        default: // >=3
            if (block[0] == '-') {
                if (block[1] == '-') {
                    // a long format option
                    // etc: ./exec --option
                    (*_pr)[block.substr(2)] = NULL;
                } else {
                    // a conbination options
                    // etc: ./exec -ab[...]
                    string::iterator tbegin = block.begin() + 1; // ignore the first '-'
                    string::iterator tend   = block.end();
                    string::iterator t      = tbegin;

                    for (; t != tend; ++t) {
                        string key;
                        key.push_back(*t);
                        (*_pr)[key] = NULL;
                    }
                }
            }
            break;
        }// switch

        if (block[0] != '-'
            && previous != block //not the first option
           ) {
            // it's the value of previous option.
            // etc: ./exec -o   [...]
            // etc: ./exec -opq [...]

            if (previous[0] != '-') {
                // previous is not an option, error occur
                // etc: ./exec abc def
                throw OptionError("'" + block + "' is not allowed here");
            }

            string key;
            key.push_back(*(previous.end() - 1));

            if (_pr->count(key)) {
                (*_pr)[key] = new CParseItem(block);
            }
        }

        previous = block;
    }// for

    return _pr;
}

bool
CParser::has(const char *key)
{
    string skey(key);

    if (_pr && !skey.empty()) {
        if (skey[0] == '-') {
            //check conbination options, etc: CParser::has("-xyz")
            for (size_t i = 1; i < skey.size(); ++i) {
                string tkey;
                tkey.push_back(skey[i]);
                if (!_pr->count(tkey)) {
                    return false;
                }
            }
            return true;
        } else {
            // check single option, etc: CParser::has("x")
            return _pr->count(skey);
        }
    }
    return false;
}

bool
CParser::has_or(int n, ...)
{
    va_list keys;
    va_start(keys, n);
    while (n--) {
        const char *key = va_arg(keys, const char *);
        if (this->has(key)) {
            return true;
        }
    }
    va_end(keys);
    return false;
}

bool
CParser::has_and(int n, ...)
{
    va_list keys;
    va_start(keys, n);
    while (n--) {
        const char *key = va_arg(keys, const char *);
        if (!this->has(key)) {
            return false;
        }
    }
    va_end(keys);
    return true;
}

void
CParser::dump()
{
    if (_pr) {
        ParseResult::iterator ibegin = _pr->begin();
        ParseResult::iterator iend   = _pr->end();
        ParseResult::iterator it     = ibegin;
        for (; it != iend; ++it) {
            cout << it->first;
            if (it->second) {
                cout << " = " << it->second->val() << endl;
            } else {
                cout << " set" << endl;
            }
        }
    }
}

CParser::~CParser()
{
    if (_pr) {
        ParseResult::iterator ibegin = _pr->begin();
        ParseResult::iterator iend   = _pr->end();
        ParseResult::iterator it     = ibegin;
        for (; it != iend; ++it) {
            CParseItem *item = it->second;
            if (item) {
                delete item;
                item = NULL;
            }
        }
        delete _pr;
        _pr = NULL;
    }
}
