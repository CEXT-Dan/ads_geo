// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//- StdAfx.h : include file for standard system include files,
//-      or project specific include files that are used frequently,
//-      but are changed infrequently
//-----------------------------------------------------------------------------
#pragma once

#pragma pack (push, 8)
#pragma warning(disable: 4786 4996 26812)

//#pragma warning(disable: 4098)

//-----------------------------------------------------------------------------
#define STRICT
#define NOMINMAX
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN			//- Exclude rarely-used stuff from Windows headers
#endif

//- Modify the following defines if you have to target a platform prior to the ones specified below.
//- Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER					//- Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER _WIN32_WINNT_WIN10			//- Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT			//- Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT _WIN32_WINNT_WIN10		//- Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS			//- Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS _WIN32_WINNT_WIN10	//- Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE				//- Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE _WIN32_WINNT_WIN10		//- Change this to the appropriate value to target IE 5.0 or later.
#endif


#define SETBIT(flags, bit, value) ((value) ? (flags |= (bit)) : (flags &= ~(bit)))
#define GETBIT(flags, bit) (((flags) & (bit)) ? true : false)


//- ObjectARX and OMF headers needs this
#include <map>
#include <vector>
#include <array>
#include <functional>
#include <memory>
#include <variant>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <execution>
#include <filesystem>

//-----------------------------------------------------------------------------
#include <afxwin.h>				//- MFC core and standard components
#include <afxext.h>				//- MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>				//- MFC OLE classes
#include <afxodlgs.h>			//- MFC OLE dialog classes
#include <afxdisp.h>			//- MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>				//- MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>				//- MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>			//- MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>				//- MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//-----------------------------------------------------------------------------
//- Include ObjectDBX/ObjectARX headers
//- Uncomment one of the following lines to bring a given library in your project.
//#define _BREP_SUPPORT_			//- Support for the BRep API
//#define _HLR_SUPPORT_				//- Support for the Hidden Line Removal API
//#define _AMODELER_SUPPORT_		//- Support for the AModeler API
//#define _ASE_SUPPORT_				//- Support for the ASI/ASE API
//#define _RENDER_SUPPORT_			//- Support for the AutoCAD Render API
//#define _ARX_CUSTOM_DRAG_N_DROP_	//- Support for the ObjectARX Drag'n Drop API
//#define _INC_LEAGACY_HEADERS_		//- Include legacy headers in this project
#include "arxHeaders.h"
#include "axlock.h"

#if defined(_ARXTARGET)
#pragma comment( lib , "AcPal.lib" )
#pragma comment( lib , "acgeoment.lib" )
#endif

#if defined(_BRXTARGET)
//
#endif

#if defined(_GRXTARGET)
#pragma comment( lib , "gcad.lib" )
#pragma comment( lib , "gcbase.lib" )
#pragma comment( lib , "gcbr.lib" )
#pragma comment( lib , "gccore.lib" )
#pragma comment( lib , "gcdb.lib" )
#endif

#if defined(_ZRXTARGET)
//
#endif


//-----------------------------------------------------------------------------
#include "DocData.h" //- Your document specific data class holder

//- Declare it as an extern here so that it becomes available in all modules
extern AcApDataManager<CDocData> DocVars;

using AcResBufPtr = std::unique_ptr < resbuf, decltype([](resbuf* ptr) noexcept
    {
        acutRelRb(ptr);
    }) > ;


inline static double roundPointComponentToGTol(double value)
{
    const double precision = AcGeContext::gTol.equalPoint();
    return std::round(value / precision) * precision;
}

//boost
template<std::size_t Bits> struct hash_mix_impl;

template<> struct hash_mix_impl<64>
{
    inline static std::uint64_t fn(std::uint64_t x)
    {
        std::uint64_t const m = 0xe9846af9b1a615d;
        x ^= x >> 32;
        x *= m;
        x ^= x >> 32;
        x *= m;
        x ^= x >> 28;
        return x;
    }
};

static inline std::size_t hash_mix(std::size_t v)
{
    return hash_mix_impl<sizeof(std::size_t) * CHAR_BIT>::fn(v);
}

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
    seed = hash_mix(seed + 0x9e3779b9 + std::hash<T>()(v));
}

namespace std
{
    template <>
    struct hash<AcGePoint2d>
    {
        size_t operator()(const AcGePoint2d& p) const
        {
            std::size_t seed = 0;
            hash_combine(seed, roundPointComponentToGTol(p.x));
            hash_combine(seed, roundPointComponentToGTol(p.y));
            return seed;
        }
    };

    template <>
    struct hash<AcGePoint3d>
    {
        size_t operator()(const AcGePoint3d& p) const
        {
            std::size_t seed = 0;
            hash_combine(seed, roundPointComponentToGTol(p.x));
            hash_combine(seed, roundPointComponentToGTol(p.y));
            hash_combine(seed, roundPointComponentToGTol(p.z));
            return seed;
        }
    };

    template <>
    struct hash<AcDbObjectId>
    {
        size_t operator()(const AcDbObjectId& id) const
        {
            std::size_t seed = 0;
            hash_combine(seed, id.asOldId());
            return seed;
        }
    };
}

using AdsObject = std::variant<std::monostate, int, double, AcDbObjectId, AcGePoint2d, AcGePoint3d, std::wstring>;
typedef std::unordered_map<AdsObject, AdsObject> AdsObjectMap;
typedef std::unordered_set<AdsObject> AdsObjectSet;

//Voronoiator
namespace delaunator
{
    struct Edge
    {
        AcGePoint3d a;
        AcGePoint3d b;
    };
    using Edges = std::vector<Edge>;
}

#pragma pack (pop)

