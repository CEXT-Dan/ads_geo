#include "StdAfx.h"
#include "AdsMap.h"

static resbuf* AdsObjectToResbuf(const AdsObject& adso)
{
    switch (adso.index())
    {
        case 0:
        {
            return nullptr;
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

int AdsObjectMapWrapper::AdsObjectMapcreate()
{
    pMap.clear();
    return RSRSLT;
}

int AdsObjectMapWrapper::AdsObjectMapInsert()
{
    AcResBufPtr pArgs(acedGetArgs());

    /* int argNum = 0;
     int argNumFound = 0;

     for (resbuf* pTail = pArgs.get(); pTail != nullptr; pTail = pTail->rbnext)
     {
         if (argNum == 0)
         {
             switch (pTail->restype)
             {

             };
         }
     }*/
    return RSRSLT;
}
