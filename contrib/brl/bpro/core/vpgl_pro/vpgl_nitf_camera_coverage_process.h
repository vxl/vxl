// This is brl/bpro/core/vpgl_pro/vpgl_nitf_camera_coverage_process.h
#ifndef vpgl_nitf_camera_coverage_process_h_
#define vpgl_nitf_camera_coverage_process_h_
//:
// \file
// \brief A process for computing a list of images covering a Geographical point
// \author Isabel Restrepo (mir@lems.brown.edu)
// \date 08/08/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>
#include <vgl/vgl_point_2d.h>

//: Returns a list of all images covering a given geographical point.
//  It can be extended to a region
class vpgl_nitf_camera_coverage_process : public bprb_process
{
 public:

  vpgl_nitf_camera_coverage_process();

  //: Copy Constructor (no local data)
  vpgl_nitf_camera_coverage_process(const vpgl_nitf_camera_coverage_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vpgl_nitf_camera_coverage_process();

  //: Clone the process
  virtual vpgl_nitf_camera_coverage_process* clone() const {return new vpgl_nitf_camera_coverage_process(*this);}

  vcl_string name(){return "NITFCameraCoverageProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}

protected:
  void get_regions(vcl_string file,
                 vcl_vector< vgl_point_2d<double> >& region );
};

#endif //vpgl_nitf_camera_coverage_process_h_
