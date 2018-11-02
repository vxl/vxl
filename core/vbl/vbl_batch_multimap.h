// This is core/vbl/vbl_batch_multimap.h
#ifndef vbl_batch_multimap_h_
#define vbl_batch_multimap_h_

//:
// \file
// \brief Like a faster std::multimap but using only a single block of memory, and no fast insertion or deletion.
// \author Ian Scott, Imorphics 2011


#include <vector>
#include <cstddef>
#include <functional>
#include <utility>
#include <algorithm>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace
{

}
//: A fast read and batch-write map-style collection.
// This container stores its elements in a single vector, and has fast construction and deletion.
// It has all the const-access map fundtions, but its contents can only be modified all-at-once.
template <typename K, typename T, typename C=std::less<K> >
class vbl_batch_multimap
{
public:
  typedef K key_type;
  typedef T mapped_type;
  typedef typename std::pair<key_type, mapped_type> value_type;
  typedef C key_compare;
  typedef typename std::vector<value_type> container_type;
  typedef typename container_type::allocator_type allocator_type;

  typedef typename container_type::const_iterator const_iterator;

  typedef typename container_type::const_reference const_reference;



  class value_compare_t
  {
  //  friend class vbl_batch_multimap<key_type, mapped_type, key_compare>;
  // protected:
  public:
    key_compare comp;

    value_compare_t(key_compare c)
    : comp(c) { }

  public:
    bool operator()(const value_type& x, const value_type& y) const
    { return comp(x.first, y.first); }

    bool compare(const value_type& x, const value_type& y) const
    { return comp(x.first, y.first); }
  };


  vbl_batch_multimap() :data_() {}

  template <typename CI>
  vbl_batch_multimap(CI start, CI finish):
    data_(start, finish)
  {
    std::sort(data_.begin(), data_.end(), value_compare_t(key_compare()));
  }

  //: Change all the values in the multimap.
  template <typename CI>
  void assign(CI start, CI finish)
  {
    data_.assign(start, finish);
    std::sort(data_.begin(), data_.end(), value_compare_t(key_compare()));
  }

  //: Change all the values in the multimap, to a ready sorted sequence
  // The input values must already be sorted on their v.first members.
  template <typename CI>
  void assign_sorted(CI start, CI finish)
  {
    data_.assign(start, finish);
    assert(is_sorted(start, finish, value_compare_t(key_compare())));
  }

  void swap(vbl_batch_multimap& rhs)
  {
    data_.swap(rhs.data_);
  }


  bool operator==(const vbl_batch_multimap&rhs)
  {
    return data_ == rhs.data_;
  }

// const vector API

  const_iterator begin() const { return data_.begin(); }
  const_iterator end() const { return data_.end(); }
  bool empty() const { return data_.empty(); }
  std::size_t size() const { return data_.size(); }

// const map API

  //: Finds the beginning of a subsequence matching given \p key.
  // /return iterator to the first element that equals \p key, or the
  //   next greatest element if no match is found.
  const_iterator lower_bound(const key_type& key) const
  {
    return std::lower_bound(data_.begin(), data_.end(),
      std::make_pair(key, mapped_type()), value_compare_t(key_compare()));
  }

  //: Finds the one past the end of a subsequence matching given \p key.
  // /return iterator to one past the last element that equals \p key, or to the
  //   next greatest element if no match is found.
  const_iterator upper_bound(const key_type& key) const
  {
    return std::upper_bound(data_.begin(), data_.end(),
      std::make_pair(key, mapped_type()), value_compare_t(key_compare()));
  }

  //: A more efficient  make_pair(lower_bound(...), upper_bound(...))
  std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
  {
    return std::equal_range(data_.begin(), data_.end(),
      std::make_pair(key, mapped_type()), value_compare_t(key_compare()));
  }

  //: Finds the first matching value in the sequence, or returns end() if no match,
  const_iterator find(const key_type& key) const
  {
    const_iterator it = lower_bound(key);
    if (it != end() && it->first != key)
      return end();
    else
      return it;
  }

  //: Finds the number of elements with matching \p key,
  std::size_t count(const key_type& key) const
  {
    std::pair<const_iterator, const_iterator> range = equal_range(key);
    return range.second - range.first;
  }


private:
  container_type data_;

  template <typename CI, typename CMP>
  static bool is_sorted(CI start, CI end, CMP comp)
  {
    if (start == end) return true;

    for (end--; start!=end; ++start)
    {
      if ( comp(*(start+1), *start)) return false;
    }
    return true;
  }

};

template<typename K, typename T, typename C>
inline void swap(vbl_batch_multimap<K, T, C> &x, vbl_batch_multimap<K, T, C>& y)
{
  x.swap(y);
}

#endif // vbl_batch_multimap_h_
