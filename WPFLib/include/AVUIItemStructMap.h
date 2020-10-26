#pragma once

namespace AVUI {

template<class T>
class ItemStructMap : public Object
{
public:
    ItemStructMap() : m_capacity(0), m_count(0), m_rgEntries(NULL) {}

    ~ItemStructMap()
    {
        EraseData();
    }

    void EraseData()
    {
        if(m_rgEntries != NULL)
        {
            delete[] m_rgEntries;
        }

        m_rgEntries = NULL;
        m_count = 0;
        m_capacity = 0;
    }

    unsigned int get_Count() const { return m_count; }

    struct Entry
    {
        int Key;
        T Value;

        void* operator new (size_t size) { return Object::AllocateMemory(size); }
        void operator delete (void *p) { Object::FreeMemory(p); }

        void* operator new[] (size_t size) { return Object::AllocateMemory(size); }
        void operator delete[] (void *p) { Object::FreeMemory(p); }
    };

    void RemoveEntry(int key)
    {
        int index = Search(key);

        if(index < 0)
        {
            return;
        }

        for (int i = index; i < m_count - 1; i++)
        {
            m_rgEntries[i] = m_rgEntries[i + 1];
        }

        m_rgEntries[m_count - 1] = Entry();

        m_count -= 1;
    }

    int EnsureEntry(int key)
    {
        int index = Search(key);

        if (index < 0)
        {
            if (m_rgEntries == NULL)
            {
                m_capacity = 4;
                m_rgEntries = new Entry[m_capacity];
            }
            index = ~index;

            Entry* rgEntries = m_rgEntries;

            if (m_count == m_capacity)
            {
                m_capacity = m_capacity * 2;

                rgEntries = new Entry[m_capacity];
                for (int i = 0; i < index; i++)
                {
                    rgEntries[i] = m_rgEntries[i];
                }
            }

            for (int i = m_count - 1; i >= index; i--)
            {
                rgEntries[i + 1] = m_rgEntries[i];
            }

            if (m_rgEntries != rgEntries)
            {
                delete[] m_rgEntries;
                m_rgEntries = rgEntries;
            }

            m_rgEntries[index].Value = T();
            m_rgEntries[index].Key = key;
            m_count += 1;
        }

        return index;
    }


    int Search(int key) const
    {
        int currentKey = 0x7fffffff;
        int index = 0;
        if (m_count > 4)
        {
            int left = 0;
            int right = m_count - 1;
            while (left <= right)
            {
                index = (left + right) / 2;
                currentKey = m_rgEntries[index].Key;
                if (key == currentKey)
                {
                    return index;
                }
                if (key < currentKey)
                {
                    right = index - 1;
                }
                else
                {
                    left = index + 1;
                }
            }
        }
        else
        {

            for(int i = 0; i < m_count; i++)
            {
                index = i;

                currentKey = m_rgEntries[index].Key;

                if(key == currentKey)
                {
                    return index;
                }

                if(key < currentKey)
                {
                    break;
                }
            }
        }
        if (key > currentKey)
        {
            index++;
        }
        return ~index;
    }

    bool Contains(int key) const
    {
        if(m_count == 0)
        {
            return false;
        }

        int index = Search(key);

        if(index < 0)
        {
            return false;
        }

        return true;
    }

    ItemStructMap(const ItemStructMap<T>& other)
    {
        m_rgEntries = NULL;
        m_count = 0;
        m_capacity = 0;

        if(other.m_rgEntries != NULL)
        {
            m_count = other.m_count;
            m_capacity = other.m_capacity;
            m_rgEntries = new Entry[m_capacity];

            for(int i = 0; i < m_count; i++)
            {
                m_rgEntries[i] = other.m_rgEntries[i];
            }
        }
    }

    ItemStructMap& operator=(const ItemStructMap<T>& other)
    {
        if(&other == this)
        {
            return *this;
        }

        if(m_rgEntries != NULL)
        {
            delete[] m_rgEntries;
            m_rgEntries = NULL;
            m_count = 0;
            m_capacity = 0;
        }

        if(other.m_rgEntries != NULL)
        {
            m_count = other.m_count;
            m_capacity = other.m_capacity;
            m_rgEntries = new Entry[m_capacity];

            for(int i = 0; i < m_count; i++)
            {
                m_rgEntries[i] = other.m_rgEntries[i];
            }
        }

        return *this;
    }


    const T& operator [] (unsigned int i) const { return m_rgEntries[i].Value; }
    T& operator [] (unsigned int i) { return m_rgEntries[i].Value; }

private:
    int m_capacity;
    int m_count;
    Entry* m_rgEntries;

};

};
