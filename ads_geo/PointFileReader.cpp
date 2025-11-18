#include "stdafx.h"
#include "PointFileReader.h"

/*
 * strtod.c --
 *
 *	Source code for the "strtod" library procedure.
 *
 * Copyright (c) 1988-1993 The Regents of the University of California.
 * Copyright (c) 1994 Sun Microsystems, Inc.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * RCS: @(#) $Id: strtod.c,v 1.3 2000/02/17 07:11:22 matz Exp $
 */

 //-==-==-===-=-=-=-==-=-=-=-=--=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=
static int maxExponent = 511;   /* Largest possible base 10 exponent.  Any
                                 * exponent larger than this will already
                                 * produce underflow or overflow, so there's
                                 * no need to worry about additional digits.
                                 */
static double powersOf10[] = {  /* Table giving binary powers of 10.  Entry */
        10.,                    /* is 10^2^i.  Used to convert decimal */
        100.,                   /* exponents into floating-point numbers. */
        1.0e4,
        1.0e8,
        1.0e16,
        1.0e32,
        1.0e64,
        1.0e128,
        1.0e256
};

static double strtodd(const char* string, char** endPtr)
{
    int sign, expSign = FALSE;
    double fraction, dblExp, * d;
    const char* p;
    int c;
    int exp = 0;
    int fracExp = 0;
    int mantSize;
    int decPt;
    const char* pExp;
    p = string;
    while (isspace(*p))
    {
        p += 1;
    }
    if (*p == '-')
    {
        sign = TRUE;
        p += 1;
    }
    else
    {
        if (*p == '+')
        {
            p += 1;
        }
        sign = FALSE;
    }
    decPt = -1;
    for (mantSize = 0;; mantSize += 1)
    {
        c = *p;
        if (!isdigit(c))
        {
            if ((c != '.') || (decPt >= 0))
            {
                break;
            }
            decPt = mantSize;
        }
        p += 1;
    }
    pExp = p;
    p -= mantSize;
    if (decPt < 0)
    {
        decPt = mantSize;
    }
    else
    {
        mantSize -= 1;
    }
    if (mantSize > 18)
    {
        fracExp = decPt - 18;
        mantSize = 18;
    }
    else
    {
        fracExp = decPt - mantSize;
    }
    if (mantSize == 0)
    {
        fraction = 0.0;
        p = string;
        goto done;
    }
    else
    {
        int frac1, frac2;
        frac1 = 0;
        for (; mantSize > 9; mantSize -= 1)
        {
            c = *p;
            p += 1;
            if (c == '.')
            {
                c = *p;
                p += 1;
            }
            frac1 = 10 * frac1 + (c - '0');
        }
        frac2 = 0;
        for (; mantSize > 0; mantSize -= 1)
        {
            c = *p;
            p += 1;
            if (c == '.')
            {
                c = *p;
                p += 1;
            }
            frac2 = 10 * frac2 + (c - '0');
        }
        fraction = (1.0e9 * frac1) + frac2;
    }

    p = pExp;
    if ((*p == 'E') || (*p == 'e'))
    {
        p += 1;
        if (*p == '-')
        {
            expSign = TRUE;
            p += 1;
        }
        else
        {
            if (*p == '+')
            {
                p += 1;
            }
            expSign = FALSE;
        }
        while (isdigit(*p))
        {
            exp = exp * 10 + (*p - '0');
            p += 1;
        }
    }
    if (expSign)
    {
        exp = fracExp - exp;
    }
    else
    {
        exp = fracExp + exp;
    }
    if (exp < 0)
    {
        expSign = TRUE;
        exp = -exp;
    }
    else
    {
        expSign = FALSE;
    }
    if (exp > maxExponent)
    {
        exp = maxExponent;
        errno = ERANGE;
    }
    dblExp = 1.0;
    for (d = powersOf10; exp != 0; exp >>= 1, d += 1)
    {
        if (exp & 01)
        {
            dblExp *= *d;
        }
    }
    if (expSign)
    {
        fraction /= dblExp;
    }
    else
    {
        fraction *= dblExp;
    }

done:
    if (endPtr != NULL)
    {
        *endPtr = (char*)p;
    }

    if (sign)
    {
        return -fraction;
    }
    return fraction;
}

//-==-==-===-=-=-=-==-=-=-=-=--=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=
// utf8_to_wstr
#pragma warning(push)
#pragma warning(disable: 4267)
static std::wstring utf8_to_wstr(const std::string_view str8) noexcept
{
    const int count = MultiByteToWideChar(CP_UTF8, 0, str8.data(), str8.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str8.data(), str8.length(), &wstr[0], count);
    return wstr;
}
#pragma warning(pop)

//-==-==-===-=-=-=-==-=-=-=-=--=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=
// FileHnd
class FileHnd
{
public:
    FileHnd(HANDLE h)
        :_h(h)
    {
    }

    ~FileHnd()
    {
        if (CloseHandle(_h) == FALSE)
            assert(0);
    }

    HANDLE hnd() const
    {
        return _h;
    }

private:
    HANDLE _h;
};

//-==-==-===-=-=-=-==-=-=-=-=--=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=
// FileMap
class FileView
{
public:
    FileView(LPVOID fmap, size_t len)
        :_fmap((char*)fmap), _fend((char*)fmap + len), _len(len)
    {
    }

    ~FileView()
    {
        if (UnmapViewOfFile(_fmap) == FALSE)
            assert(0);
    }

    char* beginf() const
    {
        return _fmap;
    }

    char* endf() const
    {
        return _fend;
    }

    size_t size() const
    {
        return _len;
    }

    std::string_view view() const
    {
        return std::string_view(_fmap, _len);
    }

private:
    char* _fmap = nullptr;
    char* _fend = nullptr;
    size_t _len = 0;
};

static auto getLispArgs() -> std::tuple<bool, std::filesystem::path, wchar_t>
{
    int argNum = 0;
    std::filesystem::path _pnezdFilePath;
    wchar_t _delimiter = ',';
    AcResBufPtr pArgs(acedGetArgs());

    for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
    {
        if (argNum == 0)
        {
            if (pTail->restype != RTSTR)
                return std::make_tuple(false, _pnezdFilePath, _delimiter);
            _pnezdFilePath = pTail->resval.rstring;
            argNum++;
        }
        else if (argNum == 1)
        {
            if (pTail->restype != RTSTR || wcslen(pTail->resval.rstring) == 0)
                return std::make_tuple(false, _pnezdFilePath, _delimiter);
            _delimiter = pTail->resval.rstring[0];
            argNum++;
        }
        else
        {
            break;
        }
    }
    if (argNum != 2)
    {
        return std::make_tuple(false, _pnezdFilePath, _delimiter);
    }
    return std::make_tuple(true, _pnezdFilePath, _delimiter);
}

static int parse(const std::filesystem::path& _pnezdFilePath, wchar_t _delimiter, int _first, int _second, int _third)
{
    //read file into memory
    FileHnd fh(CreateFileW(_pnezdFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
    if (fh.hnd() == INVALID_HANDLE_VALUE)
    {
        acutPrintf(_T("Error: Could not open input file %ls: "), _pnezdFilePath.c_str());
        return RSRSLT;
    }
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(fh.hnd(), &fileSize))
    {
        acutPrintf(_T("Error: Failed to get file size for: %ls: "), _pnezdFilePath.c_str());
        return RSRSLT;
    }
    FileHnd fhm(CreateFileMapping(fh.hnd(), NULL, PAGE_READONLY, 0, 0, NULL));
    if (fhm.hnd() != INVALID_HANDLE_VALUE)
    {
        size_t cb = static_cast<size_t>(fileSize.QuadPart);
        FileView mv(MapViewOfFile(fhm.hnd(), FILE_MAP_READ, 0, 0, cb), cb);
        const std::string_view fv = mv.view();

        //list begin
        AcResBufPtr pResultHead(acutNewRb(RTLB));
        resbuf* pResultTail = pResultHead.get();

        for (auto iter = fv.data(), endp = fv.data() + fv.length(); iter < endp; iter++)
        {
            //sublist begin
            pResultTail = pResultTail->rbnext = acutNewRb(RTLB);

            //number
            char* n = (char*)memchr(iter, _delimiter, endp - iter);
            if (!n) [[unlikely]]
            {
                acedRetNil();
                return RSRSLT;
            }
            pResultTail = pResultTail->rbnext = acutNewRb(RTSTR);
            pResultTail->resval.rstring = StrDupW(utf8_to_wstr(std::string_view(iter, n - iter)).c_str());
            iter = n + 1;

            pResultTail = pResultTail->rbnext = acutNewRb(RT3DPOINT);
            char* x = (char*)memchr(iter, _delimiter, endp - iter);
            if (!x) [[unlikely]]
            {
                acedRetNil();
                return RSRSLT;
            }
            pResultTail->resval.rpoint[_first] = strtodd(iter, NULL);
            iter = x + 1;

            //easting
            char* y = (char*)memchr(iter, _delimiter, endp - iter);
            if (!y) [[unlikely]]
            {
                acedRetNil();
                return RSRSLT;
            }
            pResultTail->resval.rpoint[_second] = strtodd(iter, NULL);
            iter = y + 1;

            //elevation
            char* z = (char*)memchr(iter, _delimiter, endp - iter);
            if (!z) [[unlikely]]
            {
                acedRetNil();
                return RSRSLT;
            }
            pResultTail->resval.rpoint[_third] = strtodd(iter, NULL);
            iter = z + 1;

            //description
            char* d = (char*)memchr(iter, '\r', endp - iter);
            if (d)
            {
                pResultTail = pResultTail->rbnext = acutNewRb(RTSTR);
                pResultTail->resval.rstring = StrDupW(utf8_to_wstr(std::string_view(iter, d - iter)).c_str());
                iter = d + 1;
            }
            else
            {
                pResultTail = pResultTail->rbnext = acutNewRb(RTSTR);
                pResultTail->resval.rstring = StrDupW(utf8_to_wstr(std::string_view(iter, endp - 1)).c_str());
                break;
            }

            //sublist end
            pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
        }
        //list end
        pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
        acedRetList(pResultHead.get());
        return RSRSLT;
    }
    acedRetNil();
    return RSRSLT;
}

int PointFileReader::AdsReadPNEZD()
{
    if (const auto [success, _pnezdFilePath, _delimiter] = getLispArgs(); success)
        return parse(_pnezdFilePath, _delimiter, Y, X, Z);
    acedRetNil();
    return RSRSLT;
}

int PointFileReader::AdsReadPENZD()
{
    if (const auto [success, _pnezdFilePath, _delimiter] = getLispArgs(); success)
        return parse(_pnezdFilePath, _delimiter, X, Y, Z);
    acedRetNil();
    return RSRSLT;
}