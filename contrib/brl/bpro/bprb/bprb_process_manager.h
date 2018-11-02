// This is brl/bpro/bprb/bprb_process_manager.h
#ifndef bprb_process_manager_h_
#define bprb_process_manager_h_

//:
// \file
// \brief This file defines a class base process manager
// It is templated to allow its children to be singleton managers
// \author Isabel Restrepo
// \date March 27, 2007
//
// \verbatim
//  Modifications
//   Jan 31, 2008 - J.L. Mundy - Ported to brl/bprb
// \endverbatim


#include <vector>
#include <iostream>
#include <map>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_ref_count.h>
#include <bprb/bprb_process_sptr.h>

template <class T>
class bprb_process_manager : public vbl_ref_count
{
 public:

  //: Destructor
  ~bprb_process_manager() override;

  //: Use this instead of constructor
  static T* instance();

  bprb_process_sptr get_process_by_name( const std::string& name ) const;

  void add_process_to_queue(const bprb_process_sptr& process);
  void delete_last_process();
  void clear_process_queue();

  std::vector <std::string> get_process_queue_list() const;

  static void register_process( const bprb_process_sptr& sptr );

 protected:

  //: Constructor
  bprb_process_manager();

  static T* instance_;

  //: Initialize the static instance
  void initialize();

  static std::map< std::string , bprb_process_sptr > process_map;

  std::vector< bprb_process_sptr > process_queue;
};

#endif // bprb_process_manager_h_
