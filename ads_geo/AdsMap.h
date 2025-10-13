#pragma once

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

