// This is prip/vmap/vmap_ptr_sequences.h
#ifndef vmap_ptr_sequences_h_
#define vmap_ptr_sequences_h_
//:
// \file
// \brief provides classes for defining and handling sequences of pointers.
// \author Jocelyn Marchadier
// \date 06/05/2004
//
// \verbatim
//  06 May 2004 Jocelyn Marchadier
// \endverbatim

#include "vmap_types.h"
#include <vcl_vector.h>

//:
template< typename F, typename Ref, typename Ptr, typename It >
class vmap_ptr_iterator_wrapper
{
 public:
    //:
    typedef vmap_ptr_iterator_wrapper< F,Ref,Ptr,It > self_type ;

    //:
    typedef F element_type ;

    //:
    vmap_ptr_iterator_wrapper() {}

    //:
    vmap_ptr_iterator_wrapper(const self_type &it) :_it(it._it) {}

    //:
    ~vmap_ptr_iterator_wrapper() {}

    //:
    self_type & operator=(const self_type &it)
    {
        _it=it._it ;
        return *this ;
    }

    //:
    bool operator==(const self_type &it) const
    {
        return _it==it._it ;
    }

    //:
    bool operator!=(const self_type &it) const
    {
        return _it!=it._it ;
    }

    //:
    Ref operator * () const
    {
        return (Ref)**_it ;
    }

    //:
    Ptr operator->() const
    {
        return (Ptr) *_it;
    }

    //:
    self_type & operator++ ()
    {
        ++_it;
        return *this ;
    }

// private :

    //:
    vmap_ptr_iterator_wrapper(It arg) :_it(arg) {}

    //:
    It reference() const
    {
        return _it ;
    }
 private :
    //:
    It _it ;
};

//:
template <typename DPtr>
int offset(DPtr arg1,DPtr arg2)
{
  return arg1->sequence_index()-arg2->sequence_index() ;
}

//:
template <class D>
class vmap_ptr_sequence
{
 protected:

  //:
  typedef D element_type ;

  //:
  typedef element_type** iterator ;

  //:
  typedef element_type*const* const_iterator ;

  //:
  typedef element_type* pointer ;

 public:
  //:
  vmap_ptr_sequence() : _begin(NULL), _end(NULL) {}

  //:
  vmap_ptr_sequence(const vmap_ptr_sequence<D> & arg ) : _begin(arg._begin), _end(arg._end) {}

  //:
  int size () const
  {
    return _end-_begin;
  }

  //:
  bool empty() const
  {
    return size()==0 ;
  }

 protected:

  //:
  iterator begin()
  {
    return _begin ;
  }

  //:
  iterator end()
  {
    return _end ;
  }

  //:
  const_iterator begin() const
  {
    return _begin;
  }

  //:
  const_iterator end() const
  {
    return _end ;
  }

  //:
  template <class _Predicate>
  iterator reorder(const _Predicate & arg)
  {
    iterator middle=vcl_stable_partition(_begin,_end,arg) ;
    for (iterator tmp=_begin;tmp!=_end;++tmp)
      (*tmp)->set_sequence_index(tmp-_begin) ;
    return middle ;
  }

  //:
  pointer & get_pointer(int arg)
  {
    return _begin[arg] ;
  }

  //:
  const pointer & get_pointer(int arg) const
  {
    return _begin[arg] ;
  }

  //:
  void swap(int i, int j)
  {
    vcl_swap(get_pointer(i),get_pointer(j)) ;
    get_pointer(i)->set_sequence_index(i) ;
    get_pointer(j)->set_sequence_index(j) ;
  }

  //:
  void pop_back()
  {
    delete_dart(*(--_end)) ;
  }

  //:
  void set_begin(iterator arg)
  {
    _begin=arg;
  }

  //:
  void resize(int arg_size)
  {
    _end=_begin+arg_size ;
  }

  //:
  void clear()
  {
    _begin=_end=NULL ;
  }

 private:

  //:
  iterator _begin, _end ;
};

//:
template <class D>
class vmap_owning_sequence: public vmap_ptr_sequence<D>
{
 protected:

  //:
  typedef vmap_owning_sequence<D> self_type ;

  //:
  typedef D element_type ;

  //:
  typedef element_type** iterator ;

  //:
  typedef element_type*const* const_iterator ;

  //:
  typedef element_type* pointer ;

 public:
  //:
  vmap_owning_sequence() {}

  //:
  vmap_owning_sequence(const self_type & arg )
  {
    operator=( arg ) ;
  }

  //:
  self_type & operator=(const self_type & arg )
  {
    if (&arg!=this)
    {
      resize(arg.size()) ;
      const_iterator i=arg.begin();
      int j=0;
      for (;i!=arg.end(); ++i,++j)
      {
        _storage[j]=**i ;
        _storage[j].set_sequence_index(j) ;
      }
    }
    return *this ;
  }

  //:
  int position(const element_type & arg) const
  {
    return &arg - &_storage.front() ;
  }

 protected:

  //:
  void pop_back()
  {
    vmap_ptr_sequence<D>::resize(size()-1) ; ;
  }

  //:
  void resize(int arg_size)
  {
    clear() ;
    _storage.resize(arg_size) ;
    set_begin(new pointer[arg_size]) ;
    vmap_ptr_sequence<D>::resize(arg_size) ;

    for (int i=0; i<arg_size; ++i)
    {
      get_pointer(i)=&_storage[i] ;
      _storage[i].set_sequence_index(i) ;
    }
  }

  //:
  void clear()
  {
    if (begin()!=NULL)
      delete [] begin() ;
    vmap_ptr_sequence<D>::clear() ;
    _storage.clear();
  }

 private:

  //:
  vcl_vector<element_type> _storage ;
};

#endif
