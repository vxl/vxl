#ifndef mbl_progress_h_
#define mbl_progress_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief A base for classes which wish to take some action during a lengthy operation.
// \author 	Graham Vincent and Kevin de Souza	
// \date 25 Feb 2005	


#include <vcl_string.h>
#include <vsl/vsl_binary_io.h>


//========================================================================
//: An exception that can be thrown by an operation when cancelled.
class mbl_progress_cancel_exception
{
public:
  mbl_progress_cancel_exception() {}
  ~mbl_progress_cancel_exception() {}
};


//========================================================================
//: A base for classes which wish to take some action during a lengthy operation.
//
// An mbl_progress object can be used to keep track of multiple operations.
// Each one is identified by a unique string.
//
// A function implementing an algorithm may accept a mbl_progress object.
// Before entering a processing loop, the function passes the progress 
// object an identifying string together with an estimate of how many 
// iterations the loop is expected to take. During the subsequent 
// processing, the process informs the progress object when each iteration 
// has completed. It is up to the derived classes to act on this information.
//
// For example: derive a class containing a graphical progress bar, 
// which is updated when a particular identifier indicates that an 
// iteration has completed. 
//
// The algorithm may also query the progress object to check if it has 
// been cancelled (e.g. by a user pressing a cancel button in a 
// derived GUI class) and take appropriate action.
//
// Algorithms could have the identifier associated with various tasks in 
// their public interface.
class mbl_progress
{   
public:
  
  //: Constructor
  mbl_progress();
  
  
  //: Destructor
  virtual ~mbl_progress();
  
  
  //: Estimated number of iterations for the given identifier.
  void set_estimated_iterations(const vcl_string& identifier,
                                const int iterations,
                                const vcl_string& display_text);
  
  
  //: If true set_progress() will throw an exception if cancel has been set
  void set_throw_exception_on_cancel(bool t) 
    { throw_exception_on_cancel_=t; }
  
  
  //: Sets progress for the given identifier. 
  // Checks whether cancel has been set; if so calls end_progress(),
  // and throws an exception if "throw_exception_on_cancel" is true.
  void set_progress(const vcl_string& identifier,
                    const int progress);
  
  
  //: Increments progress for the given identifier by n.
  void increment_progress(const vcl_string& identifier, 
                          const int n=1);
  
  
  //: Explicitly marks the end of loop for the given identifier.
  void end_progress(const vcl_string& identifier);
  
  
  //: Gets progress for given identifier.
  //  \param identifier to query.
  //  \return progress (-i if identifier is not known by this object).
  int progress(const vcl_string& identifier) const;
  
  
  //: Gets display text for given identifier.
  //  \param identifier to query.
  vcl_string display_text(const vcl_string& identifier) const;
  
  
  //: Gets estimated total iterations for given identifier.
  //  \param identifier to query. 
  //  \return progress (-1 if identifier is not known by this object).
  int estimated_iterations(const vcl_string& identifier) const;
  
  
  //: Modify the flag to cancel the current process.
  //  \param identifier Progress object to cancel.
  void set_cancelled(const vcl_string& identifier, 
                     const bool cancel);
  
  
  //: Check whether progress object is marked as cancelled.
  //  \param identifier Progress object to check.
  //  \return True if a cancel flag is set for this progress object.
  bool is_cancelled(const vcl_string& identifier) const;
  
  
  //: Version number for I/O 
  short version_no() const;
  
  
  //: Name of the class
  virtual vcl_string is_a() const;
  
  
  //: Print class to os
  virtual void print_summary(vcl_ostream& os) const {};
  
  
protected:
  
  //: Called when set_estimate_iterations() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored. 
  virtual void on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations) = 0;
  
  
  //: Called when set_progress() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored. 
  //  \param progress The new progress status.
  virtual void on_set_progress(const vcl_string& identifier, 
                               const int progress) = 0;
  
  
  //: Called when end_progress() is called for a given identifier.
  //  Derived classes may take some action here.
  //  \param identifier The operation being monitored. 
  virtual void on_end_progress(const vcl_string& identifier) = 0;    
  
  
private:
  
  //: Stores display text for each identifier
  vcl_map<vcl_string, vcl_string> identifier2displaytext_;
  
  //: Stores estimated iterations for each identifier
  vcl_map<vcl_string, int> identifier2estimatediterations_;
  
  //: Stores current progress for each identifier
  vcl_map<vcl_string, int> identifier2progress_;
  
  //: Flags to indicate whether a request to cancel has been registered for each identifier
  vcl_map<vcl_string, bool> identifier2cancel_;
  
  //: If true set_progress() will throw an exception if cancel has been set.
  bool throw_exception_on_cancel_;
};

//========================================================================


#endif // mbl_progress_base_h_
