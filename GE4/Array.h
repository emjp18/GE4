#pragma once
#ifndef ARRAY_H
#define ARRAY_H
#define ll long long
using namespace std;
namespace CUSTOM_CONTAINER
{
   

    // Template class to create vector of
    // different data_type
    template <typename DT>
    class Array {
    private:
        DT* arr;

        // Variable to store the current capacity
        // of the vector
        ll capacity;

        // Variable to store the length of the
        // vector
        ll length;

    public:
        explicit Array(ll = 100);
        
        virtual ~Array();
        DT* Data();
        // Function that returns the number of
        // elements in array after pushing the data
        ll push_back(DT);
        void clear();
        // function that returns the popped element
        DT pop_back();

        // Function that return the size of vector
        ll size() const;
        DT& operator[](ll);

        // Iterator Class
        class iterator {
        private:
            // Dynamic array using pointers
            DT* ptr;

        public:
            explicit iterator()
                : ptr(nullptr)
            {
            }
            explicit iterator(DT* p)
                : ptr(p)
            {
            }
            bool operator==(const iterator& rhs) const
            {
                return ptr == rhs.ptr;
            }
            bool operator!=(const iterator& rhs) const
            {
                return !(*this == rhs);
            }
            DT operator*() const
            {
                return *ptr;
            }
            iterator& operator++()
            {
                ++ptr;
                return *this;
            }
            iterator operator++(int)
            {
                iterator temp(*this);
                ++* this;
                return temp;
            }
        };

        // Begin iterator
        iterator begin() const;
        ll findIndex(DT) const;
        iterator find(DT) const;
        // End iterator
        iterator end() const;
    };

    // Template class to return the size of
    // vector of different data_type
    template <typename DT>
    Array<DT>::Array(ll n)
        : capacity(n), arr(new DT[n]), length(0)
    {
    }

    template<typename DT>
    inline Array<DT>::~Array()
    {
        delete[]arr;
    }

    template<typename DT>
    inline DT* Array<DT>::Data()
    {
        return arr;
    }
   
    // Template class to insert the element
    // in vector
    template <typename DT>
    ll Array<DT>::push_back(DT data)
    {
        if (length == capacity) {
            DT* old = arr;
            arr = new DT[capacity = capacity * 2];
            copy(old, old + length, arr);
            delete[] old;
        }
        arr[length++] = data;
        return length;
    }

    template<typename DT>
    inline void Array<DT>::clear()
    {
        delete[]arr;
        ll n= 100;
        capacity = n, arr = new DT[n], length = 0;
    }

   

    // Template class to return the popped element
    // in vector
    template <typename DT>
    DT Array<DT>::pop_back()
    {
        return arr[length-- - 1];
    }

    // Template class to return the size of
    // vector
    template <typename DT>
    ll Array<DT>::size() const
    {
        return length;
    }

    // Template class to return the element of
    // vector at given index
    template <typename DT>
    DT& Array<DT>::operator[](ll index)
    {
        // if given index is greater than the
        // size of vector print Error
        if (index >= length) {
            
            return *(arr + length);
        }

        // else return value at that index
        return *(arr + index);
    }

    // Template class to return begin iterator
    template <typename DT>
    typename Array<DT>::iterator
        Array<DT>::begin() const
    {
        return iterator(arr);
    }

    // Template class to return end iterator
    template <typename DT>
    typename Array<DT>::iterator
        Array<DT>::end() const
    {
        return iterator(arr + length);
    }
   
    template <typename DT>
    typename Array<DT>::iterator
        Array<DT>::find(DT key) const
    {
       
        iterator it = begin();
        while (it != end())
        {
            if ((*it) == key)
            {
                return it;
                
            }
            else
            {
                it++;
            }
            
        }
        return end();
    }
    template <typename DT>
    ll
        Array<DT>::findIndex(DT key) const
    {
        iterator it = begin();
        ll index = 0;
        while (it != end())
        {
            if ((*it) == key)
            {
                return index;

            }
            else
            {
                it++;
                index++;
            }

        }
        return -1;
    }
}
#endif //ARRAY_H