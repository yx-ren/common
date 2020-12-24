#include <common/base/Utility.h>

#ifdef WIN32
#include <objbase.h>
#include <windows.h>
#include <wchar.h>
#else
#include <uuid/uuid.h>
#endif

CBASE_BEGIN

Utility::Utility()
{
}
Utility::~Utility()
{
}
#ifdef WIN32
std::wstring Utility::stringToWstring(const std::string &str)
{
    LPCSTR pszSrc = str.c_str();
    int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
	if (nLen == 0)
		return std::wstring(L"");
	wchar_t *pwszDst = new wchar_t[nLen];
	if (!pwszDst)
		return std::wstring(L"");
	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
	std::wstring wstr(pwszDst);
	delete[] pwszDst;
	pwszDst = NULL;

	return wstr;
}

std::string Utility::wstringToString(const std::wstring &wstr)
{
	LPCWSTR pwszSrc = wstr.c_str();
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if(nLen == 0)
		return std::string("");
	char *pszDst = new char[nLen];
	if(!pszDst)
		return std::string("");
	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	std::string str(pszDst);
	delete [] pszDst;
	pszDst = NULL;

	return str;
}
#endif
std::string Utility::BinToHex(const std::string &strBin, bool bIsUpper)
{
    std::string strHex;
    strHex.resize(strBin.size() * 2);
    for (size_t i = 0; i < strBin.size(); i++)
    {
        uint8_t cTemp = strBin[i];
        for (size_t j = 0; j < 2; j++)
        {
            uint8_t cCur = (cTemp & 0x0f);
            if (cCur < 10)
            {
                cCur += '0';
            }
            else
            {
                cCur += ((bIsUpper ? 'A' : 'a') - 10);
            }
            strHex[2 * i + 1 - j] = cCur;
            cTemp >>= 4;
        }
    }

    return strHex;
}
std::string Utility::HexToBin(const std::string &strHex)
{
    if (strHex.size() % 2 != 0)
    {
        return "";
    }
    std::string strBin;
    strBin.resize(strHex.size() / 2);
    for (size_t i = 0; i < strBin.size(); i++)
    {
        uint8_t cTemp = 0;
        for (size_t j = 0; j < 2; j++)
        {
            char cCur = strHex[2 * i + j];
            if (cCur >= '0' && cCur <= '9')
            {
                cTemp = (cTemp << 4) + (cCur - '0');
            }
            else if (cCur >= 'a' && cCur <= 'f')
            {
                cTemp = (cTemp << 4) + (cCur - 'a' + 10);
            }
            else if (cCur >= 'A' && cCur <= 'F')
            {
                cTemp = (cTemp << 4) + (cCur - 'A' + 10);
            }
            else
            {
                return "";
            }
        }
        strBin[i] = cTemp;
    }
    return strBin;
}
std::string Utility::ws2s(const std::wstring& ws)
{
    std::string curLocale = setlocale(LC_ALL, NULL);// curLocale = "C";
    setlocale(LC_ALL, "chs");
    const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize);
    wcstombs(_Dest, _Source, _Dsize);
    std::string result = _Dest;
    delete[]_Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}
std::wstring Utility::s2ws(const std::string &s)
{
    setlocale(LC_ALL,"chs");
    const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest,0,_Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    std::wstring result = _Dest;
    delete []_Dest;
    setlocale(LC_ALL,"c");
    return result;
}
std::string Utility::generateUuid()
{
    char buf[64] = {0};
    #ifdef WIN32
    GUID guid;
    CoCreateGuid(&guid);
    _snprintf_s(
        buf,
        sizeof(buf),
        "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1],
        guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]
        );
    #else
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid,buf);
    #endif
    return std::string(buf);
}
CBASE_END
