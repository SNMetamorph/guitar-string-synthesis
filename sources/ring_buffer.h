#pragma once
#include <vector>

template <class T> class CRingBuffer 
{
public:
    CRingBuffer() {}
    CRingBuffer(int capacity)       { SetCapacity(capacity); }
    inline int GetSize() const      { return m_iSize; }
    inline int GetFreeSize() const  { return m_iCapacity - m_iSize; }
    inline int GetCapacity() const  { return m_iCapacity; }
    inline bool IsEmpty() const     { return m_iSize == 0; }
    inline bool IsFull() const      { return m_iSize == m_iCapacity; }

    void SetCapacity(int capacity) 
    {
        m_Elements.reserve(capacity);
        m_Elements.assign(capacity, 0);
        m_iCapacity = capacity;
        m_iSize = 0;
    }

    bool Enqueue(const T &value) 
    {
        if (m_iSize < m_iCapacity)
        {
            m_Elements[m_iIndexLast] = value;
            m_iIndexLast = (m_iIndexLast + 1) % m_iCapacity;
            m_iSize++;
            return true;
        }
        return false;
    }

    bool Dequeue(T &value) 
    {
        if (m_iSize > 0)
        {
            value = m_Elements[m_iIndexFirst];
            m_iIndexFirst = (m_iIndexFirst + 1) % m_iCapacity;
            m_iSize--;
            return true;
        }
        return false;
    }

    bool Peek(T &value, int offset = 0) const
    {
        if (m_iSize > 0)
        {
            int index = (m_iIndexFirst + offset) % m_iSize;
            value = m_Elements[index];
            return true;
        }
        return false;
    }

    bool Assign(const T &value)
    {
        for (int i = 0; i < GetCapacity(); ++i)
        {
            if (!Enqueue(value))
                return false;
        }
        return true;
    }

private:
    int m_iSize = 0;
    int m_iCapacity = 0;
    int m_iIndexLast = 0;
    int m_iIndexFirst = 0;
    std::vector<T> m_Elements;
};
