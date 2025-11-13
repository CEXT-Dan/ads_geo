#include "StdAfx.h"
#include "AdsMap.h"

static resbuf* AdsObjectToResbuf(const AdsObject& adso)
{
    switch (adso.index())
    {
        case 0:
        {
            return acutNewRb(RTNIL);
        }
        case 1:
        {
            resbuf* pRb = acutNewRb(RTLONG);
            pRb->resval.rlong = std::get<int>(adso);
            return pRb;
        }
        case 2:
        {
            resbuf* pRb = acutNewRb(RTREAL);
            pRb->resval.rreal = std::get<double>(adso);
            return pRb;
        }
        case 3:
        {
            resbuf* pRb = acutNewRb(RTENAME);
            acdbGetAdsName(pRb->resval.rlname, std::get<AcDbObjectId>(adso));
            return pRb;
        }
        case 4:
        {
            resbuf* pRb = acutNewRb(RTPOINT);
            ZeroMemory(pRb->resval.rpoint, sizeof(AcGePoint3d));
            memcpy_s(pRb->resval.rpoint, sizeof(AcGePoint2d), asDblArray(std::get<AcGePoint2d>(adso)), sizeof(AcGePoint2d));
            return pRb;
        }
        case 5:
        {
            resbuf* pRb = acutNewRb(RT3DPOINT);
            memcpy_s(pRb->resval.rpoint, sizeof(AcGePoint3d), asDblArray(std::get<AcGePoint3d>(adso)), sizeof(AcGePoint3d));
            return pRb;
        }
        case 6:
        {
            resbuf* pRb = acutNewRb(RTSTR);
            pRb->resval.rstring = StrDupW(std::get<std::wstring>(adso).c_str());;
            return pRb;
        }
    }
    return acutNewRb(RTNIL);
}

static AdsObject ResbufToAdsObject(const resbuf* pRb)
{
    switch (pRb->restype)
    {
        case RTSHORT:
        {
            return AdsObject{ static_cast<int32_t>(pRb->resval.rint) };
        }
        case RTLONG:
        {
            return AdsObject{ static_cast<int32_t>(pRb->resval.rlong) };
        }
        case RTREAL:
        {
            return AdsObject{ static_cast<double>(pRb->resval.rreal) };
        }
        case RTENAME:
        {
            AcDbObjectId id;
            acdbGetObjectId(id, pRb->resval.rlname);
            return AdsObject{ id };
        }
        case RTPOINT:
        {
            return AdsObject{ asPnt2d(pRb->resval.rpoint) };
        }
        case RT3DPOINT:
        {
            return AdsObject{ asPnt3d(pRb->resval.rpoint) };
        }
        case RTSTR:
        {
            return AdsObject{ std::wstring{ pRb->resval.rstring} };
        }
    }
    return AdsObject{};
}

//-----------------------------------------------------------------------------------------
//AdsObjectMapWrapper
int AdsObjectMapWrapper::AdsObjectMapClear()
{
    pMap.clear();
    return RSRSLT;
}

int AdsObjectMapWrapper::AdsObjectMapInsert()
{
    int argNum = 0;
    AdsObject key;
    AdsObject value;
    AcResBufPtr pArgs(acedGetArgs());

    for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
    {
        if (argNum == 0)
        {
            if (key = ResbufToAdsObject(pTail); key.index() == 0)
            {
                acedRetNil();
                return RSRSLT;
            }
            argNum++;
        }
        else if (argNum == 1)
        {
            value = ResbufToAdsObject(pTail);
            argNum++;
        }
        else
        {
            break;
        }
    }
    if (argNum == 2)
    {
        pMap.emplace(key,value);
        acedRetT();
        return RSRSLT;
    }
    acedRetNil();
    return RSRSLT;
}

int AdsObjectMapWrapper::AdsObjectMapgetat()
{
    int argNum = 0;
    AdsObject key;
    AcResBufPtr pArgs(acedGetArgs());
    for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
    {
        if (argNum == 0)
        {
            if (key = ResbufToAdsObject(pTail); key.index() == 0)
            {
                acedRetNil();
                return RSRSLT;
            }
            argNum++;
        }
        else
        {
            break;
        }
    }
    if (argNum == 1)
    {
        if (pMap.contains(key))
        {
            const auto& value = pMap.at(key);
            switch (value.index())
            {
                case 0:
                {
                    acedRetNil();
                    return RSRSLT;
                }
                case 1:
                {
                    acedRetInt(std::get<int>(value));
                    return RSRSLT;

                }
                case 2:
                {
                    acedRetReal(std::get<double>(value));
                    return RSRSLT;
                }
                case 3:
                {
                    ads_name name = { 0L };
                    acdbGetAdsName(name, std::get<AcDbObjectId>(value));
                    acedRetName(name, RTENAME);
                    return RSRSLT;
                }
                case 4:
                {
                    ads_point pnt{};
                    ZeroMemory(pnt, sizeof(AcGePoint3d));
                    memcpy_s(pnt, sizeof(AcGePoint2d), asDblArray(std::get<AcGePoint2d>(value)), sizeof(AcGePoint2d));
                    acedRetPoint(pnt);
                    return RSRSLT;
                }
                case 5:
                {
                    ads_point pnt{};
                    memcpy_s(pnt, sizeof(AcGePoint3d), asDblArray(std::get<AcGePoint3d>(value)), sizeof(AcGePoint3d));
                    acedRetPoint(pnt);
                    return RSRSLT;

                }
                case 6:
                {
                    acedRetStr(std::get<std::wstring>(value).c_str());
                    return RSRSLT;
                }
            }
        }
    }
    acedRetNil();
    return RSRSLT;
}

int AdsObjectMapWrapper::AdsObjectMapContains()
{
    int argNum = 0;
    AdsObject key;
    AcResBufPtr pArgs(acedGetArgs());
    for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
    {
        if (argNum == 0)
        {
            if (key = ResbufToAdsObject(pTail); key.index() == 0)
            {
                acedRetNil();
                return RSRSLT;
            }
            argNum++;
        }
        else
        {
            break;
        }
    }
    if (argNum == 1)
    {
        if (pMap.contains(key))
        {
            acedRetT();
            return RSRSLT;
        }
    }
    acedRetNil();
    return RSRSLT;
}

int AdsObjectMapWrapper::AdsObjectMapgetall()
{
    AcResBufPtr pResultHead (acutNewRb(RTLB));
    resbuf* pResultTail = pResultHead.get();
    for (const auto& item : pMap)
    {
        pResultTail = pResultTail->rbnext = acutNewRb(RTLB);
        pResultTail = pResultTail->rbnext = AdsObjectToResbuf(item.first);
        pResultTail = pResultTail->rbnext = AdsObjectToResbuf(item.second);
        pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
    }
    pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
    acedRetList(pResultHead.get());
    return RSRSLT;
}

//-----------------------------------------------------------------------------------------
//AdsObjectSetWrapper
int AdsObjectSetWrapper::AdsObjectSetClear()
{
    pSet.clear();
    return RSRSLT;
}

int AdsObjectSetWrapper::AdsObjectSetInsert()
{
    int argNum = 0;
    AdsObject key;
    AcResBufPtr pArgs(acedGetArgs());

    for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
    {
        if (argNum == 0)
        {
            if (key = ResbufToAdsObject(pTail); key.index() == 0)
            {
                acedRetNil();
                return RSRSLT;
            }
            argNum++;
        }
        else
        {
            break;
        }
    }
    if (argNum == 1)
    {
        pSet.emplace(key);
        acedRetT();
        return RSRSLT;
    }
    acedRetNil();
    return RSRSLT;
}

int AdsObjectSetWrapper::AdsObjectSetContains()
{
    int argNum = 0;
    AdsObject key;
    AcResBufPtr pArgs(acedGetArgs());
    for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
    {
        if (argNum == 0)
        {
            if (key = ResbufToAdsObject(pTail); key.index() == 0)
            {
                acedRetNil();
                return RSRSLT;
            }
            argNum++;
        }
        else
        {
            break;
        }
    }
    if (argNum == 1)
    {
        if (pSet.contains(key))
        {
            acedRetT();
            return RSRSLT;
        }
    }
    acedRetNil();
    return RSRSLT;
}

int AdsObjectSetWrapper::AdsObjectSetgetall()
{
    AcResBufPtr pResultHead(acutNewRb(RTLB));
    resbuf* pResultTail = pResultHead.get();
    for (const auto& item : pSet)
    {
        pResultTail = pResultTail->rbnext = AdsObjectToResbuf(item);
    }
    pResultTail = pResultTail->rbnext = acutNewRb(RTLE);
    acedRetList(pResultHead.get());
    return RSRSLT;
}
