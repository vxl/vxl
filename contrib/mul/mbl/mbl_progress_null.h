#ifndef mbl_progress_null_h_
#define mbl_progress_null_h_
//:
// \file
// \brief Progress object that does nothing.
// \author Graham Vincent and Kevin de Souza
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
  ~mbl_progress_null() override;

  //: Name of the class
  virtual std::string is_a() const;

 protected:
  void on_set_estimated_iterations(const std::string& identifier,
                                           const int total_iterations) override;

  void on_set_progress(const std::string& identifier,
                               const int progress) override;

  void on_end_progress(const std::string& identifier) override;
};

//========================================================================

#endif // mbl_progress_null_h_
