// This is prip/vmap/vmap_map_iterator_wrapper.h
#ifndef vmap_map_iterator_wrapper_h_
#define vmap_map_iterator_wrapper_h_
//:
// \file
// \brief provides a wrapper in order to use any iterator on pairs (e.g. vcl_map::iterator) as if it was an iterator on the 2nd elmt
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

//: A wrapper in order to use any iterator on pairs (e.g. vcl_map::iterator) as if it was an iterator on the second element.
template <typename Ref, typename Ptr, class It>
class vmap_map_iterator_wrapper
{
 public:
  //:
  vmap_map_iterator_wrapper() {}

  //:
  vmap_map_iterator_wrapper(vmap_map_iterator_wrapper const& w) :current_(w.current_) {}

  //:
  vmap_map_iterator_wrapper(It const& w) : current_(w) {}

  //:
  ~vmap_map_iterator_wrapper() {}

  //:
  vmap_map_iterator_wrapper & operator=(vmap_map_iterator_wrapper const& w)
  {
    current_=w.current_ ;
    return *this ;
  }

  //:
  vmap_map_iterator_wrapper & operator=(It const& w)
  {
    current_=w ;
    return *this ;
  }

  //:
  int operator==(vmap_map_iterator_wrapper const& w) const
  {
      return current_==w.current_ ;
  }

  //:
  int operator!=(vmap_map_iterator_wrapper const& w) const
  {
      return current_!=w.current_ ;
  }

  //:
  vmap_map_iterator_wrapper & operator ++ ()
  {
      ++current_ ;
      return *this ;
  }

  //:
  vmap_map_iterator_wrapper & operator -- ()
  {
      --current_ ;
      return *this ;
  }

#if 0
  const T & operator * () const
  {
      return (*current_).second ;
  }

  T & operator * ()
  {
      return (*current_).second ;
  }
#endif // 0

  //:
  Ref operator * () const
  {
      return (*current_).second ;
  }

  //:
  Ptr operator->() const
  {
    return &(operator*());
  }

 private:
  //:
  It current_;
};

#endif
