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
            T w, h;
        };
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
            T b, g, r;
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
            T b, g, r, a;
        };
        T data[4];
    };
};

template<typename T, int n> 
struct vec : public inner<T, n>
{
    using inner<T, n>::data;
    
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }
    
    inline vec<T, n> operator+(const vec<T, n>& other) const
    {
        vec<T, n> retval = *this;
        for (int i = 0; i < n; ++i)
            retval[i] += other[i];
        return retval;
    }
    
    inline vec<T, n> operator-(const vec<T, n>& other) const
    {
        vec<T, n> retval = *this;
        for (int i = 0; i < n; ++i)
            retval[i] -= other[i];
        return retval;
    }
    
    inline T operator*(const vec<T, n>& other) const
    {
        T retval = data[0] * other[0];
        for (int i = 1; i < n; ++i)
            retval += data[i] * other[i];
        return retval;
    }
    
    template<typename S>
    inline vec<T, n> operator*(const S& scalar) const
    {
        vec<T, n> retval = *this;
        for (int i = 0; i < n; ++i)
            retval[i] *= scalar;
        return retval;
    }
    
    template<typename S>
    inline const vec<T, n>& operator*=(const S& scalar)
    {
        for (int i = 0; i < n; ++i)
            data[i] *= scalar;
        return *this;
    }
    
    inline T length2() const
    {
        return (*this) * (*this);
    }
    
    inline float length() const
    {
        return std::sqrt(length2());
    }
    
    inline vec<T, n> normalized() const
    {
        float l = length();
        
        assert(l > 0);
        
        return (*this) * (1.f / l);
    }
    
    inline void normalize()
    {
        *this = normalized();
    }
};

template<typename T>
inline T sarea(const vec<T, 2>& a, const vec<T, 2>& b)
{
    return (a.x * b.y - a.y * b.x);
}

template<typename T>
inline vec<T, 3> cross(const vec<T, 3>& a, const vec<T, 3>& b)
{
    return {(a.y * b.z - b.y * a.z), 
            -(a.x * b.z - b.x * a.z), 
            (a.x * b.y - b.x * a.y)};
}

template<typename T, int n>
struct sqmat
{
    T data[n][n];
    
    inline T* operator[] (int i) {return data[i];}
    inline const T* operator[] (int i) const {return data[i];}
};

template<typename T, typename U, int n>
inline vec<typename std::common_type<T, U>::type, n> 
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

template<typename T, int n>
inline vec<T, n> normalize(const vec<T, n>& val)
{
	return val.normalized();
}

template<typename T>
inline sqmat<T, 3> rotate(const vec<T, 3>& _dir, const vec<T, 3>& _up)
{
	vec<T, 3> dir = _dir.normalized();
	vec<T, 3> right = cross(_up, dir).normalized();
	vec<T, 3> upper = cross(dir, right).normalized();
	return {{	
			{upper.x, 	upper.y, 	upper.z},
			{right.x, 	right.y, 	right.z},
			{dir.x, 	dir.y, 		dir.z}
			}};
}

typedef vec<uint8_t, 4> bgracolor_t;
typedef vec<uint16_t, 2> resolution_t;

typedef vec<uint16_t, 2> pixelcoords_t;
typedef vec<float, 2> screencoords_t;
typedef vec<float, 3> realcoords_t;

typedef vec<float, 4> vec4f;
typedef vec<float, 3> vec3f;
typedef vec<float, 2> vec2f;

typedef vec<int, 2> vec2i;

typedef sqmat<float, 3> sqmat3f;



