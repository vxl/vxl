// This is brl/bseg/bmrf/bmrf_curve_3d.h
#ifndef bmrf_curve_3d_h_
#define bmrf_curve_3d_h_
//:
// \file
// \brief A 3D curve 
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/23/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <bmrf/bmrf_curvel_3d_sptr.h>
#include <vnl/vnl_double_3x4.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>

//: A 3D curve
class bmrf_curve_3d : public vcl_list<bmrf_curvel_3d_sptr>, public vbl_ref_count
{
 public:
  //: Constructor
  bmrf_curve_3d();

  //: Destructor
  ~bmrf_curve_3d() {}

  //: Trim the ends of the curve with few correspondences
  void trim(int min_prj);

  //: Trim curvels with large deviation in gamma
  void stat_trim(double max_std);

  //: Attempt to fill in missing correspondences
  void fill_gaps(const vcl_set<int>& frames, double da);

  //: Attempt to interpolate artificial values for missing correspondences
  void interp_gaps(const vcl_set<int>& frames);

  //: Simultaneously reconstruct all points in a 3d curve
  void reconstruct(const vcl_map<int,vnl_double_3x4>& cameras, float sigma = 0.5);

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

};


//: Binary save bmrf_curve_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, const bmrf_curve_3d* n);

//: Binary load bmrf_curve_3d* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_curve_3d* &n);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bmrf_curve_3d* n);


#endif // bmrf_curve_3d_h_
