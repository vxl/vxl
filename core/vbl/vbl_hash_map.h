// -*- c++ -*-
#ifndef vbl_hash_map_h_
#define vbl_hash_map_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl/vcl_iosfwd.h>
#include <vcl/vcl_functional.h> // vcl_equal_to()
#include <vcl/vcl_hash_map.h>
#include <vcl/vcl_pair.h>

//: Simpler hashtable
//
template <class Key, class T>
class vbl_hash_map : public vcl_hash_map<Key, T, vcl_hash<Key>, vcl_equal_to<Key> > {
 public:
  typedef vcl_hash_map<Key, T, vcl_hash<Key>, vcl_equal_to<Key> > base;
  typedef typename base::value_type value_type;
  typedef typename base::iterator   iterator;
  typedef typename base::const_iterator const_iterator;

  vbl_hash_map() {}

  //: convenience insert
  void insert(const Key& key, const T& t) {
    base::insert(value_type(key, t));
  }

  vcl_pair<iterator, bool> insert(const value_type& obj) {
    return base::insert(obj);
  }
  void insert(const value_type* f, const value_type* l) {
    base::insert(f,l);
  }
  void insert(const_iterator f, const_iterator l) {
    base::insert(f,l);
  }
  vcl_pair<iterator, bool> insert_noresize(const value_type& obj) {
    return base::insert(obj);
  }

  //: cool compat
  typedef iterator Position;
  Position pos;
  bool cool_find(const Key& key) {
    return (pos = find(key)) != end();
  }
  T& value() {
    return (*pos).second;
  }
  void remove() {
    erase(pos);
  }
  void reset() {
    pos = begin();
  }
  bool next() {
    ++pos;
    Position tmp = pos; ++tmp;
    return (tmp == end());
  }
private:
  bool prev() {
    //Position tmp = pos;
    //--pos;
    //return (tmp == begin());
    return false;
  }
public:
  Position& current_position () {
    return pos;
  }
};

#define VBL_HASHTABLE_INSTANTIATE \
extern "please include vbl/vbl_hash_map.txx instead"

#endif // vbl_hash_map_h_
