#pragma once
#include <stdlib.h>
#include <string.h>
#include <type_traits>

#include "Vectorset_iterator.h"

namespace BWAPI
{
  template<typename _T>
  class Vectorset;

  /// @~English
  /// The Vectorset is a class template designed specifically for trivial classes or PODs and 
  /// performance. It mimics the usage of various stl containers (specifically the Vector and Set)
  /// in order to replace them. The Vectorset is designed for BWAPI usage and is recommended, 
  /// especially if stl containers are causing a bottleneck in your bot's code.
  ///
  /// @warning The Vectorset should only be used with objects that have a trivial destructor and 
  /// trivial copy constructor. Objects in the Vectorset should not do any memory management 
  /// or the behaviour will be undefined. Use with caution.
  ///
  /// @note The Vectorset will only free memory when the object is destroyed.
  /// @~
  /// @see std::vector, std::set
  template<typename _T>
  class ConstVectorset
  {
#ifndef SWIG
    static_assert(std::has_trivial_copy<_T>::value == true &&
                  std::has_trivial_copy_constructor<_T>::value == true &&
                  std::has_trivial_destructor<_T>::value == true,
                  "Vectorset can only be used with classes that have a trivial destructor and trivial copy constructor.");
#endif
  private:
    ConstVectorset();
  public:
    /// @~English
    /// The iterator is used to traverse the elements in the set. It is used the same way as in
    /// the stl containers.
    /// @~
    typedef iterator<_T> iterator;

  // ----------------------------------------------------------------- Constructors
    /// @~English
    /// This constructor uses an existing array of objects and copies them into the vector.
    /// The Vectorset will allocate only enough memory to copy the array's contents.
    ///
    /// @param pArray
    ///   Pointer to an array of objects of type _T.
    /// @param arrSize
    ///   The number of elements contained in the given array.
    ///
    /// @note Duplicate entries are not removed.
    /// @~
    ConstVectorset(const void *pArray, size_t arrSize = 0)
      : pStartArr( (_T*)pArray )
      , pEndArr( (_T*)pArray + arrSize )
    {};
    
    ConstVectorset(const ConstVectorset &other)
      : pStartArr(other.pStartArr)
      , pEndArr(other.pEndArr)
    {}

  // ----------------------------------------------------------------- Operators
    /// @~English
    /// Creates a new Vectorset containing all elements of the current Vectorset with all elements
    /// of the other Vectorset appended on to it.
    ///
    /// @param other
    ///   The other Vectorset of the same type to use in combination.
    ///
    /// @returns A new Vectorset containing the contents of both this and other.
    ///
    /// @note Duplicate entries are not removed.
    /// @~
    /// @see operator|
    Vectorset<_T> operator +(const ConstVectorset<_T> &other) const
    {
      Vectorset<_T> vcopy(this->size() + other.size());
      vcopy.push_back(*this);
      vcopy.push_back(other);
      return vcopy;
    };
    Vectorset<_T> operator +(const _T &val) const
    {
      Vectorset<_T> vcopy(this->size() + other.size());
      vcopy.push_back(*this);
      vcopy.push_back(val);
      return vcopy;
    };

    /// @~English
    /// Creates a new Vectorset containing all elements of the current Vectorset and all elements
    /// of the other Vectorset with duplicates removed.
    ///
    /// @param other
    ///   The other Vectorset of the same type to use in combination.
    ///
    /// @returns A new Vectorset containing the contents of both this and other.
    /// @~
    Vectorset<_T> operator |(const ConstVectorset<_T> &other) const
    {
      Vectorset<_T> vcopy(this->size() + other.size());
      vcopy.push_back(*this);
      vcopy.insert(other);
      return vcopy;
    };
    Vectorset<_T> operator |(const _T &val) const
    {
      Vectorset<_T> vcopy(this->size() + 1);
      vcopy.push_back(*this);
      vcopy.insert(val);
      return vcopy;
    };
    /// @~English
    /// Compares this Vectorset with another Vectorset of the same type.
    /// 
    /// @param other
    ///   A Vectorset of the same type to use for comparison.
    ///
    /// @retval true if the other Vectorset is exactly the same as this one.
    /// @retval false if at least one element in this Vectorset is not found in the other, or if
    /// they are not in the exact same positions.
    ///
    /// @note This simply calls memcmp.
    /// @~
    bool operator ==(const ConstVectorset<_T> &other) const
    {
      if ( this->empty() && other.empty() )
        return true;
      else if ( this->empty() || other.empty() )
        return false;

      return memcmp( this->pStartArr, other, std::min(this->size(), other.size())*sizeof(_T)) == 0;
    };
    
    /// @~English
    /// Retrieves a pointer to the raw data in the Vectorset.
    ///
    /// @note Vectorset data is simply an array.
    ///
    /// @returns A pointer to the Vectorset's array data.
    /// @~
    inline operator void*() const
    {
      return this->pStartArr;
    };
    /// @~English
    /// Used as a macro to check if the Vectorset is empty.
    ///
    /// @retval true if the Vectorset is not empty.
    /// @retval false if the Vectorset is empty.
    /// @~
    inline operator bool() const
    {
      return !this->empty();
    };
    /// @~English
    /// Array indexing operator.
    /// 
    /// @TODO Needs to be modified
    ///
    /// @param index
    ///   The array index in the Vectorset to retrieve the value from.
    /// @~
    inline _T operator [](unsigned int index) const
    {
      if ( index < this->size() )
        return this->pStartArr[index];
      return pStartArr[0];
    };

  // ----------------------------------------------------------------- Custom const functions
    /// @~English
    /// This function checks if an element exists in the Vectorset.
    ///
    /// @param element
    ///   The value to search for in the Vectorset.
    ///
    /// @retval true if the element is in the Vectorset.
    /// @retval false if the element was not found.
    /// @~
    bool exists(const _T &element) const
    {
      for ( auto i = this->begin(); i != this->end(); ++i )
      {
        if ( element == *i )
          return true;
      }
      return false;
    };
    /// @~English
    /// This macro is used to choose a random value from a Vectorset.
    ///
    /// @TODO needs to be modified
    ///
    /// @retval NULL if the Vectorset is empty.
    ///
    /// @note This function calls the rand() function. A call to srand() should be used for
    /// initialization.
    /// @~
    /// @see rand()
    _T rand() const
    {
      size_t size = this->size();
      switch ( size )
      {
      case 0:
        return NULL;
      case 1:
        return this->pStartArr[0];
      case 2:
        return this->pStartArr[::rand()%2];
      case 4:
        return this->pStartArr[::rand()%4];
      case 8:
        return this->pStartArr[::rand()%8];
      case 16:
        return this->pStartArr[::rand()%16];
      case 32:
        return this->pStartArr[::rand()%32];
      case 64:
        return this->pStartArr[::rand()%64];
      case 128:
        return this->pStartArr[::rand()%128];
      case 256:
        return this->pStartArr[::rand()%256];
      case 512:
        return this->pStartArr[::rand()%512];
      case 1024:
        return this->pStartArr[::rand()%1024];
      case 2048:
        return this->pStartArr[::rand()%2048];
      }
      return this->pStartArr[::rand()%size];
    };
    /// @~English
    /// Iterates the Vectorset and retrieves the best entry using two callback procedures. The
    /// first returning the value to compare, the second being a binary comparison.
    ///
    /// @param cmpValue
    ///   A functor taking one argument, _T, and returning a value to compare.
    ///
    /// @param cmpProc
    ///   A functor taking two values, (the ones returned by cmpValue), and returns a boolean
    ///   indicating that the first value passed in is the new best value.
    ///
    /// @retval NULL If the Vectorset is empty.
    /// @returns A _T representing the best in the Vectorset.
    /// @~
    ///
    template < typename _V, typename _C >
    _T getBest(const _V &cmpValue, const _C &cmpProc) const
    {
      // Return if empty
      if ( this->empty() )
        return NULL;
      
      // retrieve a value as the placeholder for the "best"
      _T best = this->front();
      int bestVal = cmpValue(best);

      // Iterate all (remaining) elements
      for ( auto i = this->begin()+1; i != this->end(); ++i )
      {
        // Retrieve new value
        int newVal = cmpValue(*i);

        // Set as new best if new value > best
        if ( cmpProc(newVal, bestVal) )
        {
          bestVal = newVal;
          best = *i;
        }
      }

      return best;
    };
    /// @copydoc getBest
    /// @see getBest
    template < typename _V >
    _T most(const _V &cmpValue) const
    {
      return this->getBest( std::forward<_V>(cmpValue), [](const int &v1, const int &v2)->bool{ return v1 > v2; } );
    };
    /// @copydoc getBest
    /// @see getBest
    template < typename _V >
    _T least(const _V &cmpValue) const
    {
      return this->getBest( std::forward<_V>(cmpValue), [](const int &v1, const int &v2)->bool{ return v1 < v2; } );
    };
    /// @~English
    /// Calculates a total by applying a functor to each element and adding what the functor
    /// returns.
    ///
    /// @param valProc
    ///   A unary functor that takes _T as a parameter and returns the integer used to add to
    ///   the total.
    ///
    /// @returns An integer representing the sum of results from \p valProc applied to every
    /// element in the Vectorset.
    /// @~
    template < typename _V >
    int total(const _V &valProc) const
    {
      int sum = 0;
      for ( auto i = this->begin(); i != this->end(); ++i )
        sum += valProc(*i);
      return sum;
    };
  // ----------------------------------------------------------------- stl spinoff const functions
    /// @~English
    /// Retrieves the number of elements currently in the Vectorset.
    ///
    /// @returns The number of elements contained in this Vectorset.
    ///
    /// @note For efficiency it is recommended to use empty() to check if the Vectorset has 0
    /// elements.
    /// @~
    /// @see empty
    inline size_t size() const
    {
      return ((size_t)this->pEndArr - (size_t)this->pStartArr)/sizeof(_T);
    };
    /// @~English
    /// Checks if the Vectorset is empty.
    ///
    /// @retval true if the Vectorset is empty.
    /// @retval false if the Vectorset contains elements.
    /// @~
    inline bool empty() const
    {
      return this->pEndArr == this->pStartArr;
    };

    // iterators
    inline iterator begin() const
    {
      return this->pStartArr;
    };
    inline iterator rbegin() const
    {
      return this->pEndArr - 1;
    };
    inline iterator end() const
    {
      return this->pEndArr;
    };
    inline iterator rend() const
    {
      return this->pStartArr - 1;
    };
    iterator find(const _T &element) const
    {
      for ( auto i = this->begin(); i != this->end(); ++i )
      {
        if ( element == *i )
          return i;
      }
      return this->end();
    };
    inline _T front() const
    {
      return *this->pStartArr;
    };
    inline _T back() const
    {
      return *(this->pEndArr - 1);
    };
  // ----------------------------------------------------------------- stl algorithms
    /// @~English
    /// Works similar to the STL algorithm count_if. Iterates and calls a function predicate for
    /// each element in the Vectorset. If the predicate call returns true, then a counter is
    /// incremented.
    ///
    /// @param pred Function predicate used to determine if a value is counted.
    ///
    /// @returns An integer containing the number of elements that were counted.
    /// @~
    /// @see std::count_if, count
    template <typename Func>
    int count_if( const Func &pred ) const
    {
      size_t rval = 0;
      for ( auto i = this->begin(); i != this->end(); ++i )
      {
        if ( pred(*i) )
          ++rval;
      }
      return rval;
    };
    /// @~English
    /// Works similar to the STL algorithm count. Iterates and compares each element of the
    /// Vectorset to a value. If the value matches, then a counter is incremented.
    ///
    /// @param val
    ///   The value to compare each element with.
    ///
    /// @returns An integer containing the number of elements that were counted.
    /// @~
    /// @see std::count, count_if, size
    int count(const _T &val) const
    {
      size_t rval = 0;
      for ( auto i = this->begin(); i != this->end(); ++i )
      {
        if ( *i == val )
          ++rval;
      }
      return rval;
    };
  // -----------------------------------------------------------------
  protected:
    // Variables
    _T *pStartArr;  // ptr to beginning of array
    _T *pEndArr;    // ptr to last element + 1
  };

}

