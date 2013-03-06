#ifndef MINMAX_H
#define MINMAX_H

#ifndef min
  template <class T>
  const T& min(const T& a, const T& b)
  {
    return (a < b) ? a : b;
  }
#endif

#ifndef max
  template<class T>
  const T& max(const T& a, const T& b)
  {
    return (b < a) ? a : b;
  }
#endif

#endif // MINMAX_H
