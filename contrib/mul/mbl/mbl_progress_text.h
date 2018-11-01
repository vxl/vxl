#ifndef mbl_progress_text_h_
#define mbl_progress_text_h_
//:
// \file
// \brief Progress class that outputs simple text reporting on progress
// \author Graham Vincent and Kevin de Souza
// \date 25 Feb 2005

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_progress.h>

//========================================================================
//: Progress object that outputs simple text reporting on progress
class mbl_progress_text : public mbl_progress
{
 public:

  //: Constructor
  // \param os Ostream for text output (defaults to cout)
  mbl_progress_text(std::ostream& os=std::cout);

  //: Destructor
  ~mbl_progress_text() override;

  //: Name of the class
  virtual std::string is_a() const;


 protected:

  //: Called when set_estimate_iterations() is called for a given identifier.
  //  \param identifier The operation being monitored.
  void on_set_estimated_iterations(const std::string& identifier,
                                           const int total_iterations) override;

  //: Called when set_progress() is called for a given identifier.
  //  \param identifier The operation being monitored.
  //  \param progress The new progress status.
  void on_set_progress(const std::string& identifier,
                               const int progress) override;

  //: Called when end_progress() is called for a given identifier.
  //  \param identifier The operation being monitored.
  void on_end_progress(const std::string &identifier) override;

  std::ostream& os_;
};

//========================================================================

#endif // mbl_progress_text_h_
