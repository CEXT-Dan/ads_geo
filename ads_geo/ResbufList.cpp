#include "StdAfx.h"
#include "ResbufList.h"

//************************************
// DRM        [11/4/2010]
// Method:    ResbufList
// FullName:  ResbufList::ResbufList
// Access:    public 
// Returns:   
// Parameter: void
//************************************
ResbufList::ResbufList(void) : mpHead(0), mpTail(0) {}

//************************************
// DRM        [11/4/2010]
// Method:    ResbufList ctor : Takes ownership of the source chain
// FullName:  ResbufList::ResbufList
// Access:    public 
// Returns:   
// Parameter: resbuf *src
//************************************
ResbufList::ResbufList(resbuf* src) : mpHead(src), mpTail(0)
{
    mpTail = tail(mpHead);
}

//************************************
// DRM        [11/4/2010]
// Method:    ResbufList cctor : Creates a new copy of the source list
// FullName:  ResbufList::ResbufList
// Access:    public 
// Returns:   
// Qualifier: : mpHead(0), mpTail(0)
// Parameter: const ResbufList & rhs
//************************************
ResbufList::ResbufList(const ResbufList& rhs) : mpHead(0), mpTail(0)
{
    if (rhs.mpHead)
    {
        this->mpHead = cloneChain(rhs.mpHead);
        this->mpTail = tail(this->mpHead);
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    ~ResbufList dtor
// FullName:  ResbufList::~ResbufList
// Access:    public 
// Returns:   
// Qualifier:
// Parameter: void
//************************************
ResbufList::~ResbufList(void)
{
    if (mpHead != 0)
    {
        acutRelRb(mpHead);
        mpHead = 0;
        mpTail = 0;
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    operator= : Takes ownership of the source chain
// FullName:  ResbufList::operator=
// Access:    public 
// Returns:   ResbufList&
// Qualifier:
// Parameter: resbuf *pRb
//************************************
ResbufList& ResbufList::operator=(resbuf* pRb) noexcept
{
    if (this->mpHead != pRb)
    {
        this->removeAll();
        if (pRb)
        {
            this->mpHead = pRb;
            this->mpTail = tail(this->mpHead);
        }
    }
    return *this;
}

//************************************
// DRM        [11/4/2010]
// Method:    operator= : creates a new copy of the source list
// FullName:  ResbufList::operator=
// Access:    public 
// Returns:   ResbufList&
// Qualifier:
// Parameter: const ResbufList & src 
//************************************
ResbufList& ResbufList::operator=(const ResbufList& src) noexcept
{
    if (this->mpHead != src.mpHead)
    {
        this->removeAll();
        if (src.mpHead)
        {
            this->mpHead = cloneChain(src.mpHead);
            this->mpTail = tail(this->mpHead);
        }
    }
    return *this;
}

//************************************
// DRM        [11/4/2010]
// Method:    operator const resbuf*
// FullName:  ResbufList::operator const resbuf*
// Access:    public 
// Returns:   
// Qualifier: const
// Parameter: void
//************************************
ResbufList::operator const resbuf* (void) const noexcept
{
    return mpHead;
}

//************************************
// DRM        [11/4/2010]
// Method:    at
// FullName:  ResbufList::at
// Access:    public 
// Returns:   ResbufList_iterator at the index idx
// Qualifier:
// Parameter: const size_t idx
//************************************
ResbufList_iterator ResbufList::at(const size_t idx) noexcept
{
    size_t nItems = 0;
    for (resbuf* pRbTemp = mpHead; pRbTemp != NULL; pRbTemp = pRbTemp->rbnext, ++nItems)
    {
        if (idx == nItems)
            return pRbTemp;
    }
    return NULL;
}

//************************************
// DRM        [11/4/2010]
// Method:    cloneChain : creates a copy of the source chain
// FullName:  ResbufList::cloneChain
// Access:    private static 
// Returns:   resbuf*
// Qualifier:
// Parameter: const resbuf * pRb
//************************************
resbuf* ResbufList::cloneChain(const resbuf* pRb) noexcept
{
    if (pRb == NULL)
        return NULL;
    // get the first node
    resbuf* pRbRet = cloneNode(pRb);
    resbuf* pRbTail = pRbRet;
    resbuf* pRbTmp = NULL;


    for (const resbuf* pRbIter = pRb->rbnext;
        pRbIter != NULL;
        pRbIter = pRbIter->rbnext)
    {
        pRbTmp = cloneNode(pRbIter);
        if (pRbTmp)
            pRbTail = pRbTail->rbnext = pRbTmp;
    }
    return pRbRet;
}

//************************************
// DRM        [11/4/2010]
// Method:    cloneNode : creates a new copy of the source node
// FullName:  ResbufList::cloneNode
// Access:    private static 
// Returns:   resbuf*
// Qualifier:
// Parameter: const resbuf * pRb
//************************************
resbuf* ResbufList::cloneNode(const resbuf* pRb) noexcept
{
    if (pRb == NULL)
        return NULL;

    struct resbuf* pRbTmp = NULL;

    if (pRb->restype > 5000)
    {
        if (pRb->restype == RTSTR)
        {
            pRbTmp = acutNewRb(pRb->restype);
            if (pRbTmp)
                pRbTmp->resval.rstring = _tcsdup(pRb->resval.rstring);
            return pRbTmp;
        }
        else
        {
            pRbTmp = acutNewRb(pRb->restype);
            if (pRbTmp)
                memcpy(&pRbTmp->resval, &pRb->resval, sizeof(pRbTmp->resval));
            return pRbTmp;
        }
    }
    else
    {
        AcDb::DwgDataType typ = acdbGroupCodeToType(pRb->restype);
        if (typ == kDwgText)
        {
            pRbTmp = acutNewRb(pRb->restype);
            if (pRbTmp)
                pRbTmp->resval.rstring = _tcsdup(pRb->resval.rstring);
            return pRbTmp;
        }
        else if (typ == kDwgBChunk)
        {
            pRbTmp = acutNewRb(pRb->restype);
            if (pRbTmp)
            {
                pRbTmp->resval.rbinary.clen = pRb->resval.rbinary.clen;
                pRbTmp->resval.rbinary.buf =
                    static_cast<char*>(malloc(pRb->resval.rbinary.clen));
                if (pRbTmp->resval.rbinary.buf)
                    memcpy(pRbTmp->resval.rbinary.buf,
                        pRb->resval.rbinary.buf, pRb->resval.rbinary.clen);
            }
            return pRbTmp;
        }
        else
        {
            pRbTmp = acutNewRb(pRb->restype);
            if (pRbTmp)
                memcpy(&pRbTmp->resval, &pRb->resval, sizeof(pRbTmp->resval));
            return pRbTmp;
        }
    }
    return NULL;
}

//************************************
// DRM        [11/4/2010]
// Method:    empty
// FullName:  ResbufList::empty
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: void
//************************************
bool ResbufList::empty(void)  const noexcept
{
    return (mpHead == NULL);
}

//************************************
// DRM        [11/4/2010]
// Method:    detach : detaches the specified node from the chain 
// FullName:  ResbufList::detach
// Access:    private 
// Returns:   resbuf*
// Qualifier:
// Parameter: resbuf * nodeToDel
//************************************
resbuf* ResbufList::detach(resbuf* nodeToDel) noexcept
{
    // trap for safety
    if ((mpHead == NULL) || (nodeToDel == NULL))
        return NULL;

    // see if it is first on the list
    if (mpHead == nodeToDel)
    {
        mpHead = mpHead->rbnext;
        nodeToDel->rbnext = NULL;
        if (mpHead == NULL)
            mpTail = NULL;    // removed only element

        return nodeToDel;
    }
    // node is somewhere in the middle or end of the list
    resbuf* rb = mpHead;

    while (rb->rbnext)
    {
        if (rb->rbnext == nodeToDel)
        {
            if (mpTail == nodeToDel)    // don't lose tail ptr
                mpTail = rb;

            rb->rbnext = nodeToDel->rbnext;
            nodeToDel->rbnext = NULL;
            return(nodeToDel);
        }
        rb = rb->rbnext;
    }
    return NULL;
}

//************************************
// DRM        [11/4/2010]
// Method:    detach  detach : detaches the specified nodes from the chain 
// FullName:  ResbufList::detach
// Access:    private 
// Returns:   resbuf*
// Qualifier:
// Parameter: resbuf * startPtr
// Parameter: resbuf * endPtr
//************************************
resbuf* ResbufList::detach(resbuf* startPtr, resbuf* endPtr) noexcept
{
    // trap safety case
    if ((mpHead == NULL) || (startPtr == NULL) || (endPtr == NULL))
        return NULL;

    // see if it is first on the list
    if (mpHead == startPtr)
    {
        mpHead = endPtr->rbnext;
        endPtr->rbnext = NULL;

        if (mpHead == NULL)
            mpTail = NULL;    // removed only element

        return startPtr;
    }
    // node is somewhere in the middle or end of the list
    resbuf* rb = mpHead;

    while (rb->rbnext)
    {
        if (rb->rbnext == startPtr)
        {
            if (mpTail == endPtr)    // don't lose tail ptr
                mpTail = rb;

            rb->rbnext = endPtr->rbnext;
            endPtr->rbnext = NULL;
            return startPtr;
        }
        rb = rb->rbnext;
    }
    return NULL;
}

//************************************
// DRM        [11/4/2010]
// Method:    tail : returns the last node of this
// FullName:  ResbufList::tail
// Access:    private static 
// Returns:   ResbufList_iterator
// Qualifier:
// Parameter: void
//************************************
ResbufList_iterator ResbufList::tail(void) noexcept
{
    return mpTail = tail(mpHead);
}

//************************************
// DRM        [11/4/2010]
// Method:    tail : returns the last node of src
// FullName:  ResbufList::tail
// Access:    private static 
// Returns:   resbuf*
// Qualifier:
// Parameter: const resbuf * src
//************************************
resbuf* ResbufList::tail(const resbuf* src) noexcept
{
    if (src != 0)
    {
        resbuf* pItr = const_cast<resbuf*>(src);
        while (pItr->rbnext != NULL)
        {
            pItr = pItr->rbnext;
        }
        return pItr;
    }
    return 0;
}

//************************************
// DRM        [11/4/2010]
// Method:    head
// FullName:  ResbufList::head
// Access:    public 
// Returns:   ResbufList_iterator
// Qualifier:
// Parameter: void
//************************************
ResbufList_iterator ResbufList::head(void) const noexcept
{
    return mpHead;
}

//************************************
// DRM        [11/4/2010]
// Method:    size
// FullName:  ResbufList::size
// Access:    public 
// Returns:   size_t
// Qualifier:
// Parameter: void
//************************************
size_t ResbufList::size(void) const noexcept
{
    if (mpHead == NULL)
        return 0;

    size_t nItems = 0;
    for (const resbuf* pRbTemp = mpHead;
        pRbTemp != NULL;
        pRbTemp = pRbTemp->rbnext)
    {
        nItems++;
    }
    return nItems;
}

//************************************
// DRM        [11/4/2010]
// Method:    pop_front
// FullName:  ResbufList::pop_front
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void ResbufList::pop_front(void) noexcept
{
    if (mpHead != 0)
    {
        resbuf* pDel = mpHead;
        resbuf* pRest = pDel->rbnext;
        pDel->rbnext = 0;
        acutRelRb(pDel);
        pDel = 0;
        mpHead = pRest;
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    pop_back
// FullName:  ResbufList::pop_back
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void ResbufList::pop_back(void) noexcept
{
    if (mpHead != 0)
    {
        resbuf* pItrBuf = mpHead;
        mpTail = 0;
        while (pItrBuf->rbnext != NULL)
        {
            mpTail = pItrBuf;
            pItrBuf = pItrBuf->rbnext;
        }
        if (mpTail)
            mpTail->rbnext = NULL;
        if (pItrBuf)
        {
            acutRelRb(pItrBuf);
            pItrBuf = 0;
        }
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    append_back : appends a chain to this, takes ownership
// FullName:  ResbufList::append
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: resbuf * src
//************************************
void ResbufList::append_back(resbuf* src) noexcept
{
    if (mpHead != 0)
    {
        mpTail->rbnext = src;
        mpTail = this->tail(mpHead);
    }
    else
    {
        mpHead = src;
        mpTail = tail(mpHead);
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    append_back : appends a list to this, creates a new copy of src
// FullName:  ResbufList::append
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const ResbufList & src
//************************************
void ResbufList::append_back(const ResbufList& src) noexcept
{
    append_back(cloneChain(src));
}


//************************************
// DRM        [11/10/2010]
// Method:    push_back : adds a single node 
// FullName:  ResbufList::push_back
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: resbuf * src
// Note: Not to be used with a chain, use append instead
//************************************
void ResbufList::push_back(resbuf* src) noexcept
{
    if (src)
    {
        if (mpHead == 0)
        {
            mpTail = mpHead = src;
            mpHead->rbnext = 0;
        }
        mpTail = mpTail->rbnext = src;
        mpTail->rbnext = 0;
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    append_front : appends a chain to the front of this, takes ownership 
// FullName:  ResbufList::append_front
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: resbuf * src
//************************************
void ResbufList::append_front(resbuf* src) noexcept
{
    if (mpHead != 0)
    {
        if (src != 0)
        {
            resbuf* pNewHead = src;
            resbuf* pNewTail = tail(src);
            pNewTail->rbnext = mpHead;
            mpHead = pNewHead;
            mpTail = tail(mpHead);
        }
    }
    else
    {
        mpHead = src;
        mpTail = tail(mpHead);
    }
}

//************************************
// DRM        [11/4/2010]
// Method:    append_front  : appends a copy of the source list to the front of this
// FullName:  ResbufList::append_front
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const ResbufList & src
//************************************
void ResbufList::append_front(const ResbufList& src) noexcept
{
    append_front(cloneChain(src));
}

//************************************
// DRM        [11/4/2010]
// Method:    release
// FullName:  ResbufList::release
// Access:    public 
// Returns:   ResbufList_iterator
// Qualifier:
// Parameter: void
//************************************
ResbufList_iterator ResbufList::release(void) noexcept
{
    resbuf* ptmp = mpHead;
    mpHead = 0;
    mpTail = 0;
    return ptmp;
}

//************************************
// DRM        [11/4/2010]
// Method:    remove
// FullName:  ResbufList::remove
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: ResbufList_iterator & nodeToDel
//************************************
void ResbufList::remove(ResbufList_iterator& nodeToDel) noexcept
{
    resbuf* rbTail = nodeToDel->rbnext;
    resbuf* rb = detach(const_cast<resbuf*>((const resbuf*)nodeToDel));
    if (rbTail)
        nodeToDel.mptr = rbTail;
    if (rb)
    {
        acutRelRb(rb);
        rb = NULL;
    }
    mpTail = tail(mpHead);
}


//************************************
// DRM        [11/4/2010]
// Method:    removeAt
// FullName:  ResbufList::removeAt
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: const size_t nItem
//************************************
void ResbufList::removeAt(const size_t nItem) noexcept
{
    resbuf* _pRb = NULL;
    size_t idx = 0;
    for (resbuf* pRbTemp = mpHead; pRbTemp != NULL; pRbTemp = pRbTemp->rbnext, idx++)
    {
        if (nItem == idx)
            _pRb = detach(pRbTemp);
    }
    if (_pRb)
    {
        acutRelRb(_pRb);
        _pRb = NULL;
    }
    mpTail = tail(mpHead);
}

//************************************
// DRM        [11/4/2010]
// Method:    removeAll
// FullName:  ResbufList::removeAll
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void ResbufList::removeAll(void) noexcept
{
    acutRelRb(mpHead);
    mpHead = 0;
    mpTail = 0;
}


//************************************
// DRM        [11/4/2010]
// Method:    reverse
// FullName:  ResbufList::reverse
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************
void ResbufList::reverse(void) noexcept
{
    resbuf* next = 0;
    resbuf* curr = mpHead;
    resbuf* prev = 0;
    while (curr != 0)
    {
        next = curr->rbnext;
        curr->rbnext = prev;
        prev = curr;
        curr = next;
    }
    mpHead = prev;
    mpTail = tail(mpHead);
}
