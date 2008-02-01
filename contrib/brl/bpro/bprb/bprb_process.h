// This is brl/bpro/bprb/bprb_process.h
#ifndef bprb_process_h_
#define bprb_process_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief The bprb process base class
//
//  A process is an object that wraps an algorithm to create
//  standardized interface.
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy    October   9, 2002  Initial version.
//   Amir Tamrakar October  30, 2003  Redesigned for Brown Eyes
//   Matt Leotta   October  18, 2004  Added some documentation
//   Matt Leotta   December 15, 2004  Migrated from vidpro
//   Edu Almeida   April    21, 2007  Added global methods and variables
//   J.L. Mundy    January  30, 2008  Migrated back to brl/bpro,adapted to brdb
// \endverbatim
//--------------------------------------------------------------------------------

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters_sptr.h>


//: This abstract class is the base class for process objects
//
// The process manager uses polymorphism to manage and execute
// classes derived from this one.  The process manager will call
// your execute function once for every frame of the video.  The 
// process manager will call your finish function once after all
// calls to execute are finished.
//
// To make a working video process you must overload the following:
// - clone
// - name
// - init
// - execute
// - finish
//
// The clone and name function are trivial.  The clone function
// creates a copy of your process.  The following should be 
// sufficient in almost all cases (in some cases you may need
// to write an explicit copy constructor).
// \verbatim
// bprb_process* bprb_my_process::clone() const
// {
//   return new bprb_my_process(*this);
// }
// \endverbatim
// The name function should simply return a string containing 
// a unique name.  Think of this as a title for your process.
// It must be unique for identification, but it may also 
// appear in menus to allow a user to select your process.
//
// Typically, the constructor is used to define the parameters
// for a process. See bprb_parameters. 
class bprb_process : public vbl_ref_count
{
 public:

  //: Clone the process
  virtual bprb_process* clone() const = 0;
  
  //: Return a smart pointer to the parameters
  bprb_parameters_sptr parameters();

  //: Set the parameters
  void set_parameters(const bprb_parameters_sptr& params);

  //: The name of the process
  virtual vcl_string name() = 0;

  //: The name and type of each input, <name, value type>
  virtual vcl_vector<vcl_pair<vcl_string, vcl_string> > inputs() = 0;

  //: The name and type of each output, <name, value type>
  virtual vcl_vector<vcl_pair<vcl_string, vcl_string> > outputs() = 0;

  //: The set of inputs
  void set_input_data(vcl_vector<brdb_value> > const& inputs){input_data=inputs;}

  //: The set of outputs
  void vcl_vector<brdb_value> >& output_data(){return=output_data;}


  //: Perform any initialization required by the process
  virtual bool init() = 0;

  //: Execute the process 
  virtual bool execute() = 0;

  //: Perform any clean up or final computation 
  virtual bool finish() = 0;
  
 protected:
  
  //: Copy Constructor
  bprb_process(const bprb_process& other);
  bprb_process();
  virtual ~bprb_process();

 private:
  //: The parameters of this process
  vcl_vector<brdb_value> input_data_;
  vcl_vector<brdb_value> output_data_;
  bprb_parameters_sptr parameters_;
};
#include <bprb/bprb_process_sptr.h>
#endif // bprb_process_h_
