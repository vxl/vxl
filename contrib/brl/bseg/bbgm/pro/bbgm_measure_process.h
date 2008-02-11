// This is brl/bseg/bbgm/pro/bbgm_measure_process.h
#ifndef bbgm_measure_process_h_
#define bbgm_measure_process_h_

//:
// \file
// \brief A process for computing a measurement on a distribution image
// \author J.L. Mundy
// \date February 9, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process.h>


class bbgm_measure_process : public bprb_process
{
 public:
  
  bbgm_measure_process();

  //: Copy Constructor (no local data)
  bbgm_measure_process(const bbgm_measure_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bbgm_measure_process();

  //: Clone the process
  virtual bbgm_measure_process* clone() const {return new bbgm_measure_process(*this);}

  vcl_string name(){return "MeasureProcess";}

  bool init(){return true;} 
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bbgm_measure_process_h_
