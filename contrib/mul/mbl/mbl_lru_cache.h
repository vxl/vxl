// This is mul/mbl/mbl_lru_cache.h
#ifndef mbl_lru_cache_h_
#define mbl_lru_cache_h_
//:
// \file
// \brief Least recently used cache.
// \author Ian Scott

#include <iostream>
#include <list>
#include <map>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Least recently used cache
// This cache is optimised for speed and is not very memory efficient.
// \param I is the index type
// \param V is the value type
template <class I, class V>
class mbl_lru_cache
{
  //: Allow least recently used item to be found quickly $O(1)$.
  typedef std::list<I> list_type;
  //: Allow least recently used item to be found quickly $O(1)$.
  list_type l_;

  //: Allow value to be looked up quickly $O(\log(n))$ given index.
  typedef std::map<I, std::pair<V, typename list_type::iterator> > map_type;
  map_type m_;
  //: Limit of cache size.
  unsigned long n_;

public:

  mbl_lru_cache(): n_(0) {}


  //: Create a cache of size n.
  // The actual memory size of the cache will be
  // n * (sizeof(V) + 2*sizeof(I) + overhead(map element) + overhead(list element).
  //
  // For many implementations overhead(list element) = 2 * sizeof(void *), and
  // overhead(map element) = 3 * sizeof(void *) + sizeof(enum).
  //
  // e.g. on a 32 bit computer, where I is a pair<unsigned, unsigned> and
  // V is a double, memory size = n * (8 + 16 + 16 + 8).
  // This makes the cache 17% space efficient - not very good.
  mbl_lru_cache(unsigned n):  n_(n) {}

  //: Lookup index in the cache
  // \return A pointer to the value if it is in the cache, or 0 if not .
  const V* lookup(const I& index)
  {
    assert (m_.size() == l_.size());
    typename map_type::iterator it= m_.find(index);
    if (it != m_.end())
    {
      l_.push_front(index);
      l_.erase((*it).second.second);
      (*it).second.second = l_.begin();
      return &((*it).second.first);
    }
    else return nullptr;
  }


  //: Insert this value into the cache.
  // For speed this method assumes that the index isn't already in the cache.
  // \param dont_if_full
  //     If true, and the cache is full, then the cache will not be modified.
  // \param value The value to be inserted in the cache
  // \param index Index to access the value
  void insert(const I& index, const V& value, bool dont_if_full=false)
  {
    assert (m_.size() == l_.size());
    assert (m_.find(index) == m_.end());

    if (m_.size() < n_)
    {
      l_.push_front(index);
      m_[index] = std::make_pair(value, l_.begin());
    }
    else
    {
      if (!dont_if_full)
      {
        l_.push_front(index);
        m_[index] = std::make_pair(value, l_.begin()) ;
        m_.erase( m_.find(l_.back()));
        l_.pop_back();
      }
    }
  }
  void clear() {l_.clear(); m_.clear();}
  bool full() const { return m_.size() < n_;}
  void resize(unsigned n)
  {
    while (m_.size() > n)
    {
      m_.erase( m_.find(l_.back()));
      l_.pop_back();
    }
    n_=n;
  }
};

#endif // mbl_lru_cache_h_
