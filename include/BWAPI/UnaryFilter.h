#pragma once
#include <functional>

namespace BWAPI
{
  template<typename _PARAM>
  class UnaryFilter
  {
  private:
    std::function<bool(_PARAM)> pred;
  public:
    // Constructor
    template <typename _T>
    UnaryFilter(const _T& predicate) : pred(predicate)
    {};

    // Assignment
    template <typename _T>
    UnaryFilter &operator =(const _T& other)
    {
      this->pred = other;
      return *this;
    };
    
    // Bitwise operators    
    template <typename _T>
    inline UnaryFilter operator &&(const _T& other) const
    {
      return [&](_PARAM u){ return (*this)(u) && other(u); };
    };
    template <typename _T>
    inline UnaryFilter operator ||(const _T& other) const
    {
      return [&](_PARAM u){ return (*this)(u) || other(u); };
    };
    
    // operator not
    inline UnaryFilter operator !() const
    {
      if ( !this->pred )
        return nullptr;
      return std::not1(this->pred);
    };

    // call
    inline bool operator()(_PARAM u) const
    {
      return pred(u);
    };

    // operator bool
    inline bool isValid() const
    {
      return (bool)pred;
    };
  };
}
