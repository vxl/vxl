// This is brl/bseg/sdet/sdet_region.h
#ifndef sdet_region_h_
#define sdet_region_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A region class for the watershed algorithm.
//
// \verbatim
//  Modifications
//   Initial version July 11, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_iosfwd.h>
#include <sdet/sdet_region_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vdgl/vdgl_digital_region.h>

class sdet_region : public vdgl_digital_region
{
 public:
  sdet_region();
  sdet_region(int npts, const float* xp, const float* yp,
              const unsigned short *pix);
  sdet_region(vdgl_digital_region const& reg);
  ~sdet_region() {}
  //:accessors
  void set_label(const unsigned int label){region_label_ = label;}
  unsigned int label(){return region_label_;}

  void set_boundary(vsol_polygon_2d_sptr const& boundary){boundary_ = boundary;}
  vsol_polygon_2d_sptr boundary();

  vdgl_digital_region* cast_to_digital_region(){return (vdgl_digital_region*) this;}

  //:utilities
  bool compute_boundary();
  // must implement comparison since containers need this
  struct compare
  {
    bool operator()(sdet_region_sptr const & l,
                    sdet_region_sptr const & r) const
    {
      if (!l||!r)
        return false;
      return l->Npix() < r->Npix();//Large areas are favored
    }
  };
 protected:
  unsigned int region_label_;
  bool boundary_valid_;
  vsol_polygon_2d_sptr boundary_;
};

vcl_ostream& operator<<(vcl_ostream& s, sdet_region const& sd);

#endif // sdet_region_h_
