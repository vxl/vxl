// This is brl/bprb/bprb_batch_process_manager.h
#ifndef bprb_batch_process_manager_h_
#define bprb_batch_process_manager_h_

//:
// \file
// \brief This file defines a process manager for handing batch processing
// i.e. no user interface
// \author J.L. Mundy
// \date 1/31/2008
//
// \verbatim
//  Modifications
//           
// \endverbatim


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <bprb/bprb_process_sptr.h>
#include <bprb/bprb_process_manager.h>
#include <brdb/brdb_value_sptr.h>

class bprb_batch_process_manager : public bprb_process_manager<bprb_batch_process_manager>
{
public:

   //: Destructor
  virtual ~bprb_batch_process_manager();
  
  //: clear the database for new script processing
  bool clear();
  
  //: initialize the process
  bool init_process(vcl_string const& process_name);

  //: set primitive data type input on current process
  bool set_input(unsigned i, brdb_value_sptr const& input);

  //: set input from the database
  bool set_input_from_db(unsigned i, unsigned id);

  //: put the output into the database
  bool commit_output(unsigned i, unsigned& id);

  //: put the output into the database
  bool commit_all_outputs(vcl_vector<unsigned>& ids);

  //: remove data from the database
  bool remove_data(unsigned id);

  //: Run the current process
  bool run_process();

  //: Debug purposes
  void print_db();

  friend class bprb_process_manager<bprb_batch_process_manager>;
protected:
  
  //: Constructor
  bprb_batch_process_manager();

  //: Members
  bprb_process_sptr current_process_;
};

#endif // bprb_batch_process_manager_h_
