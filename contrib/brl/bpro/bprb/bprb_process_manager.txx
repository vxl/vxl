// This is brl/bprb/bprb_process_manager<T>.cxx

//:
// \file

#include "bprb_process_manager.h"

#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vul/vul_arg.h>
#include <bprb/bprb_process.h>

//: Constructor
template <class T>
bprb_process_manager<T>::bprb_process_manager()
{
  
}

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
bprb_process_manager<T>::get_process_by_name( const vcl_string& name ) const
{
  vcl_multimap< vcl_string , bprb_process_sptr >::const_iterator it = process_map.find( name );
  if( it == process_map.end() ) {
    return NULL ;
  }
  return it->second;
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
vcl_vector <vcl_string>
bprb_process_manager<T>::get_process_queue_list() const
{
  vcl_vector <vcl_string> list;
  vcl_vector< bprb_process_sptr >::const_iterator i = process_queue.begin();
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

  process_map.insert( vcl_pair< vcl_string , bprb_process_sptr >( sptr->name() , sptr ) );
  vcl_cout << "Registered " << sptr->name() << '\n';
}



#undef BPRB_PROCESS_MANAGER_INSTANTIATE
#define BPRB_PROCESS_MANAGER_INSTANTIATE(T) \
template class bprb_process_manager<T >; \
template <class T> T* bprb_process_manager<T>::instance_ = 0; \
template <class T> vcl_multimap< vcl_string , bprb_process_sptr > bprb_process_manager<T>::process_map;


