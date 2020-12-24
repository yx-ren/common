#ifndef __COMMON_BASE_CONFIG_PARSER_H__
#define __COMMON_BASE_CONFIG_PARSER_H__

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/assert.hpp>
#include <boost/any.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/noncopyable.hpp>
#include <boost/spirit/include/classic.hpp>

#include <common/base/common.h>
#include <common/base/logger/LoggerManager.h>
#include <common/base/Convertor.h>

#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <string>


using boost::property_tree::wptree;
using namespace BOOST_SPIRIT_CLASSIC_NS;

CBASE_BEGIN
typedef std::pair<std::wstring, std::wstring> wstr2wstr_tuple;
typedef std::map<std::wstring, std::wstring> wstr2wstr_map;
typedef std::pair<std::string, std::string> str2str_tuple;
typedef std::map<std::string, std::string> str2str_map;
typedef std::list<wstr2wstr_map> wstr2wstr_maplist;
typedef std::list<str2str_map> str2str_maplist;

class PathNode
{
public:
    PathNode()
    {

    }

    void setName(const wchar_t *begin, const wchar_t *end)
    {
        BOOST_ASSERT(mName.empty());

        mName.assign(begin, end);
    }

    void setAttribute(const wchar_t *begin, const wchar_t *end)
    {
        BOOST_ASSERT(mAttribute.empty());
        mAttribute.assign(begin, end);
    }

    void setValue(const wchar_t *begin, const wchar_t *end)
    {
        BOOST_ASSERT(mValue.empty());
        mValue.assign(begin, end);
    }

    bool isSimple() const
    {
        return isValid() && !isAttribute();
    }

    bool isAttribute() const
    {
        return (isValid() && !mAttribute.empty());
    }

    bool isValid() const
    {
        if (mName.empty())
        {
            CB_ERROR("invalid node name.");
            return false;
        }
        if ((mAttribute.empty() && !mValue.empty())
            || (!mAttribute.empty() && mValue.empty()))
        {
            CB_ERROR("invalid attribute:value peer.");
            return false;
        }
        return true;
    }

    const std::wstring &name() const
    {
        BOOST_VERIFY(!mName.empty());
        return mName;
    }

    const std::wstring &attribute() const
    {
        BOOST_VERIFY(!mAttribute.empty());
        return mAttribute;
    }

    const std::wstring &value() const
    {
        BOOST_VERIFY(!mValue.empty());
        return mValue;
    }

private:
    std::wstring mName;
    std::wstring mAttribute;
    std::wstring mValue;
};

typedef sp<PathNode> PathNodePtr;

class ParsedPath
{
public:
    ParsedPath()
    {

    }

    void setNodeName(const wchar_t *begin, const wchar_t *end)
    {
        //std::string s(begin, end);
        //std::cout << "a new node: " << s << std::endl;

        mCurrNode.reset(new PathNode);
        mCurrNode->setName(begin, end);
    }

    void setNodeAttribute(const wchar_t *begin, const wchar_t *end)
    {
        //std::string s(begin, end);
        //std::cout << "a new attribute: " << s << std::endl;
        BOOST_ASSERT(mCurrNode);
        mCurrNode->setAttribute(begin, end);
    }

    void setNodeValue(const wchar_t *begin, const wchar_t *end)
    {
        //std::string s(begin, end);
        //std::cout << "attribute value: " << s << std::endl;
        BOOST_ASSERT(mCurrNode);
        mCurrNode->setValue(begin, end);
    }

    void finishNode(const wchar_t *begin, const wchar_t *end)
    {
        //std::cout << "a node is parsed." << std::endl;
        BOOST_ASSERT(mCurrNode);
        BOOST_ASSERT(mCurrNode->isValid());
        mNodes.push_back(mCurrNode);
    }

    const std::list<PathNodePtr> &getNodes() const
    {
        return mNodes;
    }

private:
    std::list<PathNodePtr> mNodes;
    PathNodePtr            mCurrNode;
};

struct SimplePathParser : public grammar<SimplePathParser>
{
    template <typename ScannerT>
        struct definition
        {
            definition(const SimplePathParser &parser)
                {
                    ParsedPath *parsed_path = const_cast<ParsedPath *>(&(parser.parsed_path));
                    strict_char_range = range_p<wchar_t>(L'a', L'z') | range_p<wchar_t>(L'A', L'Z') | ch_p<wchar_t>(L'_') | ch_p<wchar_t>(L'-') | ch_p<wchar_t>(L':');
                    //loose_char_range = strict_char_range | range_p<wchar_t>(L'0', L'9');
                    loose_char_range = anychar_p - (ch_p<wchar_t>(L'[') | ch_p<wchar_t>(L'=') | ch_p<wchar_t>(L']') | ch_p<wchar_t>(L'.'));

                    //identifier = strict_char_range >> *(loose_char_range);
                    identifier = loose_char_range >> *(loose_char_range);
                    name = identifier;
                    attribute = identifier;
                    value = ch_p<wchar_t>(L'"') >> (*(print_p & (~ch_p<wchar_t>(L'"'))))[boost::bind(&ParsedPath::setNodeValue, parsed_path, boost::lambda::_1, boost::lambda::_2)] >> ch_p<wchar_t>(L'"');

                    attribute_node = name[boost::bind(&ParsedPath::setNodeName, parsed_path, boost::lambda::_1, boost::lambda::_2)]
                        >> ch_p<wchar_t>(L'[')
                        >> attribute[boost::bind(&ParsedPath::setNodeAttribute, parsed_path, boost::lambda::_1, boost::lambda::_2)]
                        >> ch_p<wchar_t>(L'=')
                        >> value
                        >> ch_p<wchar_t>(L']');

                    simple_node = name[boost::bind(&ParsedPath::setNodeName, parsed_path, boost::lambda::_1, boost::lambda::_2)];

                    node = attribute_node[boost::bind(&ParsedPath::finishNode, parsed_path, boost::lambda::_1, boost::lambda::_2)]
                        | simple_node[boost::bind(&ParsedPath::finishNode, parsed_path, boost::lambda::_1, boost::lambda::_2)];

                    path = node >> *(ch_p<wchar_t>(L'.') >> node);
                }

            rule<ScannerT> strict_char_range, loose_char_range, identifier, name, attribute, value, attribute_node, simple_node, node, path;

            const rule<ScannerT> &start() const
                {
                    return path;
                }

        };

    const std::list<PathNodePtr> &getParsedNodes() const
    {
        return parsed_path.getNodes();
    }

    ParsedPath parsed_path;
};


class ConfigParser : private boost::noncopyable
{
public:
    ConfigParser(const std::wstring &filename);
    ConfigParser(const std::string &filename);
    ConfigParser(const std::string &xmlString, const std::string &saveFilename);
    ConfigParser(const wptree &wp);
    ~ConfigParser();

    //ptree get(const std::string &path);
    const wptree &get(const std::string &path) const;
    const wptree &get(const std::wstring &path) const;

    template <typename T>
        T getValue(const std::string &path, const T &default_value) const;
    template <typename T>
        T getValue(const std::wstring &path, const T &default_value) const;

    template <typename T>
        T getValue(const std::string &path) const;
    template <typename T>
        T getValue(const std::wstring &path) const;

    template <typename T>
        std::list<T> getValueList(const std::string &path) const;
    template <typename T>
        std::list<T> getValueList(const std::wstring &path) const;

    template <typename T>
        bool setValue(const std::string &path, const T &value);
    template <typename T>
        bool setValue(const std::wstring &path, const T &value);

    template <typename T>
        bool setValue(const std::string &path, const T &value, const T &old_value);
    template <typename T>
        bool setValue(const std::wstring &path, const T &value, const T &old_value);

    template <typename T>
        bool setValueList(const std::string &path, const std::list<T> &value_list);
    template <typename T>
        bool setValueList(const std::wstring &path, const std::list<T> &value_list);

    template <typename T>
        bool setValueList(const std::string &path, const std::list<T> &value_list, const std::list<T> &old_value_list);
    template <typename T>
        bool setValueList(const std::wstring &path, const std::list<T> &value_list, const std::list<T> &old_value_list);

    std::list<std::string> getAttrList(const std::string &path, const std::string &attrName) const;
    std::list<std::string> getAttrList(const std::wstring &path, const std::wstring &attrName) const;

    bool save() const;

    bool save(const std::string &filename) const;
    bool save(const std::wstring &filename) const;

    bool save(std::wostream &os) const;

    bool isNodeExist(const std::string &nodePath);

private:
    wptree &getUnsafe(const std::wstring &path, bool create = false);
    const wptree &getUnsafe(const std::wstring &path) const;

private:
    mutable rwmutex     mLock;
    const std::wstring mFilename;
    wptree mConfigTree;
};

template <typename T>
T ConfigParser::getValue(const std::string &path, const T& default_value) const
{
    return getValue<T>(Convertor::toWstring(path), default_value);
}

template <typename T>
T ConfigParser::getValue(const std::wstring &path, const T& default_value) const
{
    T result = default_value;
    try
    {
        result = getValue<T>(path);
    }
    catch (...)
    {
#ifdef DEBUG
        std::cout << "failed to get value from the configuration file, use the default one" << std::endl;
#endif
    }

    return result;
}

template <typename T>
T ConfigParser::getValue(const std::string &path) const
{
    return getValue<T>(Convertor::toWstring(path));
}
template <typename T>
T ConfigParser::getValue(const std::wstring &path) const
{
    read_lock rl(mLock);
    const wptree &node = getUnsafe(path);
    return node.get_value<T>();
}
template <>
inline std::string ConfigParser::getValue<std::string>(const std::string &path) const
{
    std::wstring wvalue = getValue<std::wstring>(path);
    return Convertor::toString(wvalue);
}
template <>
inline std::string ConfigParser::getValue<std::string>(const std::wstring &path) const
{
    std::wstring wvalue = getValue<std::wstring>(path);
    return Convertor::toString(wvalue);
}
/*
  template <>
  inline wstr2wstr_map ConfigParser::getValue<wstr2wstr_map>(const std::string &path) const
  {
  std::wstring wpath(path.begin(), path.end());
  return getValue<wstr2wstr_map>(wpath);
  }
*/
static inline str2str_map mapwstr2str(const wstr2wstr_map &wmap)
{
    str2str_map result;

    BOOST_FOREACH(const wstr2wstr_tuple &wtuple, wmap)
    {
        result.insert(std::make_pair(Convertor::toString(wtuple.first), Convertor::toString(wtuple.second)));
    }
    return result;
}

static inline wstr2wstr_map mapstr2wstr(const str2str_map &map)
{
    wstr2wstr_map result;

    BOOST_FOREACH(const str2str_tuple &tuple, map)
    {
        result.insert(std::make_pair(Convertor::toWstring(tuple.first), Convertor::toWstring(tuple.second)));
    }
    return result;
}


template <>
inline wstr2wstr_map ConfigParser::getValue<wstr2wstr_map>(const std::wstring &path) const
{

    wstr2wstr_map result;

    read_lock rl(mLock);

    const wptree &node = getUnsafe(path);


    BOOST_FOREACH( const wptree::value_type &v, node)
    {
        result[v.first] = v.second.get_value<std::wstring>();
    }

    return result;
}

template <>
inline str2str_map ConfigParser::getValue<str2str_map>(const std::string &path) const
{
    wstr2wstr_map wresult = getValue<wstr2wstr_map>(path);
    return mapwstr2str(wresult);
}

template <>
inline str2str_map ConfigParser::getValue<str2str_map>(const std::wstring &path) const
{
    wstr2wstr_map wresult = getValue<wstr2wstr_map>(path);
    return mapwstr2str(wresult);
}

template <typename T>
std::list<T> ConfigParser::getValueList(const std::string &path) const
{
    return getValueList<T>(Convertor::toWstring(path));
}
template <typename T>
std::list<T> ConfigParser::getValueList(const std::wstring &path) const
{
    read_lock rl(mLock);
    std::list<T> result;

    std::size_t sep_found = path.rfind('.');
    if (sep_found != std::string::npos)
    {
        std::wstring abs_path(path, 0, sep_found);
        std::wstring node_name(path, ++sep_found);
        //std::cout << "abs_path: " << abs_path << std::endl;
        //std::cout << "node_name: " << node_name << std::endl;
        const wptree &node = getUnsafe(abs_path);

        BOOST_FOREACH( const wptree::value_type &v, node)
        {
            if (v.first == node_name)
            {
                result.push_back(v.second.get_value<T>());
            }
        }
    }
    else
    {
        const wptree &node = getUnsafe(path);
        result.push_back(node.get_value<T>());
    }

    return result;
}
template <>
inline	std::list<std::string> ConfigParser::getValueList<std::string>(const std::string &path) const
{
    std::list<std::wstring> wvalue_list = getValueList<std::wstring>(path);
    std::list<std::string> value_list;
    BOOST_FOREACH(const std::wstring &wvalue, wvalue_list)
    {
        value_list.push_back(Convertor::toString(wvalue));
    }
    return value_list;
}
/*
  template <>
  inline	std::list<wstr2wstr_map> ConfigParser::getValueList<wstr2wstr_map >(const std::string &path) const
  {
  std::wstring wpath;
  wpath.assign(path.begin(), path.end());
  return getValueList<wstr2wstr_map>(wpath);
  }
*/
template <>
inline	std::list<wstr2wstr_map> ConfigParser::getValueList<wstr2wstr_map >(const std::wstring &path) const
{
    wstr2wstr_maplist result;

    read_lock rl(mLock);
    std::size_t sep_found = path.rfind('.');
    if (sep_found != std::string::npos)
    {
        std::wstring abs_path(path, 0, sep_found);
        std::wstring node_name(path, ++sep_found);
        //std::cout << "abs_path: " << abs_path << std::endl;
        //std::cout << "node_name: " << node_name << std::endl;
        const wptree &node = getUnsafe(abs_path);

        BOOST_FOREACH( const wptree::value_type &v, node)
        {
            //std::cout << "for node_name: " << node_name << std::endl;
            //std::cout << "v.first: " << v.first << std::endl;
            if (v.first == node_name)
            {
                wstr2wstr_map sm;
                BOOST_FOREACH(const wptree::value_type &smv, v.second)
                {
                    sm[smv.first] = smv.second.get_value<std::wstring>();
                }
                //std::cout << "push one map" << std::endl;
                result.push_back(sm);
            }
        }
    }
    else
    {
        const wptree &node = getUnsafe(path);
        wstr2wstr_map sm;
        BOOST_FOREACH(const wptree::value_type &v, node)
        {
            sm[v.first] = v.second.get_value<std::wstring>();
        }
        result.push_back(sm);
    }

    return result;
}

template <>
inline std::list<str2str_map> ConfigParser::getValueList<str2str_map>(const std::wstring &path) const
{
    std::list<wstr2wstr_map> wresult = getValueList<wstr2wstr_map>(path);
    std::list<str2str_map> result;
    BOOST_FOREACH(const wstr2wstr_map &wmap, wresult)
    {
        result.push_back(mapwstr2str(wmap));
    }
    return result;
}

inline std::list<std::string> ConfigParser::getAttrList(const std::wstring &path, const std::wstring &attrName) const
{
    std::list<std::string> result;
    read_lock rl(mLock);

    std::size_t sep_found = path.rfind('.');
    if (sep_found != std::string::npos)
    {
        std::wstring abs_path(path, 0, sep_found);
        std::wstring node_name(path, ++sep_found);
        const wptree &node = getUnsafe(abs_path);

        BOOST_FOREACH( const wptree::value_type &v, node)
        {
            if (v.first == node_name)
            {
                const wptree *child = &(v.second);
                try
                {
                    const std::wstring &witem = child->get_child(L"<xmlattr>." + attrName).get_value<std::wstring>();
                    result.push_back(Convertor::toString(witem));
                }catch(...)
                {
                    /* if the attribute doesn't exist, create a empty value */
                    result.push_back("");
                }
            }
        }
    }
    else
    {
        const wptree &node = getUnsafe(path);
        const std::wstring &witem = node.get_child(L"<xmlattr>." + attrName).get_value<std::wstring>();
        result.push_back(Convertor::toString(witem));
    }

    return result;
}

inline std::list<std::string> ConfigParser::getAttrList(const std::string &path, const std::string &attrName) const
{
    std::wstring wpath, wattr_name;
    wpath = Convertor::toWstring(path);
    wattr_name = Convertor::toWstring(attrName);
    return getAttrList(wpath, wattr_name);
}

template <typename T>
bool ConfigParser::setValue(const std::string &path, const T &value)
{
    return setValue<T>(Convertor::toWstring(path), value);
}
template <typename T>
bool ConfigParser::setValue(const std::wstring &path, const T &value)
{
    write_lock wl(mLock);

    wptree &node = getUnsafe(path, true);
    node.put_value<T>(value);
    return true;
}
template <>
inline bool ConfigParser::setValue<std::string>(const std::string &path, const std::string &value)
{
    return setValue<std::wstring>(path, Convertor::toWstring(value));
}

template <typename T>
bool ConfigParser::setValue(const std::string &path, const T &value, const T &old_value)
{
    return setValue<T>(Convertor::toWstring(path), value, old_value);
}
template <typename T>
bool ConfigParser::setValue(const std::wstring &path, const T &value, const T &old_value)
{
    write_lock wl(mLock);

    wptree &node = getUnsafe(path, true);

    if (node.get_value<T>() != old_value)
        return false;
    node.put_value<T>(value);
    return true;
}
template <>
inline	bool ConfigParser::setValue<std::string>(const std::string &path, const std::string &value, const std::string &old_value)
{
    return setValue<std::wstring>(path, Convertor::toWstring(value), Convertor::toWstring(old_value));
}

template <typename T>
bool ConfigParser::setValueList(const std::string &path, const std::list<T> &value_list)
{
    return setValueList<T>(Convertor::toWstring(path), value_list);
}
template <typename T>
bool ConfigParser::setValueList(const std::wstring &path, const std::list<T> &value_list)
{
    write_lock rl(mLock);

    std::size_t sep_found = path.rfind('.');
    if (sep_found == std::string::npos)
    {
        return false;
    }

    std::wstring abs_path(path, 0, sep_found);
    std::wstring node_name(path, ++sep_found);
    //std::cout << "abs_path: " << abs_path << std::endl;
    //std::cout << "node_name: " << node_name << std::endl;
    wptree &node = getUnsafe(abs_path, true);

    // clear all old value
    node.erase(node_name);

    // insert new value
    BOOST_FOREACH( const T &v, value_list)
    {
        wptree child;
        child.put_value(v);
        node.add_child(node_name, child);
    }

    return true;
}
template <>
inline	bool ConfigParser::setValueList<std::string>(const std::string &path, const std::list<std::string> &value_list)
{
    std::list<std::wstring> wvalue_list;
    BOOST_FOREACH(const std::string &value, value_list)
    {
        wvalue_list.push_back(Convertor::toWstring(value));
    }
    return setValueList<std::wstring>(path, wvalue_list);
}
#if 1
/*
  template <>
  inline	bool ConfigParser::setValueList<wstr2wstr_map>(const std::string &path, const std::list<wstr2wstr_map> &value_list)
  {
  std::wstring wpath;
  wpath.assign(path.begin(), path.end());
  return setValueList<wstr2wstr_map>(wpath, value_list);
  }
*/
template <>
inline	bool ConfigParser::setValueList<wstr2wstr_map>(const std::wstring &path, const std::list<wstr2wstr_map> &value_list)
{
    write_lock rl(mLock);

    std::size_t sep_found = path.rfind('.');
    if (sep_found == std::string::npos)
    {
        return false;
    }

    std::wstring abs_path(path, 0, sep_found);
    std::wstring node_name(path, ++sep_found);
    //std::cout << "abs_path: " << abs_path << std::endl;
    //std::cout << "node_name: " << node_name << std::endl;
    wptree &node = getUnsafe(abs_path, true);

    // clear all old value
    node.erase(node_name);

    // insert new value
    BOOST_FOREACH( const wstr2wstr_map &v, value_list)
    {
        wptree child;

        BOOST_FOREACH(const wstr2wstr_tuple &tuple, v)
        {
            child.put(tuple.first, tuple.second);
        }
        node.add_child(node_name, child);
    }

    return true;
}
template <>
inline bool ConfigParser::setValueList<str2str_map>(const std::wstring &path, const std::list<str2str_map> &value_list)
{
    std::list<wstr2wstr_map> wvalue_list;
    BOOST_FOREACH(const str2str_map &value, value_list)
    {
        wvalue_list.push_back(mapstr2wstr(value));
    }
    return setValueList<wstr2wstr_map>(path, wvalue_list);
}
#endif
template <typename T>
bool ConfigParser::setValueList(const std::string &path, const std::list<T> &value_list, const std::list<T> &old_value_list)
{
    return setValueList<T>(Convertor::toWstring(path), value_list, old_value_list);
}
template <typename T>
bool ConfigParser::setValueList(const std::wstring &path, const std::list<T> &value_list, const std::list<T> &old_value_list)
{
    write_lock rl(mLock);

    std::size_t sep_found = path.rfind('.');
    if (sep_found == std::string::npos)
    {
        return false;
    }

    std::wstring abs_path(path, 0, sep_found);
    std::wstring node_name(path, ++sep_found);
    //std::cout << "abs_path: " << abs_path << std::endl;
    //std::cout << "node_name: " << node_name << std::endl;
    wptree &node = getUnsafe(abs_path, true);

    std::list<T> curr_value_list;
    BOOST_FOREACH( const wptree::value_type &v, node)
    {
        if (v.first == node_name)
        {
            curr_value_list.push_back(v.second.get_value<T>());
        }
    }
    if (curr_value_list.size() != old_value_list.size())
        return false;

    auto oit = old_value_list.begin();
    auto cit = curr_value_list.begin();
    for (;
         oit != old_value_list.end() && cit != curr_value_list.end();
         ++oit, ++cit)
    {
        if (*oit != *cit)
        {
            return false;
        }
    }

    // clear all old value
    node.erase(node_name);

    // insert new value
    BOOST_FOREACH( const T &v, value_list)
    {
        wptree child;
        child.put_value(v);
        node.add_child(node_name, child);
    }

    return true;
}
template <>
inline	bool ConfigParser::setValueList<std::string>(const std::string &path, const std::list<std::string> &value_list, const std::list<std::string> &old_value_list)
{
    std::list<std::wstring> wvalue_list;
    std::list<std::wstring> wold_value_list;
    BOOST_FOREACH(const std::string &value, value_list)
    {
        wvalue_list.push_back(Convertor::toWstring(value));
    }
    BOOST_FOREACH(const std::string &old_value, old_value_list)
    {
        wold_value_list.push_back(Convertor::toWstring(old_value));
    }
    return setValueList<std::wstring>(path, wvalue_list, wold_value_list);
}
#if 1
/*
  template <>
  inline	bool ConfigParser::setValueList<wstr2wstr_map>(const std::string &path, const std::list<wstr2wstr_map> &value_list, const std::list<wstr2wstr_map> &old_value_list)
  {
  std::wstring wpath;
  wpath.assign(path.begin(), path.end());
  return setValueList<wstr2wstr_map>(wpath, value_list, old_value_list);
  }
*/
template <>
inline	bool ConfigParser::setValueList<wstr2wstr_map>(const std::wstring &path, const std::list<wstr2wstr_map> &value_list, const std::list<wstr2wstr_map> &old_value_list)
{
    write_lock rl(mLock);

    std::size_t sep_found = path.rfind('.');
    if (sep_found == std::string::npos)
    {
        return false;
    }

    std::wstring abs_path(path, 0, sep_found);
    std::wstring node_name(path, ++sep_found);
    //std::cout << "abs_path: " << abs_path << std::endl;
    //std::cout << "node_name: " << node_name << std::endl;
    wptree &node = getUnsafe(abs_path, true);

    std::list<wstr2wstr_map> curr_value_list;
    BOOST_FOREACH( const wptree::value_type &v, node)
    {
        if (v.first == node_name)
        {
            wstr2wstr_map sm;
            BOOST_FOREACH(const wptree::value_type &smv, v.second)
            {
                sm[smv.first] = smv.second.get_value<std::wstring>();
            }
            curr_value_list.push_back(sm);
        }
    }

    if (curr_value_list.size() != old_value_list.size())
        return false;


    auto oit = old_value_list.begin();
    auto cit = curr_value_list.begin();
    for(;
        oit != old_value_list.end() && cit != curr_value_list.end();
        ++oit, ++cit)
    {
        const wstr2wstr_map &osm = *oit;
        const wstr2wstr_map &csm = *cit;
        if (osm.size() != csm.size())
            return false;
        auto oiit = osm.begin();
        auto ciit = csm.begin();
        for (;
             oiit != osm.end() && ciit != csm.end();
             ++oiit, ++ciit)
        {
            if (oiit->first != ciit->first || oiit->second != ciit->second)
                return false;
        }
    }

    // clear all old value
    node.erase(node_name);

    // insert new value
    BOOST_FOREACH( const wstr2wstr_map &v, value_list)
    {
        wptree child;

        BOOST_FOREACH(const wstr2wstr_tuple &tuple, v)
        {
            child.put(tuple.first, tuple.second);
        }
        node.add_child(node_name, child);
    }

    return true;
}
template <>
inline bool ConfigParser::setValueList<str2str_map>(const std::wstring &path, const std::list<str2str_map> &value_list, const std::list<str2str_map> &old_value_list)
{
    std::list<wstr2wstr_map> wvalue_list;
    std::list<wstr2wstr_map> wold_value_list;
    BOOST_FOREACH(const str2str_map &value, value_list)
    {
        wvalue_list.push_back(mapstr2wstr(value));
    }
    BOOST_FOREACH(const str2str_map &old_value, old_value_list)
    {
        wold_value_list.push_back(mapstr2wstr(old_value));
    }
    return setValueList<wstr2wstr_map>(path, wvalue_list, wold_value_list);
}
#endif

typedef sp<ConfigParser> ConfigParserPtr;

CBASE_END

#endif
