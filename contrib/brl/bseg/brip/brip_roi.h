//-*-c++-*-
// This is brl/bseg/brip/brip_roi.h
#ifndef brip_roi_h_
#define brip_roi_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A composite region of interest class for image processing operations
//
// \verbatim
// Provides a set of rectangular regions for processing in a larger image.
// 
// 
// 
// 
//  Modifications
//   Initial version October 6, 2003
//   Actual implementation July 12, 2004
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_box_2d_sptr.h>

class brip_roi : public vbl_ref_count
{
 public:
  //:should intialize with image bounds on construction
  brip_roi(const int n_image_cols = 0, const int n_image_rows = 0);
  ~brip_roi() {};
  //:replace existing image bounds
  void set_image_bounds(const int n_image_cols,
                        const int n_image_rows);

  //:roi from origin and size
  void add_region(const int x0, const int y0, const int xs, const int ys);
  //roi from center and radius
  void add_region(const int xc, const int yc, const int radius);
  //roi from box
  void add_region(vsol_box_2d_sptr const & box);

  //:useful for inserting a new region, doesn't affect existing image bounds
  vsol_box_2d_sptr clip_to_image_bounds(vsol_box_2d_sptr box, 
                                        const int n_image_cols,
                                        const int n_image_rows);
  //:clips all existing regions, doesn't affect existing image bounds
  void clip_to_image_bounds(const int n_image_cols,
                            const int n_image_rows);
  bool empty();
  int n_regions(){return regions_.size();}
  int cmin(int i);//column minimum for region_i
  int cmax(int i);//column maximum for region_i
  int rmin(int i);//row minimum for region_i
  int rmax(int i);//row maximum for region_i

  //:image column and row coordinates from roi coordinates for region i
  int ic(int col, int i = 0);
  int ir(int row, int i = 0);

  vsol_box_2d_sptr region(int i){return regions_[i];}
  bool remove_region(int i);
protected:

  int n_image_cols_;
  int n_image_rows_;
  vcl_vector<vsol_box_2d_sptr> regions_;
};
#include <brip/brip_roi_sptr.h>
#endif // brip_roi_h_
