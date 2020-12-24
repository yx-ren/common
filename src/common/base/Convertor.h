#ifndef __COMMON_BASE_CONVERTOR_H__
#define __COMMON_BASE_CONVERTOR_H__

#include <vector>
#include <string>
#include <sstream>
#include <boost/locale.hpp>
#include <common/base/common.h>

CBASE_BEGIN

class Convertor
{
public:
    static std::wstring toWstring(const std::u16string& text)
    {
        return boost::locale::conv::utf_to_utf<wchar_t, char16_t>(text);
    }

    static std::wstring toWstring(const std::string& text)
    {
        return boost::locale::conv::utf_to_utf<wchar_t, char>(text);
    }
    static std::u16string toU16String(const std::string& text)
    {
        return boost::locale::conv::utf_to_utf<char16_t, char>(text);
    }

    static std::u16string toU16String(const std::wstring& text)
    {
        return boost::locale::conv::utf_to_utf<char16_t, wchar_t>(text);
    }

    static std::string toString(const std::u16string& text)
    {
        return boost::locale::conv::utf_to_utf<char, char16_t>(text);
    }

    static std::string toString(const std::wstring& text)
    {
        return boost::locale::conv::utf_to_utf<char, wchar_t>(text);
    }
};

CBASE_END

#endif
