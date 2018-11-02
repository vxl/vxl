// This is core/vbl/vbl_batch_compact_multimap.h
#ifndef vbl_batch_compact_multimap_h_
#define vbl_batch_compact_multimap_h_
//:
// \file
// \brief Like a smaller and slightly faster vcl_batch_multimap but without the pair<key, value> sequence
// \author Ian Scott, Imorphics 2011

#include <vector>
#include <cstddef>
#include <functional>
#include <utility>
#include <algorithm>
#include <iterator>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: A fast read and batch-write map-style collection.
// This container stores its keys separately from its values, and has fast construction and deletion.
// It has all the const-access map fundtions, but its contents can only be modified all-at-once.
// You can not get a key,value pair, but you can get access to all the compactly-stored values for
// a given key.
template <typename K, typename T, typename C=std::less<K> >
class vbl_batch_compact_multimap
{
 public:
  typedef K key_type;
  typedef T value_type;
  //: The type of data in the inputted sequence.
  typedef typename std::pair<key_type, value_type> input_type;
  typedef C key_compare;
  typedef unsigned index_type;
  typedef typename std::vector<key_type> key_container_type;
  typedef typename std::vector<index_type> index_container_type;
  typedef typename std::vector<value_type> value_container_type;

  typedef typename key_container_type::const_iterator const_key_iterator;
  typedef typename value_container_type::const_iterator const_value_iterator;

  protected:
  //: The type of container used internally to process inputted data.
  typedef typename std::vector<input_type> input_container_type;

 public:
  //: A comparator to sort input data, by ignoring the value in pair<key, value>
  class input_compare
  {
   public:
    friend class vbl_batch_compact_multimap<key_type, value_type, key_compare>;

    key_compare comp;

    input_compare(key_compare c): comp(c) { }

    bool operator()(const input_type& x, const input_type& y) const
    { return comp(x.first, y.first); }
  };

  vbl_batch_compact_multimap() = default;

  template <typename CI>
  vbl_batch_compact_multimap(CI start, CI finish)
  {
    input_container_type in(start, finish);
    std::sort(in.begin(), in.end(), input_compare(key_compare()));
    assign_sorted(in.begin(), in.end());
  }

  //: Change all the values in the multimap.
  template <typename CI>
  void assign(CI start, CI finish)
  {
    input_container_type in(start, finish);
    std::sort(in.begin(), in.end(), input_compare(key_compare()));
    assign_sorted(in.begin(), in.end());
  }

  //: Change all the values in the multimap, to a ready sorted sequence
  // The input values must already be sorted on their v.first members.
  template <typename CI>
  void assign_sorted(CI start, CI finish)
  {
    keys_.clear();
    indices_.clear();
    values_.clear();
    assert(is_sorted(start, finish, input_compare(key_compare())));
    while (start != finish)
    {
     typename std::iterator_traits<CI>::value_type::first_type const & last_start_val = start->first;
      keys_.push_back(start->first);
      indices_.push_back(values_.size());
      values_.push_back(start->second);
      while(++start != finish && start->first == last_start_val)
      {
        values_.push_back(start->second);
      }
    }
    indices_.push_back(values_.size()); // always one more index than key.
  }

  void swap(vbl_batch_compact_multimap& x)
  {
    keys_.swap(x.keys_);
    indices_.swap(x.indices_);
    values_.swap(x.values_);
  }

  bool operator==(const vbl_batch_compact_multimap& rhs)
  {
    return keys_ == rhs.keys_ &&
           indices_ == rhs.indices_ &&
           values_ == rhs.values_;
  }

  // const vector API

  const_key_iterator keys_begin() const { return keys_.begin(); }
  const_key_iterator keys_end() const { return keys_.end(); }
  const_value_iterator values_begin() const { return values_.begin(); }
  const_value_iterator values_end() const { return values_.end(); }
  bool empty() const { return values_.empty(); }
  std::size_t size() const { return values_.size(); }

  // const map API

  //: Finds the beginning of a subsequence of values whose key matches given \p x.
  // \return iterator to the first value whose key matches \p x, or the
  //   next greatest element if no match is found.
  const_value_iterator lower_bound(const key_type& x) const
  {
    const_key_iterator k_it = std::lower_bound(keys_.begin(), keys_.end(),
                                              x, key_compare() );

    return values_.begin() + indices_[k_it - keys_.begin()];
  }

  //: Finds the one past the end of a subsequence of values whose key matches given \p x.
  // \return iterator to one past the last value whose key that matches \p key, or to the
  //   next greatest element if no match is found.
  const_value_iterator upper_bound(const key_type& x) const
  {
    const_key_iterator k_it = std::upper_bound(keys_.begin(), keys_.end(),
                                              x, key_compare() );

    return values_.begin() + indices_[k_it - keys_.begin()];
  }

  //: A more efficient  make_pair(lower_bound(...), upper_bound(...))
  std::pair<const_value_iterator, const_value_iterator> equal_range(const key_type& x) const
  {
    // This appears particularly slow in MSVC10 with no optimisation. In particular it appears slower
    // than std::map dereference.
    const_key_iterator k_it = std::lower_bound(keys_.begin(), keys_.end(),
                                              x, key_compare() );

    if (k_it == keys_end() || *k_it != x)
    {
      const_value_iterator v_it=values_.begin() + indices_[k_it - keys_.begin()];
      return std::make_pair(v_it, v_it);
    }
    else
    {
      return std::make_pair(values_.begin() + indices_[k_it - keys_.begin()],
                           values_.begin() + indices_[k_it - keys_.begin() + 1u] );
    }
  }

  //: Finds the first value with key matching \p x, or returns values_end() if no match,
  const_value_iterator find(const key_type& x) const
  {
    const_key_iterator k_it = std::lower_bound(keys_.begin(), keys_.end(),
                                              x, key_compare() );

    if (k_it == keys_end() || *k_it != x)
      return values_end();
    else
      return values_.begin() + indices_[k_it - keys_.begin()];
  }

  //: Finds the number of values matching key \p x,
  std::size_t count(const key_type& x) const
  {
    const_key_iterator k_it = std::lower_bound(keys_.begin(), keys_.end(),
                                              x, key_compare() );

    if (k_it == keys_end() || *k_it != x)
      return 0;
    else
      return indices_[k_it - keys_.begin() + 1u] - indices_[k_it - keys_.begin()];
  }

 private:
  key_container_type keys_;
  index_container_type indices_;
  value_container_type values_;

  template <typename CI, typename CMP>
  bool is_sorted(CI start, CI end, CMP comp)
  {
    if (start == end) return true;

    for (--end; start!=end; ++start)
    {
      // if cmp(a,b) is the sorting criteria, then !cmp(b,a) is the testing criteria for is_sorted.
      if (comp(*(start+1), *start)) return false;
    }
    return true;
  }
};

template<typename K, typename T, typename C>
inline void swap(vbl_batch_compact_multimap<K, T, C> &x, vbl_batch_compact_multimap<K, T, C>& y)
{
  x.swap(y);
}

#endif // vbl_batch_compact_multimap_h_
