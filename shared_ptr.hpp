#ifndef __SHARED_PTR_HPP__INCLUDED__
#define __SHARED_PTR_HPP__INCLUDED__

// Quick and dirty lightweight replacement for boost::shared_ptr.
//
// This version is not based on a deleter stored with the object. If you put
// in objects of a derived class this means:
//
//      the destructor MUST be VIRTUAL!

namespace util
{

    template <class T>
    class SharedPtr
    {
        T* obj;
        int* ref;

    public:

        SharedPtr()
            : obj(0)
            , ref(0)
        {
        }

        SharedPtr(const SharedPtr& p)
            : obj(p.obj)
            , ref(p.ref)
        {
            if (ref)
                ++*ref;
        }

        explicit SharedPtr(T* p)
            : obj(p)
            , ref(new int(1))
        {
        }

        SharedPtr& operator = (const SharedPtr& p)
        {
            SharedPtr(p).swap(*this);
            return *this;
        }

        ~SharedPtr()
        {
            clear();
        }

        void reset(T* p)
        {
            SharedPtr(p).swap(*this);
        }

        void swap(SharedPtr& p)
        {
            std::swap(obj, p.obj);
            std::swap(ref, p.ref);
        }

        void clear()
        {
            if (ref && !--*ref) {
                delete ref;
                delete obj;
            }
            ref = nullptr;
            obj = nullptr;
        }

        T* get() const { return obj; }
        T* operator -> () const { return get(); }
        T& operator * () const { return get(); }
    };

}

#endif // include guard
