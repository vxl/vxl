#ifndef mbl_lru_cache_h_
#define mbl_lru_cache_h_

//:
// \file
// \brief Least recently used cache.
// \author Ian Scott


#include <vcl_list.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_cassert.h>

//: Least recently used cache
// I is the index type
// V is the value type
template <class I, class V>
class mbl_lru_cache
{
  typedef vcl_list<I> list_type;
  typedef vcl_map<I, vcl_pair<V, list_type::iterator> > map_type;
  list_type l_;
  map_type m_;
  unsigned long n_;

public:

  mbl_lru_cache(): n_(0) {}


  //: Create a cache of size n.
  // The actual memory size of the cache will be
  // n * (sizeof(V) + 2*sizeof(I) + overhead(map element) + overhead(list element).
  // for many implementations overhead(list element) = 2 * sizeof(void *), and
  // overhead(list element) = 3 * sizeof(void *) + sizeof(enum).
  // e.g. on a 32 bit computer, where I is a pair<unsigned, unsigned> and
  // V is a double, memory size = n * (8 + 16 + 16 + 8).
  // This makes the cache 17% space efficient - not very good.
  mbl_lru_cache(unsigned n):  n_(n) {}

  //: lookup index in the cache
  // \return 0 if not in the cache.
  const V* lookup(const I& index)
  {
    assert (m_.size() == l_.size());
    map_type::iterator it= m_.find(index);
    if (it != m_.end())
    {
      l_.push_front(index);
      l_.erase(it->second.second);
      it->second.second = l_.begin();
      return &(it->second.first);
    }
    else return 0;
  }


  //: Insert this value into the cache.
  // For speed this method assumes that the index isn;t already in the cache.
  // \param dont_if_full if true, and the cache is full,
  // then the cache will not be modified.
  void insert(const I& index, const V& value, bool dont_if_full=false)
  {
    assert (m_.size() == l_.size());
    assert (m_.find(index) == m_.end());

    if (m_.size() < n_)
    {
      l_.push_front(index);
      m_[index] = vcl_make_pair(value, l_.begin());
    }
    else
    {
      if (!dont_if_full)
      {
        l_.push_front(index);
        m_[index] = vcl_make_pair(value, l_.begin()) ;
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
