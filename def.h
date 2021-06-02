#ifndef DEF_H_
#define DEF_H_
#include <cstdint>
#include <cstddef>

#ifdef USE_32BIT
typedef int32_t Int;
typedef float Float;
#else
typedef int64_t Int;
typedef double Float;
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
    template<typename T> static bool less(const T& a, const T& b)
    {
        return (a < b);
    }
};

class Max
{
  private:
    Max();

  public:
    template<typename T> static bool less(const T& a, const T& b)
    {
        return (a >= b);
    }
};

#endif
