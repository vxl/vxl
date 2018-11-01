#ifndef mbl_progress_time_h_
#define mbl_progress_time_h_


//:
// \file
// \brief Progress class that outputs elapsed time reporting on progress
// \author Kevin de Souza
// \date 26 June 2008


#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <vul/vul_ios_state.h>
#include <mbl/mbl_progress.h>


//========================================================================
//: \brief Progress class that outputs elapsed time reporting on progress
class mbl_progress_time : public mbl_progress
{
public:

  //: Constructor
  // \param os Ostream for text output (defaults to cout)
  mbl_progress_time(std::ostream& os=std::cout);

  //: Destructor
  ~mbl_progress_time() override;

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


protected:

  std::ostream& os_;
  vul_timer timer_;
  vul_ios_state_saver ios_state_; // Restores stream format after this progress object is destroyed
};

//========================================================================

#endif // mbl_progress_time_h_
