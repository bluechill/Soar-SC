#pragma once
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "Vectorset_iterator.h"
#include "ConstVectorset.h"

namespace BWAPI
{
  /// @~English
  /// The Vectorset is a class template designed specifically for trivial classes or PODs and 
  /// performance. It mimics the usage of various stl containers (specifically the Vector and Set)
  /// in order to replace them. The Vectorset is designed for BWAPI usage and is recommended, 
  /// especially if stl containers are causing a bottleneck in your bot's code.
  ///
  /// @warning The Vectorset should only be used with  objects that have a trivial destructor and 
  /// trivial copy constructor. Objects in the Vectorset should not do any memory management 
  /// or the behaviour will be undefined. Use with caution.
  ///
  /// @note The Vectorset will only free memory when the object is destroyed.
  /// @~
  /// @see std::vector, std::set
  template<typename _T>
  class Vectorset : public ConstVectorset<_T>
  {
  public:
  // ----------------------------------------------------------------- Constructors
    /// @~English
    /// This is the default constructor. The Vectorset will allocate memory for the given number
    /// of elements (or 16 by default).
    ///
    /// @param initialSize
    ///   The number of elements of type _T to allocate memory for.
    /// @~
    Vectorset(size_t initialSize = 16)
      : ConstVectorset( (_T*)malloc(initialSize*sizeof(_T)) )
      , pEndAlloc( pStartArr + initialSize )
    {};
    /// @~English
    /// This is the copy constructor. The Vectorset will allocate only the necessary space to
    /// copy the other Vectorset's contents.
    ///
    /// @param other
    ///   Reference to the Vectorset of the same type whose contents will be copied.
    ///
    /// @note Duplicate entries are not removed.
    /// @~
    Vectorset(const Vectorset<_T> &other)
      : ConstVectorset( (_T*)malloc( other.size()*sizeof(_T)), other.size() )
      , pEndAlloc( pEndArr )
    { 
      memcpy(this->pStartArr, (void*)other, other.size()*sizeof(_T));
    };
    /*
    Vectorset(const ConstVectorset<_T> &other)
      : ConstVectorset( (_T*)malloc( other.size()*sizeof(_T)), other.size() )
      , pEndAlloc( pEndArr )
    { 
      memcpy(this->pStartArr, (void*)other, other.size()*sizeof(_T));
    };*/
    /// @~English
    /// This is the move constructor. The Vectorset will steal the data pointer from the other
    /// Vectorset.
    ///
    /// @param other
    ///   Reference to the Vectorset of the same type whose contents will be moved.
    ///
    /// @note Duplicate entries are not removed.
    /// @~
    Vectorset(Vectorset<_T> &&other)
      : ConstVectorset( other.pStartArr, other.size() )
      , pEndAlloc( other.pEndAlloc )
    { 
      other.pStartArr = nullptr;
    };
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
    Vectorset(const _T *pArray, size_t arrSize)
      : ConstVectorset( (_T*)malloc(arrSize*sizeof(_T)), arrSize )
      , pEndAlloc( pEndArr )
    {
      memcpy(this->pStartArr, pArray, arrSize*sizeof(_T) );
    };

  // ----------------------------------------------------------------- Destructor
    ~Vectorset()
    {
      if ( this->pStartArr != nullptr )
      {
        free(this->pStartArr);
        this->pStartArr = nullptr;
      }
    };
  // ----------------------------------------------------------------- Operators
    /// @copydoc push_back(const Vectorset<_T> &)
    /// @~English
    /// @note The Vectorset is emptied before the contents of other are copied.
    ///
    /// @returns A reference to the current object.
    /// @~
    Vectorset<_T> &operator =(const Vectorset<_T> &other)
    {
      this->clear();
      this->push_back(other);
      return *this;
    };
    /*Vectorset<_T> &operator =(const ConstVectorset<_T> &other)
    {
      this->clear();
      this->push_back(other);
      return *this;
    };*/
    Vectorset &operator =(Vectorset<_T> &&other)
    {
      if ( this->pStartArr != nullptr )
        free(this->pStartArr);
      
      this->pStartArr = other.pStartArr;
      other.pStartArr = nullptr;

      this->pEndArr   = other.pEndArr;
      this->pEndAlloc = other.pEndAlloc;

      return *this;
    };
    /// @copydoc push_back(const Vectorset<_T> &)
    /// @~English
    /// @returns A reference to the current object.
    /// @~
    /// @see operator|=
    Vectorset &operator +=(const ConstVectorset<_T> &other)
    {
      this->push_back(other);
      return *this;
    };
    /// @copydoc insert(const Vectorset<_T> &)
    /// @~English
    /// @returns A reference to the current object.
    /// @~
    /// @see operator+=
    Vectorset &operator |=(const ConstVectorset<_T> &other)
    {
      this->insert(other);
      return *this;
    };
  // ----------------------------------------------------------------- Custom const functions
    /// @~English
    /// Retrieves the current capacity of the Vectorset.
    ///
    /// @returns The number of elements this Vectorset can contain before needing to allocate more
    /// memory.
    /// @~
    inline size_t max_size() const
    {
      return ((size_t)this->pEndAlloc - (size_t)this->pStartArr)/sizeof(_T);
    };
  // ----------------------------------------------------------------- erase
    /// @~English
    /// This function erases an element from a Vectorset. Unlike erase, it assumes there exists
    /// only one element. If the element is found, it is removed and the function immediately
    /// returns.
    ///
    /// @param val
    ///   The value to erase from the Vectorset.
    ///
    /// @note This function does not preserve order. If you wish to preserve order, see
    /// remove_once.
    /// @~
    /// @see erase
    void erase_once(const _T &val)
    {
      // iterate all elements
      iterator i = this->begin();
      for ( ; i != this->end(); ++i )
      {
        if ( val == *i )  // break if values are equal
          break;
      }
      // erase the value (also does nothing if end() has been reached)
      this->erase(i);
    };
    /// @~English
    /// Erases all values found in the Vectorset. When a value is found, it is erased and the
    /// function continues searching for the same value until it reaches the end of the
    /// Vectorset.
    ///
    /// @param val
    ///   The value to search for and erase.
    /// 
    /// @note This function does not preserve order. If you wish to preserve order, see remove.
    /// @~
    /// @see erase_once
    void erase(const _T &val)
    {
      // iterate all elements
      auto i = this->begin();
      while ( i != this->end() )
      {
        if ( val == *i )  // erase if values are equal
          this->erase_direct(i);
        else
          ++i;
      }
    };
    /// @~English
    /// Erases the value at an iterator for this Vectorset. The advantage of this function is that
    /// searching for the value is not necessary.
    ///
    /// @param iter
    ///   The iterator for the position to erase.
    ///
    /// @note This function does not preserve order. If you wish to preserve order, see remove.
    /// @~
    /// @see erase_once
    void erase(const iterator &iter)
    {
      // Check if the iterator is in bounds (most common hit first)
      if ( iter < this->end() && iter >= this->begin() )
        this->erase_direct(iter);
    };
    /// @copydoc erase(const iterator&)
    /// @note This function may be unsafe, but is provided for performance and used internally.
    void erase_direct(const iterator &iter)
    {
      // Remove the element by replacing it with the last one
      --this->pEndArr;
      *(&iter) = *this->pEndArr;
    };
    /// @~English
    /// Works similar to the STL algorithm remove_if. Iterates and calls a function predicate for
    /// each element in the Vectorset. If the predicate call returns true, then the value is
    /// erased by calling erase.
    ///
    /// @param pred
    ///   Function predicate used to determine if a value is removed.
    /// @~
    /// @see std::remove_if, remove_if
    template <typename Func>
    void erase_if( const Func &pred )
    {
      // iterate all elements
      auto i = this->begin();
      while ( i != this->end() )
      {
        if ( pred(*i) )  // erase if predicate returns true
          this->erase_direct(i);
        else
          ++i;
      }
    };
  // ----------------------------------------------------------------- remove
    /// @~English
    /// This function removes an element from a Vectorset. Unlike remove, it assumes there exists
    /// only one element. If the element is found, it is removed and the function immediately
    /// returns.
    ///
    /// @param val
    ///   The value to remove from the Vectorset.
    ///
    /// @note This function preserves order. It is recommended to use erase_once for performance
    /// if order is not important.
    /// @~
    /// @see remove, erase_once
    void remove_once(const _T &val)
    {
      // iterate all elements
      for ( auto i = this->begin(); i != this->end(); ++i )
      {
        if ( val == *i )  // break if values are equal
          break;
      }
      this->remove(i); // remove the value (does check for end())
    };
    /// @~English
    /// Removes all values found in the Vectorset. When a value is found, it is removed and the
    /// function continues searching for the same value until it reaches the end of the Vectorset.
    ///
    /// @param val
    ///   The value to search for and remove.
    /// 
    /// @note This function preserves order. It is recommended to use erase for performance if
    /// order is not important.
    /// @~
    /// @see remove_once, erase
    void remove(const _T &val)
    {
      // Find the first instance
      iterator i = this->begin();
      while ( i < this->end() && val != *i )
        ++i;

      // Now do iteration with shifting
      iterator skip = i;
      while ( skip < this->end() )
      {
        while ( skip < this->end() && val == *skip )  // increment remove ptr if equal
          ++skip;

        // if we've not reached the end, then shift the value up,
        // overwriting the one we removed
        if ( skip < this->end() )
          *(&i) = *(&skip);

        // increment our position
        ++i;
        ++skip;
      }
      this->pEndArr -= (&skip - &i);
    };
    /// @~English
    /// Erases the value at an iterator for this Vectorset. The advantage of this function
    /// is that searching for the value is not necessary.
    ///
    /// @param iter
    ///   The iterator for the position to erase.
    ///
    /// @note This function preserves order. It is recommended to use erase for performance if
    /// order is not important.
    /// @~
    /// @see remove_once, erase
    void remove(const iterator &iter)
    {
      // Check if the iterator is in bounds (most common hit first)
      if ( iter < this->end() && iter >= this->begin() )
        this->remove_direct(iter);
    };
    /// @copydoc remove(const iterator&)
    /// @note This function may be unsafe, but is
    /// provided for performance.
    void remove_direct(const iterator &iter)
    {
      // Remove the element by shifting positions
      iterator t = iter;
      iterator tnext = t + 1;
      while ( tnext != this->end() )
      {
        *(&t) = *(&tnext);
        ++t;
        ++tnext;
      }
      --this->pEndArr;
    };
    /// @~English
    /// Works similar to the STL algorithm remove_if. Iterates and calls a function predicate for
    /// each element in the Vectorset. If the predicate call returns true, then the value is removed.
    ///
    /// @param pred Function predicate used to determine if a value is removed.
    /// @~
    /// @see std::remove_if, erase_if
    template <typename Func>
    void remove_if( const Func &pred )
    {
      // Find the first instance
      iterator i = this->begin();
      while ( i < this->end() && !pred(*i) )
        ++i;

      // Now do iteration with shifting
      iterator skip = i;
      while ( skip < this->end() )
      {
        while ( skip < this->end() && pred(*skip) )  // increment remove ptr if equal
          ++skip;

        // if we've not reached the end, then shift the value up,
        // overwriting the one we removed
        if ( skip < this->end() )
          *(&i) = *(&skip);

        // increment our position
        ++i;
        ++skip;
      }
      this->pEndArr -= (&skip - &i);
    };
  // ----------------------------------------------------------------- stl spinoff functions
    /// @~English
    /// Clears the Vectorset, removing all elements. Used the same way as stl containers.
    ///
    /// @note Because of the restrictions placed on the Vectorset, this function executes a
    /// single instruction regardless of the number of entries.
    /// @~
    inline void clear()
    {
      this->pEndArr = this->pStartArr;
    };


  // element insertion
    /// @~English
    /// Inserts a new value into the Vectorset only if it does not already exist.
    ///
    /// @param val
    ///   The value to insert.
    /// 
    /// @~
    /// @see std::set
    inline void insert(const _T &val)
    {
      if ( !this->exists(val) )
        this->push_back(val);
    };
    /// @copydoc insert(const _T &val)
    inline void insert(const iterator &val)
    {
      this->insert(*val);
    };
    /// @~English
    /// Inserts all elements of another vector only if each element does not already exsist.
    ///
    /// @param other
    ///   Another Vectorset of the same type whose elements will be inserted into this one.
    ///
    /// @~
    void insert(const ConstVectorset<_T> &other)
    {
      for ( iterator i = other.begin(); i != other.end(); ++i )
        this->insert(i);
    };
    /// @~English
    /// Pushes a value to the back of the Vectorset, expanding it if necessary.
    /// 
    /// @param val
    ///   The value to add to the back of this Vectorset.
    ///
    /// @note Duplicate entries are not removed.
    /// @~
    /// @see push_front
    inline void push_back(const _T val)
    {
      if ( this->expand() )
      {
        *this->pEndArr = val;
        ++this->pEndArr;
      }
    };
    /// @copydoc push_back(const _T val)
    inline void push_back(const iterator &val)
    {
      this->push_back(*val);
    };
    /// @~English
    /// Pushes all values of another Vectorset to the back of this one, expanding it to contain
    /// exactly the number of elements in both Vectorsets if necessary.
    /// 
    /// @param other
    ///   The other Vectorset of the same type whose values will be appended to this one.
    ///
    /// @note Duplicate entries are not removed.
    /// @~
    void push_back(const ConstVectorset<_T> &other)
    {
      // localize variables
      size_t nSize = other.size();

      // manage existing set
      if ( this->expand(nSize) )
      {
        // copy the data to this set
        memcpy( this->pEndArr, other, nSize*sizeof(_T) );

        // update variables in this set
        this->pEndArr += nSize;
      }
    };
    /// @~English
    /// Pushes a value to the front of the Vectorset, expanding it if necessary.
    /// 
    /// @param val
    ///   The value to add to the back of this Vectorset.
    ///
    /// @note Duplicate entries are not removed.
    /// @note For efficiency, it is recommended to use push_back.
    /// @~
    /// @see push_back
    void push_front(const _T val)
    {
      if ( this->expand() )
      {
        memmove(this->pStartArr+1, this->pStartArr, this->size()*sizeof(_T));
        ++this->pEndArr;
        *this->pStartArr = val;
      }
    };
    /// @copydoc push_front(const _T val)
    inline void push_front(const iterator &val)
    {
      this->push_front(*val);
    };
    /// @~English
    /// @TODO change return value to copy of value that was popped
    /// @~
    /// @see pop_front
    inline void pop_back()
    {
      if ( !this->empty() )  // remove last element if non-empty
        --this->pEndArr;
    };
    /// @~English
    /// @TODO change return value to copy of value that was popped
    /// @note For efficiency, it is recommended to use pop_back.
    /// @~
    /// @see pop_back
    void pop_front()
    {
      if ( this->empty() )  // return if empty
        return;
      
      --this->pEndArr; // subtract the last element (not removing it)
      size_t size = this->size();  // localize the new size
      switch ( size )
      {
      case 0:  // just ignore it if it was the only element
        break;
      case 1: // if only one element should remain
        *this->pStartArr = *this->pEndArr;
        break;
      default: // otherwise move all elements up
        memmove(this->pStartArr, this->pStartArr+1, size*sizeof(_T));
        break;
      }
    };
  // -----------------------------------------------------------------
  protected:
    /// Expands the container if it currently does not have the capacity for the number of
    /// additional elements.
    ///
    /// @retval true If space is available.
    /// @retval false If realloc failed.
    bool expand(size_t additionalElements = 1)
    {
      // localize the variables
      size_t allocSize = this->max_size(), arrSize = this->size();

      // expand to expected size, or ignore of not necessary
      if ( allocSize >= arrSize + additionalElements )
        return true;

      // expand to desired size
      allocSize = ((arrSize + additionalElements)/256 + 1)*256;

      // Reallocate and store the new values
      _T *pReallocated = (_T*)realloc(this->pStartArr, allocSize*sizeof(_T));
      if ( pReallocated != nullptr )
      {
        this->pStartArr  = pReallocated;
        this->pEndArr    = pReallocated + arrSize;
        this->pEndAlloc  = pReallocated + allocSize;
        return true;
      }
      return false;
    };

    // Variables
    _T *pEndAlloc;    // ptr to end of allocation
  };

}

