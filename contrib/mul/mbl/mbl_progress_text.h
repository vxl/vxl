#ifndef mbl_progress_text_h_
#define mbl_progress_text_h_
#ifdef __GNUC__
#pragma interface
#endif


//:
// \file
// \brief Progress class that outputs simple text reporting on progress
// \author 	Graham Vincent and Kevin de Souza	
// \date 25 Feb 2005	


#include <mbl/mbl_progress.h>


//========================================================================
//: Progress object that outputs simple text reporting on progress
class mbl_progress_text : public mbl_progress
{
  
public:
  
  //: Constructor
  mbl_progress_text() {}
  
  //: Destructor
  ~mbl_progress_text() {}
  
  //: Name of the class
  virtual vcl_string is_a() const { return "mbl_progress_text"; }
  

protected:
  
  virtual void on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations)
  { vcl_cout << "Starting " << identifier << vcl_endl; }
  

  virtual void on_set_progress(const vcl_string& identifier, 
                               const int progress)
  { 
    vcl_cout << "Progress for " << identifier << " is " << progress 
             << " (out of " << estimated_iterations(identifier) << ")" 
             << vcl_endl; 
  }
  
  
  virtual void on_end_progress(const vcl_string &identifier)
  { vcl_cout << "Finishing " << identifier << vcl_endl; }
  
};
//========================================================================


#endif // mbl_progress_text_h_
