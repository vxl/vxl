#ifndef mbl_progress_null_h_
#define mbl_progress_null_h_
#ifdef __GNUC__
#pragma interface
#endif


//:
// \file
// \brief Progress object that does nothing.
// \author 	Graham Vincent and Kevin de Souza
// \date 25 Feb 2005


#include <mbl/mbl_progress.h>


//========================================================================
//: Progress object that does nothing.
class mbl_progress_null : public mbl_progress
{
public:
  //: Constructor
  mbl_progress_null();
  
  //: Destructor
  ~mbl_progress_null();

  //: Name of the class
  virtual vcl_string is_a() const;
  
protected:
  virtual void on_set_estimated_iterations(const vcl_string& identifier,
                                           const int total_iterations);

  virtual void on_set_progress(const vcl_string& identifier, 
                               const int progress);
  
  virtual void on_end_progress(const vcl_string& identifier);
};
//========================================================================


#endif // mbl_progress_null_h_
