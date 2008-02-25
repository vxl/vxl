// This is brl/bpro/bprb/bprb_process_manager.h
#ifndef bprb_process_manager_h_
#define bprb_process_manager_h_

//:
// \file
// \brief This file defines a class base process manager
// It is templated to allow its childs to be singleton managers 
// \author Isabel Restrepo
// \date 3/27/07
//
// \verbatim
//  Modifications
//  Ported to brl/bprb - 1/31/2008 - J.L. Mundy         
// \endverbatim


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>

#include <vbl/vbl_ref_count.h>
#include <bprb/bprb_process_sptr.h>

template <class T>
class bprb_process_manager : public vbl_ref_count 
{

 public:

   //: Destructor
  virtual ~bprb_process_manager();
  
  //: Use this instead of constructor
  static T* instance();
  
  bprb_process_sptr get_process_by_name( const vcl_string& name ) const;

  void add_process_to_queue(const bprb_process_sptr& process);
  void delete_last_process();
  void clear_process_queue();

  vcl_vector <vcl_string> get_process_queue_list() const;

  static void register_process( const bprb_process_sptr& sptr );
  

protected:
  
  //: Constructor
  bprb_process_manager();
  
  static T* instance_;
  
   //: Initialize the static instance
  void initialize();
  

  static vcl_map< vcl_string , bprb_process_sptr > process_map;

  vcl_vector< bprb_process_sptr > process_queue;
};

#endif // bprb_process_manager_h_
