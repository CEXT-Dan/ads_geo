#pragma once


class AdsObjectMapWrapper
{
public:
    static int AdsObjectMapcreate();
    static int AdsObjectMapInsert();


private:
    inline static AdsObjectMap pMap;
};

