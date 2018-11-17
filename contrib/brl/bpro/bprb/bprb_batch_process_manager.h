// This is brl/bpro/bprb/bprb_batch_process_manager.h
#ifndef bprb_batch_process_manager_h_
#define bprb_batch_process_manager_h_
//:
// \file
// \brief This file defines a process manager for handing batch processing, i.e. no user interface
// \author J.L. Mundy
// \date January 31, 2008
//
// \verbatim
//  Modifications
//   22 May 2012 - Peter Vanroose - catching failing redirect of stdout
// \endverbatim

#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_process_sptr.h>
#include <bprb/bprb_process_manager.h>
#include <brdb/brdb_value_sptr.h>
#include <bprb/bprb_parameters_sptr.h>

class bprb_batch_process_manager : public bprb_process_manager<bprb_batch_process_manager>
{
 public:
  // Destructor
  ~bprb_batch_process_manager() override;

  //: clear the database for new script processing
  bool clear();

  //: initialize the process
  bool init_process(std::string const& process_name);

  //: print the default values of the process into the specified XML file
  bool print_default_params(std::string const & process_name, std::string const& params_XML);

  //: read and set the parameters from an XML file for the current process
  bool set_params(std::string const& params_XML);

  //: set the parameters from another parameter instance for the current process
  bool set_params(const bprb_parameters_sptr& params);

  //: set primitive data type input on current process
  bool set_input(unsigned i, brdb_value_sptr const& input);

  bool set_input_from_db(unsigned i, unsigned id, const std::string& type);

  //: set input from the database
  bool set_input_from_db(unsigned i, unsigned id);

  //: put the output into the database
  bool commit_output(unsigned i, unsigned& id);

  //: put the output into the database
  bool commit_output(unsigned i, unsigned& id, std::string& type);

  //: put the output into the database
  bool commit_all_outputs(std::vector<unsigned>& ids);

  //: remove data from the database
  bool remove_data(unsigned id);

  //: Initialize the current process state variables
  bool process_init();

  //: Run the current process
  bool run_process();

  //: finish the current process
  bool finish_process();

  //: set verbose on
  bool verbose() {verbose_ = true; return verbose_;}

  //: set verbose off
  bool not_verbose() {verbose_ = false; return verbose_;}

  //: Debug purposes
  void print_db();

  //: Set stdout
  // \return false on failure
  bool set_stdout(const std::string& file);

  //: Reset stdout back to the console
  // \return false on failure
  bool reset_stdout();

  //: Interface to database binary read/write
  void b_write_db(std::string const& path);

  void b_read_db(std::string const& path);

  friend class bprb_process_manager<bprb_batch_process_manager>;

 protected:
  // Constructor
  bprb_batch_process_manager();

  // Members
  bprb_process_sptr current_process_;

  bool verbose_;
};

#endif // bprb_batch_process_manager_h_
