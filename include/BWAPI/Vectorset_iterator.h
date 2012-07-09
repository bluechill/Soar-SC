#pragma once

#define _ITER_COMPARE(op) bool operator op(const iterator<_T> &other) const     \
                          { return this->__val op &other; };

namespace BWAPI
{
  /// @~English
  /// The iterator class template allows the iteration
  /// of elements of a Vectorset with ease while
  /// maintaining the compatibility with any familiar
  /// STL container iteration.
  ///
  /// @~
  /// @see Vectorset
  template<typename _T>
  class iterator
  {
  public:
    // constructors
    iterator(_T *ptr = nullptr) : __val(ptr) {};
    iterator(const iterator<_T> &other) : __val(&other) {};

    // comparison operators
    bool operator ==(const _T &element) const
    {
      return *this->__val == element;
    };
    bool operator !=(const _T &element) const
    {
      return *this->__val != element;
    };

    _ITER_COMPARE(==);
    _ITER_COMPARE(!=);
    _ITER_COMPARE(<);
    _ITER_COMPARE(>);
    _ITER_COMPARE(<=);
    _ITER_COMPARE(>=);

    // modification operators
    iterator &operator ++()
    {
      ++__val;
      return *this;
    };
    iterator operator ++(int)
    {
      iterator copy(*this);
      ++__val;
      return copy;
    };
    iterator &operator --()
    {
      --__val;
      return *this;
    };
    iterator operator --(int)
    {
      iterator copy(*this);
      --__val;
      return copy;
    };
    iterator operator +(int val) const
    {
      return iterator(this->__val + val);
    };
    iterator operator -(int val) const
    {
      return iterator(this->__val - val);
    };
    iterator &operator +=(int val)
    {
      this->__val += val;
      return *this;
    };
    iterator &operator -=(int val)
    {
      this->__val -= val;
      return *this;
    };

    // Casting operators
    _T operator *() const
    {
      return *__val;
    };
    _T *operator &() const
    {
      return __val;
    };
    _T operator ->() const
    {
      return *__val;
    };
  private:
    _T *__val;
  };


}
