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
//  Modifications
//   06 May 2004 Jocelyn Marchadier
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
  vmap_ptr_iterator_wrapper(const self_type &it) :it_(it.it_) {}

  //:
  ~vmap_ptr_iterator_wrapper() {}

  //:
  self_type & operator=(const self_type &it)
  {
      it_=it.it_ ;
      return *this ;
  }

  //:
  bool operator==(const self_type &it) const
  {
      return it_==it.it_ ;
  }

  //:
  bool operator!=(const self_type &it) const
  {
      return it_!=it.it_ ;
  }

  //:
  Ref operator * () const
  {
      return (Ref)**it_ ;
  }

  //:
  Ptr operator->() const
  {
      return (Ptr) *it_;
  }

  //:
  self_type & operator++ ()
  {
      ++it_;
      return *this ;
  }

// private :

  //:
  vmap_ptr_iterator_wrapper(It arg) :it_(arg) {}

  //:
  It reference() const
  {
      return it_ ;
  }

 private :
  //:
  It it_ ;
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
  vmap_ptr_sequence() : begin_(NULL), end_(NULL) {}

  //:
  vmap_ptr_sequence(const vmap_ptr_sequence<D> & arg ) : begin_(arg.begin_), end_(arg.end_) {}

  //:
  int size () const
  {
    return end_-begin_;
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
    return begin_ ;
  }

  //:
  iterator end()
  {
    return end_ ;
  }

  //:
  const_iterator begin() const
  {
    return begin_;
  }

  //:
  const_iterator end() const
  {
    return end_ ;
  }

  //:
  template <class Predicate_>
  iterator reorder(const Predicate_ & arg)
  {
    iterator middle=vcl_stable_partition(begin_,end_,arg) ;
    for (iterator tmp=begin_;tmp!=end_;++tmp)
    {
      (*tmp)->set_sequence_index(tmp-begin_) ;
    }
    return middle ;
  }

  //:
  pointer & get_pointer(int arg)
  {
    return begin_[arg] ;
  }

  //:
  const pointer & get_pointer(int arg) const
  {
    return begin_[arg] ;
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
    delete_dart(*(--end_)) ;
  }

  //:
  void set_begin(iterator arg)
  {
    begin_=arg;
  }

  //:
  void resize(int arg_size)
  {
    end_=begin_+arg_size ;
  }

  //:
  void clear()
  {
    begin_=end_=NULL ;
  }

 private:

  //:
  iterator begin_, end_ ;
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
        storage_[j]=**i ;
        storage_[j].set_sequence_index(j) ;
      }
    }
    return *this ;
  }

  //:
  int position(const element_type & arg) const
  {
    return &arg - &storage_.front() ;
  }

 protected:

  //:
  void pop_back()
  {
    vmap_ptr_sequence<D>::resize(this->size()-1) ; ;
  }

  //:
  void resize(int arg_size)
  {
    clear() ;
    storage_.resize(arg_size) ;
    set_begin(new pointer[arg_size]) ;
    vmap_ptr_sequence<D>::resize(arg_size) ;

    for (int i=0; i<arg_size; ++i)
    {
      this->get_pointer(i)=&storage_[i] ;
      storage_[i].set_sequence_index(i) ;
    }
  }

  //:
  void clear()
  {
    delete [] this->begin() ;
    vmap_ptr_sequence<D>::clear() ;
    storage_.clear();
  }

 private:

  //:
  vcl_vector<element_type> storage_ ;
};

#endif
