#ifndef __COMMON_BASE_UTILITY_H__
#define __COMMON_BASE_UTILITY_H__

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <string.h>
#include <stdint.h>
#include <common/base/common.h>

CBASE_BEGIN
const int endian = 1;

#define is_bigendian  ((*(char*)&endian) == 0)
#define is_littlendian  ((*(char*)&endian) == 1)

class Utility
{
public:
    Utility();
    ~Utility();
    static std::string BinToHex(const std::string &strBin, bool bIsUpper = false);
    static std::string HexToBin(const std::string &strHex);
    static std::string ws2s(const std::wstring &ws);
    static std::wstring s2ws(const std::string &s);
    static std::string generateUuid();
#ifdef WIN32
    static std::wstring stringToWstring(const std::string &str);
    static std::string wstringToString(const std::wstring &wstr);
#endif

    template <typename T,
              class = typename std::enable_if<std::is_integral<T>::value>::type>
    static std::string BinToHex(T v, bool bIsUpper = false)
    {
        char size_of = sizeof(T);

        std::stringstream s;

        if (bIsUpper)
        {
            s.setf(std::ios_base::uppercase);
        }
        else
        {
            s.unsetf(std::ios_base::uppercase);
        }

        if (size_of == 1)
        {
            s << std::setw(2) << std::hex << (unsigned short)v;
        }
        else
        {
            s << std::setw(sizeof(T) * 2) << std::setfill('0') << std::hex << v;
        }

        return s.str();
    }

    template <typename T,
              class = typename std::enable_if<std::is_integral<T>::value>::type>
    static T HexToBin(const std::string &strHex)
    {
        char size_of = sizeof(T);

        T v;
        std::stringstream s;

        s << strHex;

        if (size_of == 1)
        {
            unsigned int t = 0;
            s >> std::hex >> t;
            v = static_cast<T>(t);
        }
        else
        {
            s >> std::hex >> v;
        }

        return v;
    }

    template <int32_t max,
              typename T,
              class = typename std::enable_if<std::is_integral<T>::value>::type>
    static std::vector<T> random(int32_t extract_count = max)
    {
        std::vector<T> results;

        if (max <= 0 || extract_count <= 0 || extract_count > max)
        {
            return results;
        }

        results.resize(max);
        auto t = time(nullptr);
        srand(t*rand()%5+1);

        for (int32_t i = 1; i < max; ++i)
        {
            results[i] = i;
            int32_t randv = rand()%i;
            std::swap(results[i], results[randv%i]);
        }

        if (extract_count != max)
        {
            results.erase(results.begin() + extract_count, results.end());
        }

        return std::move(results);
    }
};

CBASE_END
#endif
