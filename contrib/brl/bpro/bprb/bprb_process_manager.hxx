// This is brl/bpro/bprb/bprb_process_manager.hxx
#ifndef bprb_process_manager_hxx_
#define bprb_process_manager_hxx_

#include <iostream>
#include <utility>
#include "bprb_process_manager.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_process.h>

//: Constructor
template <class T>
bprb_process_manager<T>::bprb_process_manager()
= default;

//: Insure only one instance is created
template <class T>
T* bprb_process_manager<T>::instance()
{
  if (!instance_){
    instance_ = new T();
    instance_->initialize();
  }
  return bprb_process_manager::instance_;
}

template <class T>
void
bprb_process_manager<T>::initialize()
{
  process_queue.clear();
}

//: Destructor
template <class T>
bprb_process_manager<T>::~bprb_process_manager()
{
  process_queue.clear();
}


//: Return a process with the given name
template <class T>
bprb_process_sptr
bprb_process_manager<T>::get_process_by_name( const std::string& name ) const
{
  std::map< std::string , bprb_process_sptr >::const_iterator it = process_map.find( name );
  if ( it == process_map.end() ) {
    return nullptr ;
  }
  return it->second->clone();
}


template <class T>
void
bprb_process_manager<T>::add_process_to_queue( const bprb_process_sptr& process )
{
  process_queue.push_back(process);
}

template <class T>
void
bprb_process_manager<T>::delete_last_process()
{
  process_queue.pop_back();
}

template <class T>
void
bprb_process_manager<T>::clear_process_queue()
{
  process_queue.clear();
}

template <class T>
std::vector <std::string>
bprb_process_manager<T>::get_process_queue_list() const
{
  std::vector <std::string> list;
  std::vector< bprb_process_sptr >::const_iterator i = process_queue.begin();
  for (; i!= process_queue.end(); i++)
  {
    list.push_back( (*i)->name() );
  }

  return list;
}

template <class T>
void
bprb_process_manager<T>::register_process( const bprb_process_sptr& sptr )
{
  if(!sptr)
    return;
  std::map< std::string , bprb_process_sptr >::iterator pit;
  std::string nm = sptr->name();
  pit = process_map.find(nm);
  if(pit == process_map.end()){
  process_map.insert( std::pair< std::string , bprb_process_sptr >( nm , sptr ) );
  //std::cout << "Registered " << nm << '\n';
  }
}
#undef BPRB_PROCESS_MANAGER_INSTANTIATE
#define BPRB_PROCESS_MANAGER_INSTANTIATE(T) \
template <class T > T* bprb_process_manager<T >::instance_ = 0; \
template <class T > std::map< std::string , bprb_process_sptr > bprb_process_manager<T >::process_map; \
template class bprb_process_manager<T >

#endif // bprb_process_manager_hxx_
