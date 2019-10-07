#pragma once
#include <cmath>
#include <cassert>
#include <type_traits>

template<typename T, int n>
struct inner
{
    T data[n];
};

template<typename T>
struct inner<T, 2>
{
    union {
        struct
        {
            T x, y;
        };
        T data[2];
    };
};

template<typename T>
struct inner<T, 3>
{
    union {
        struct
        {
            T x, y, z;
        };
        struct
        {
            T r, g, b;
        };
        T data[3];
    };
};

template<typename T>
struct inner<T, 4>
{
    union {
        struct
        {
            T x, y, z, w;
        };
        struct
        {
            T r, g, b, a;
        };
        T data[3];
    };
};

template<typename T, int n> 
struct vec : public inner<T, n>
{
    using inner<T, n>::data;
    
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    
    vec<T, n> operator+(const vec<T, n> other)
    {
        vec<T, n> retval = *this;
        for (int i = 0; i < n; ++i)
            retval[i] += other[i];
        return retval;
    }
    
    vec<T, n> operator-(const vec<T, n> other)
    {
        vec<T, n> retval = *this;
        for (int i = 0; i < n; ++i)
            retval[i] -= other[i];
        return retval;
    }
    
    T operator*(const vec<T, n> other)
    {
        T retval = data[0] * other[0];
        for (int i = 1; i < n; ++i)
            retval += data[i] * other[i];
        return retval;
    }
    
    template<typename S>
    vec<T, n> operator*(const S& scalar)
    {
        vec<T, n> retval = *this;
        for (int i = 0; i < n; ++i)
            retval[i] *= scalar;
        return retval;
    }
    
    template<typename S>
    const vec<T, n>& operator*=(const S& scalar)
    {
        for (int i = 0; i < n; ++i)
            data[i] *= scalar;
        return *this;
    }
    
    T length2()
    {
        T retval = data[0] * data[0];
        for (int i = 1; i < n; ++i)
            retval += data[i] * data[i];
        return retval;
    }
    
    float length()
    {
        return std::sqrt(length2());
    }
    
    vec<T, n> normalized()
    {
        float l = length();
        
        assert(l > 0);
        
        return (*this * (1 / l));
    }
    
    void normalize()
    {
        *this = normalized();
    }
};

template<typename T>
T sarea(const vec<T, 2>& a, const vec<T, 2>& b)
{
    return (a.x * b.y - a.y * b.x);
}

template<typename T>
vec<T, 3> dot(const vec<T, 3>& a, const vec<T, 3>& b)
{
    return {(a.y * b.z - b.y * a.z), 
            -(a.x * b.z - b.x * a.z), 
            (a.x * b.y - b.x * a.y)};
}

template<typename T, int n>
struct sqmat
{
    T data[n][n];
    
    T* operator[] (int i) {return data[i];}
    const T* operator[] (int i) const {return data[i];}
};

template<typename T, typename U, int n>
vec<typename std::common_type<T, U>::type, n> 
    operator*(const sqmat<U, n>& m, const vec<T, n>& v)
{
    using C = typename std::common_type<T, U>::type;
    
    vec<C, n> retval = {};
    for (int i = 0; i < n; ++i) {
        C tmp = m[i][0] * v[0];
        for (int j = 1; j < n; ++j)
            tmp += m[i][j] * v[j];
        retval[i] = tmp;
    }
    
    return retval;
}



