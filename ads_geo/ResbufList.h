// CResbufList 0.90 DRM 3/17/2010

#pragma once
#include <iterator>

//
class ResbufList_iterator;

class ResbufList
{
private:
    resbuf* mpHead;
    resbuf* mpTail;

public:
    ResbufList(void);
    ResbufList(resbuf* src);
    ResbufList(const ResbufList& rhs);
    ~ResbufList(void);

    ResbufList& operator=(resbuf* pRb) noexcept;
    ResbufList& operator=(const ResbufList& src) noexcept;
    operator     const resbuf* (void) const noexcept;

    ResbufList_iterator at(const size_t idx) noexcept;
    ResbufList_iterator head(void) const noexcept;
    ResbufList_iterator tail(void) noexcept;
    ResbufList_iterator release(void) noexcept;
    ResbufList_iterator begin() const noexcept;
    ResbufList_iterator end() const noexcept;

    bool    empty(void) const noexcept;
    size_t  size(void) const noexcept;
    void    pop_front(void) noexcept;
    void    pop_back(void) noexcept;
    void    append_back(resbuf* src) noexcept;
    void    append_back(const ResbufList& src) noexcept;
    void    push_back(resbuf* src) noexcept;
    void    append_front(resbuf* src) noexcept;
    void    append_front(const ResbufList& src) noexcept;
    void    remove(ResbufList_iterator& nodeToDel) noexcept;
    void    removeAt(const size_t idx) noexcept;
    void    removeAll(void) noexcept;
    void    reverse(void) noexcept;

private:
    resbuf* detach(resbuf* nodeToDel) noexcept;
    resbuf* detach(resbuf* startPtr, resbuf* endPtr) noexcept;
    static resbuf* cloneChain(const resbuf* pRb) noexcept;
    static resbuf* cloneNode(const resbuf* pRb) noexcept;
    static resbuf* tail(const resbuf* src) noexcept;
};

class ResbufList_iterator
    : public std::iterator<std::forward_iterator_tag, resbuf*>
{
private:
    resbuf* mptr;
    ResbufList_iterator(resbuf* ptr);
    friend class ResbufList;
public:
    resbuf* operator->(void) const noexcept;
    const ResbufList_iterator& operator++() noexcept;
    bool operator==(const ResbufList_iterator& other) const noexcept;
    bool operator==(const resbuf* other) const noexcept;
    bool operator!=(const ResbufList_iterator& other) const noexcept;
    bool operator!=(const resbuf* other) const noexcept;
    operator      const resbuf* (void) const noexcept;
};


// iterator implementation
inline ResbufList_iterator::ResbufList_iterator(resbuf* ptr) : mptr(ptr) {}

inline ResbufList_iterator ResbufList::begin() const noexcept
{
    return mpHead;
}

inline ResbufList_iterator ResbufList::end() const noexcept
{
    return NULL;
}

inline resbuf* ResbufList_iterator::operator->(void) const noexcept
{
    return mptr;
}

inline const ResbufList_iterator& ResbufList_iterator::operator++() noexcept
{
    if (mptr)
        mptr = mptr->rbnext;
    return *this;
}

inline bool ResbufList_iterator::operator==(const ResbufList_iterator& other) const noexcept
{
    return this->mptr == other.mptr;
}

inline bool ResbufList_iterator::operator==(const resbuf* other) const noexcept
{
    return this->mptr == other;
}

inline bool ResbufList_iterator::operator!=(const ResbufList_iterator& other) const noexcept
{
    return this->mptr != other.mptr;
}

inline bool ResbufList_iterator::operator!=(const resbuf* other) const noexcept
{
    return this->mptr != other;
}

inline ResbufList_iterator::operator const resbuf* (void) const noexcept
{
    return this->mptr;
}