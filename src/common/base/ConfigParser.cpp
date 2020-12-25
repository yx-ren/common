#include <common/base/ConfigParser.h>
#include <locale>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <list>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/current_function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/fstream.hpp>

#include <common/base/logger/Logger.h>
#include <common/base/Utility.h>
#include <common/base/ScopedGuard.h>
#ifdef WIN32
#include <codecvt>
#endif // WIN32

CBASE_BEGIN

ConfigParser::ConfigParser(const std::wstring &filename)
    : mFilename(filename)
{
#ifndef WIN32
    std::string sfilename(filename.begin(), filename.end());
    std::wifstream ifs(sfilename, std::ios::in);
#else
	boost::filesystem::wpath infp(filename);
	boost::filesystem::wifstream ifs(infp);
#endif
    #ifdef WIN32
    ifs.imbue(std::locale(std::locale::classic(),new std::codecvt_utf8<wchar_t>));
    #elif defined(__APPLE__) && defined(__MACH__)
    ifs.imbue(std::locale("en_US.UTF-8"));
    #else
    ifs.imbue(std::locale("en_US.UTF8"));
    #endif

    read_xml(ifs, mConfigTree, boost::property_tree::xml_parser::trim_whitespace);
#ifdef CONFIG_DEBUG
    std::wcout << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

ConfigParser::ConfigParser(const std::string &filename)
    : mFilename(Convertor::toWstring(filename))
{
    std::wifstream ifs(filename, std::ios::in);
    #ifdef WIN32
    ifs.imbue(std::locale(std::locale::classic(),new std::codecvt_utf8<wchar_t>));
    #elif defined(__APPLE__) && defined(__MACH__)
    ifs.imbue(std::locale("en_US.UTF-8"));
    #else
    ifs.imbue(std::locale("en_US.UTF8"));
    #endif
    read_xml(ifs, mConfigTree, boost::property_tree::xml_parser::trim_whitespace);
    /*
      std::locale empty_locale = std::locale::empty();
      read_xml(filename, mConfigTree, boost::property_tree::xml_parser::trim_whitespace, std::locale(empty_locale, new std::codecvt_utf8<wchar_t>()));
    */
#ifdef CONFIG_DEBUG
    std::wcout << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

ConfigParser::ConfigParser(const std::string &xmlString, const std::string &saveFilename)
    : mFilename(Convertor::toWstring(saveFilename))
{
    std::wistringstream ss(Convertor::toWstring(xmlString));
#ifdef WIN32
    ss.imbue(std::locale(std::locale::classic(),new std::codecvt_utf8<wchar_t>));
#elif defined(__APPLE__) && defined(__MACH__)
    ss.imbue(std::locale("en_US.UTF-8"));
#else
    ss.imbue(std::locale("en_US.UTF8"));
#endif
    read_xml(ss, mConfigTree, boost::property_tree::xml_parser::trim_whitespace);
#ifdef CONFIG_DEBUG
    std::wcout << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

ConfigParser::ConfigParser(const wptree &wp) : mConfigTree(wp)
{
}

ConfigParser::~ConfigParser()
{
#ifdef CONFIG_DEBUG
    std::wcout << BOOST_CURRENT_FUNCTION << std::endl;
#endif
}

#if 0
ptree ConfigParser::get(const std::string &path)
{
    write_lock wl(mLock);
    return getUnsafe(path);
}
#endif
const wptree &ConfigParser::get(const std::string &path ) const
{
    return get(Convertor::toWstring(path));
}
const wptree &ConfigParser::get(const std::wstring &path ) const
{
    read_lock rl(mLock);
    return getUnsafe(path);
}

wptree &ConfigParser::getUnsafe(const std::wstring &path, bool create)
{
#ifdef SIMPLE_PATH_FORMAT
    return mConfigTree.get_child(path);
#else
    SimplePathParser pparser;
    parse_info<const wchar_t *> info = parse<wchar_t>(path.c_str(), pparser);
    if (!info.full || pparser.getParsedNodes().empty())
    {
        throw std::runtime_error(boost::str(boost::format("invalid path: %1%") % std::string(path.begin(), path.end())).c_str());
    }

    wptree *pt = &mConfigTree;
    BOOST_FOREACH(const PathNodePtr &node, pparser.getParsedNodes())
    {
        BOOST_ASSERT(node && node->isValid());

        if (node->isSimple())
        {
            if (create && !pt->count(node->name()))
            {
                pt = &(pt->put_child(node->name(), wptree()));
            }
            else
            {
                pt = &pt->get_child(node->name());
            }
        }
        else
        {
            bool found = false;
            BOOST_FOREACH(wptree::value_type &v, *pt)
            {
                if (v.first == node->name())
                {
                    wptree *child = &(v.second);
                    if (child->get_child(L"<xmlattr>." + node->attribute()).get_value<std::wstring>() == node->value())
                    {
                        pt = child;
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                if (create)
                {
                    pt = &(pt->add_child(node->name(), wptree()));
                    pt->put(L"<xmlattr>." + node->attribute(), node->value());
                }
                else
                {
                    throw std::runtime_error(boost::str(boost::format("the node '%1%' of '%2%=%3%' is not found.") % std::string(node->name().begin(), node->name().end())
                                                    % std::string(node->attribute().begin(), node->attribute().end())
                                                                                        % std::string(node->value().begin(), node->value().end())).c_str());
                }
            }
        }
    }

    return *pt;
#endif // SIMPLE_PATH_FORMAT
}


const wptree &ConfigParser::getUnsafe(const std::wstring &path ) const
{
#ifdef SIMPLE_PATH_FORMAT
    return mConfigTree.get_child(path);
#else
    SimplePathParser pparser;
    parse_info<const wchar_t *> info = parse<wchar_t>(path.c_str(), pparser);
    if (!info.full || pparser.getParsedNodes().empty())
    {
        throw std::runtime_error(boost::str(boost::format("invalid path: %1%") % std::string(path.begin(), path.end())).c_str());
    }

    const wptree *pt = &mConfigTree;
    BOOST_FOREACH(const PathNodePtr &node, pparser.getParsedNodes())
    {
        BOOST_ASSERT(node && node->isValid());

        if (node->isSimple())
        {
            pt = &pt->get_child(node->name());
        }
        else
        {
            bool found = false;
            BOOST_FOREACH(const wptree::value_type &v, *pt)
            {
                if (v.first == node->name())
                {
                    const wptree *child = &(v.second);
                    if (child->get_child(L"<xmlattr>." + node->attribute()).get_value<std::wstring>() == node->value())
                    {
                        pt = child;
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                throw std::runtime_error(boost::str(boost::format("the node '%1%' of '%2%=%3%' is not found.") % std::string(node->name().begin(), node->name().end())
                                                    % std::string(node->attribute().begin(), node->attribute().end())
                                                    % std::string(node->value().begin(), node->value().end())).c_str());
            }
        }
    }

    return *pt;

#endif // SIMPLE_PATH_FORMAT
}

bool ConfigParser::save() const
{
    return save(mFilename);
}

bool ConfigParser::save(const std::string &filename) const
{
    return save(Convertor::toWstring(filename));
}

bool ConfigParser::save(const std::wstring &filename) const
{
    boost::filesystem::wpath destfile(filename);

    boost::filesystem::wpath tempfile = destfile.parent_path() / boost::filesystem::unique_path();
    //std::cout << "tempfile: " << tempfile << std::endl;
    //std::cout << "destfile: " << destfile << std::endl;
    std::wofstream ofs(tempfile.string(), std::ofstream::out);

    #ifdef WIN32
    ofs.imbue(std::locale(std::locale::classic(),new std::codecvt_utf8<wchar_t>));
    #elif defined(__APPLE__) && defined(__MACH__)
    ofs.imbue(std::locale("en_US.UTF-8"));
    #else
    ofs.imbue(std::locale("en_US.UTF8"));
    #endif
    ScopedGuard fileguard([&] ()
                          {
                              boost::filesystem::remove(tempfile);
                          });
    bool result = save(ofs);
    ofs.flush();
    bool isGood = ofs.good();
    ofs.close();

    if (isGood && result)
    {
        boost::filesystem::rename(tempfile, destfile);
        fileguard.dismiss();
    }
    return isGood;
}

bool ConfigParser::save(std::wostream &os) const
{
    auto settings = boost::property_tree::xml_writer_make_settings<wchar_t>(L' ', 4, L"utf-8");
    read_lock rl(mLock);
    write_xml(os, mConfigTree, settings);
    return true;
}

bool ConfigParser::isNodeExist(const std::string &nodePath)
{
    Utility util;
    std::wstring nodeStr = util.s2ws(nodePath);
    auto item = mConfigTree.get_child_optional(nodeStr);
    if(item)
        return true;
    return false;
}

CBASE_END
