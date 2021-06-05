#ifndef DEF_H_
#define DEF_H_
#include <cstdint>
#include <cstddef>

#ifdef USE_32BIT
typedef int32_t Int;
typedef float Float;
typedef uint32_t UInt;
#else
typedef int64_t Int;
typedef double Float;
typedef uint64_t UInt;
#endif

template<typename T>
inline void swap(T* data, const size_t& i, const size_t& j)
{
    T a = data[i];
    data[i] = data[j];
    data[j]= a;
}

class Min
{
  private:
    Min();

  public:
    template<typename T> static bool prior(const T& a, const T& b)
    {
        return (a < b);
    }
};

class Max
{
  private:
    Max();

  public:
    template<typename T> static bool prior(const T& a, const T& b)
    {
        return (a >= b);
    }
};

typedef struct Edge
{
    Int x;
    Float w;
} Edge;

typedef struct EdgeTuple
{
    Int x,y;
    Float w;
} EdgeTuple;

class EdgeTupleMin
{
  private:
    EdgeTupleMin();
  public:
    static bool prior(const EdgeTuple& a, const EdgeTuple& b)
    {
        if(a.x < b.x)
            return true;
        else if(a.x > b.x)
            return false;
        else if (a.y <= b.y)
            return true;
        else 
            return false;
    }
};

#ifdef USE_32BIT

const int tab[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31};

#else

const int tab[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5};

#endif

inline int log2 (UInt value)
{
    value |= value >> 1u;
    value |= value >> 2u;
    value |= value >> 4u;
    value |= value >> 8u;
    value |= value >> 16u;
    #ifndef USE_32BIT
    value |= value >> 32u;
    #endif

    #ifdef USE_32BIT
    return tab[(uint32_t)(value*0x07C4ACDD) >> 27];
    #else
    return tab[((uint64_t)((value - (value >> 1))*0x07EDD5E59A4E28C2)) >> 58];
    #endif
}

inline int hibit(UInt n) 
{
    n |= (n >>  1u);
    n |= (n >>  2u);
    n |= (n >>  4u);
    n |= (n >>  8u);
    n |= (n >> 16u);
    #ifndef USE_32BIT
    n |= (n >> 32u);
    #endif
    return n - (n >> 1);
}

#endif
