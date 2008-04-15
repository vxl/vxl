// This is brl/bpro/bprb/bprb_batch_process_manager.h
#ifndef bprb_batch_process_manager_h_
#define bprb_batch_process_manager_h_
//:
// \file
// \brief This file defines a process manager for handing batch processing, i.e. no user interface
// \author J.L. Mundy
// \date 1/31/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process_sptr.h>
#include <bprb/bprb_process_manager.h>
#include <brdb/brdb_value_sptr.h>
#include <bprb/bprb_parameters_sptr.h>

class bprb_batch_process_manager : public bprb_process_manager<bprb_batch_process_manager>
{
 public:
  // Destructor
  virtual ~bprb_batch_process_manager();

  //: clear the database for new script processing
  bool clear();

  //: initialize the process
  bool init_process(vcl_string const& process_name);

  //: print the default values of the process into the specified XML file
  bool print_default_params(vcl_string const & process_name, vcl_string const& params_XML);

  //: read and set the parameters from an XML file for the current process
  bool set_params(vcl_string const& params_XML);

  //: set the parameters from another parameter instance for the current process
  bool set_params(const bprb_parameters_sptr& params);

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

  //: Initialize the current process state variables
  bool process_init();

  //: Run the current process
  bool run_process();

  //: Debug purposes
  void print_db();

  friend class bprb_process_manager<bprb_batch_process_manager>;

 protected:
  // Constructor
  bprb_batch_process_manager();

  // Members
  bprb_process_sptr current_process_;
};

#endif // bprb_batch_process_manager_h_
