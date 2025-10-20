#pragma once

//-----------------------------------------------------------------------------------------
//AdsObjectMapWrapper
class AdsObjectMapWrapper
{
public:
    static int AdsObjectMapClear();
    static int AdsObjectMapInsert();
    static int AdsObjectMapgetat();
    static int AdsObjectMapContains();
    static int AdsObjectMapgetall();
private:
    inline static AdsObjectMap pMap;
};

//-----------------------------------------------------------------------------------------
//AdsObjectSetWrapper
class AdsObjectSetWrapper
{
public:
    static int AdsObjectSetClear();
    static int AdsObjectSetInsert();
    static int AdsObjectSetContains();
    static int AdsObjectSetgetall();
private:
    inline static AdsObjectSet pSet;
};

