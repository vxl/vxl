#ifndef vul_lru_helper_h_
#define vul_lru_helper_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Maintain and manipulate a sequence of T's for LRU (Least Recently Used) caching.
// \author fsm at robots.ox.ac.uk
// \date   25 Jun 2002

#include <vcl_map.h>
#include <vcl_list.h>

template <typename T>
struct vul_lru_helper
{
  // put 'x' at the front.
  inline void touch_front(T x);
  inline void touch(T x) { touch_front(x); }
  
  // put 'x' at the back.
  inline void touch_back(T x);
  
  // clear sequence.
  inline void clear();
  
  // return number currently in sequence.
  inline int size() const;
  
  // get the least recently used element.
  inline T get_lru() const;
  
  // remove the least recently used element.
  inline void pop_lru();
  
  // [convenience] get and remove.
  inline T get_pop_lru();
  
private:
  typedef vcl_list<T> list_t;
  typedef vcl_map<T, list_t::iterator> map_t;
  
  // sequence of elements, most recently used first.
  list_t sequence_;
  
  // map from elements to positions (iterators) in the sequence.
  map_t backp_;
};


template <typename T>
void vul_lru_helper<T>::touch_front(T x)
{
  map_t::iterator p = backp_.find(x);
  
  if (p == backp_.end()) {
    // insert new.
    sequence_.push_front(x);
    backp_.insert(map_t::value_type(x, sequence_.begin()));
  }
  
  else {
    // touch old.
    if ((*p).second != sequence_.begin()) {
      // move in list and update iterator in map.
      sequence_.push_front(x);
      sequence_.erase((*p).second);
      (*p).second = sequence_.begin();
    }
    else {
      // already at the front
    }
  }
}

template <typename T>
void vul_lru_helper<T>::touch_back(T x)
{
  map_t::iterator p = backp_.find(x);
  
  if (p == backp_.end()) {
    // insert new.
    sequence_.push_back(x);
    list_t::iterator q = sequence_.end(); -- q;
    backp_.insert(map_t::value_type(x, q));
  }
  
  else {
    // touch old.
    if ((*p).second != sequence_.end()) {
      // move in list and update iterator in map.
      sequence_.push_back(x);
      sequence_.erase((*p).second);
      list_t::iterator q = sequence_.end(); -- q;
      (*p).second = q;
    }
    else {
      // already at the back.
    }
  }
}

template <typename T>
void vul_lru_helper<T>::clear()
{
  sequence_.clear();
  backp_.clear();
}

template <typename T> 
int vul_lru_helper<T>::size() const
{
  return sequence_.size();
}

template <typename T>
T vul_lru_helper<T>::get_lru() const
{
  return sequence_.back();
}

template <typename T>
void vul_lru_helper<T>::pop_lru()
{
  T x = sequence_.back();
  map_t::iterator p = backp_.find(x);
  backp_.erase(p);
  sequence_.pop_back();
}

template <typename T>
T vul_lru_helper<T>::get_pop_lru()
{
  T t = get_lru();
  pop_lru();
  return t;
}

#undef VUL_LRU_HELPER_INSTANTIATE
#define VUL_LRU_HELPER_INSTANTIATE(T) template struct vul_lru_helper<T >

#endif
