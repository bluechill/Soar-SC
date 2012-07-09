#pragma once
#include <functional>
#include <limits.h>

#include "UnaryFilter.h"

#define BWAPI_COMPARE_FILTER_OP(op) UnaryFilter<_Param> operator op(const _Cmp &cmp) const               \
                                    {   return [&](_Param u)->bool{ return (*this)(u) op cmp; };   }

#define BWAPI_ARITHMATIC_FILTER_OP(op) template <typename _T>                                            \
                                       CompareFilter<_Param,_Cmp> operator op(const _T &other) const     \
                                       {   return [&](_Param u)->int{ return (*this)(u) op other(u); };   }

namespace BWAPI
{
  /// @~English
  /// The CompareFilter is a temporary container
  /// in which the stored function predicate
  /// returns a value. If any relational
  /// operators are used, then it becomes
  /// a UnaryFilter.
  ///
  /// @~
  template<typename _Param, typename _Cmp = int>
  class CompareFilter
  {
  private:
    std::function<_Cmp(_Param)> pred;
  public:
    // Constructor
    template <typename _T>
    CompareFilter(const _T &predicate) : pred(predicate)
    {};

    // Assignment
    template <typename _T>
    CompareFilter &operator =(const _T& other)
    {
      this->pred.assign(other);
      return *this;
    };

    // Comparisons
    BWAPI_COMPARE_FILTER_OP(==);
    BWAPI_COMPARE_FILTER_OP(!=);
    BWAPI_COMPARE_FILTER_OP(<=);
    BWAPI_COMPARE_FILTER_OP(>=);
    BWAPI_COMPARE_FILTER_OP(<);
    BWAPI_COMPARE_FILTER_OP(>);

    BWAPI_ARITHMATIC_FILTER_OP(+);
    BWAPI_ARITHMATIC_FILTER_OP(-);
    BWAPI_ARITHMATIC_FILTER_OP(|);
    BWAPI_ARITHMATIC_FILTER_OP(&);
    BWAPI_ARITHMATIC_FILTER_OP(*);
    BWAPI_ARITHMATIC_FILTER_OP(^);

    template <typename _T>
    CompareFilter<_Param,_Cmp> operator /(const _T &other) const
    {   
      return [&](_Param u)->int{ int rval = other(u);
                                 return rval == 0 ? INT_MAX : (*this)(u) / rval;
                               };
    };
    template <typename _T>
    CompareFilter<_Param,_Cmp> operator %(const _T &other) const
    {   
      return [&](_Param u)->int{ int rval = other(u);
                                 return rval == 0 ? 0 : (*this)(u) % rval;
                               };
    };

    // call
    inline _Cmp operator()(_Param u) const
    {
      return pred(u);
    };
    
    inline operator bool() const
    {
      return (bool)pred;
    };
  };
}

