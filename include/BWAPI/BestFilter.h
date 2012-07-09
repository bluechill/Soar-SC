#pragma once
#include <functional>
#include "ComparisonFilter.h"

namespace BWAPI
{
  /// @~English
  /// The BestFilter is used to compare two
  /// objects with each other. Each function
  /// object compares two of the same object
  /// and returns the most desirable one.
  ///
  /// @note: A function object should return
  /// one of the two given parameters, never NULL. 
  /// Both parameters being given should never be
  /// NULL.
  /// 
  /// @~
  template<typename _Param>
  class BestFilter
  {
  private:
    std::function<_Param(_Param,_Param)> pred;
  public:
    // Constructor
    template <typename _T>
    BestFilter(const _T &predicate) : pred(predicate)
    {};

    // Assignment
    template <typename _T>
    BestFilter<_Param> &operator =(const _T &other)
    {
      this->pred = other;
      return *this;
    };

    // Bitwise operators
    template <typename _T>
    inline BestFilter<_Param> operator &&(const _T &other) const
    {
      return [&](_Param p1, _Param p2)->_Param{ return other( (*this)(p1, p2) ); };
    };

    // call
    inline _Param operator()(const _Param &p1, const _Param &p2) const
    {
      return this->pred(p1, p2);
    };

  };

  template <typename _Param>
  BestFilter<_Param> Lowest(const CompareFilter<_Param,int> &filter)
  {
    return [&](_Param p1, _Param p2)->_Param{ return filter(p2) < filter(p1) ? p2 : p1; };
  };
  template <typename _Param>
  BestFilter<_Param> Highest(const CompareFilter<_Param,int> &filter)
  {
    return [&](_Param p1, _Param p2)->_Param{ return filter(p2) > filter(p1) ? p2 : p1; };
  };

}

