// This is brl/bseg/brip/brip_roi.h
#ifndef brip_roi_h_
#define brip_roi_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A composite region of interest class for image processing operations
//
// Provides a set of rectangular regions for processing in a larger image.
//
// \verbatim
//  Modifications
//   Initial version October 6, 2003
//   Actual implementation July 12, 2004
//   10-sep-2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_box_2d_sptr.h>

class brip_roi : public vbl_ref_count
{
 public:
  //:should initialize with image bounds on construction
  brip_roi(const unsigned n_image_cols = 0, const unsigned n_image_rows = 0);
  brip_roi(brip_roi const& r)
    : vbl_ref_count(), n_image_cols_(r.n_image_cols_),
      n_image_rows_(r.n_image_rows_), regions_(r.regions_) {}

  //:expand (or contract) each region of the roi by delta, creating a new roi
  brip_roi(brip_roi const& roi, const float delta);

  ~brip_roi() {}
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
  vsol_box_2d_sptr clip_to_image_bounds(vsol_box_2d_sptr box);
  //:clips all existing regions, doesn't affect existing image bounds
  void clip_to_image_bounds();
  bool empty() const;
  unsigned int n_regions() const { return regions_.size(); }
  int cmin(const unsigned i) const; //!< column minimum for region_i
  int cmax(const unsigned i) const; //!< column maximum for region_i
  int rmin(const unsigned i) const; //!< row minimum for region_i
  int rmax(const unsigned i) const; //!< row maximum for region_i
  unsigned csize(const unsigned i) const; //!< number of cols for region_i
  unsigned rsize(const unsigned i) const; //!< number of rows for region_i

  //:image column and row coordinates from local roi coordinates for region i
  unsigned ic(int local_col, unsigned i = 0) const;
  unsigned ir(int local_row, unsigned i = 0) const;

  //:local roi column and row coordinates from global image coordinates for region i
  unsigned lc(unsigned global_col, unsigned i = 0) const;
  unsigned lr(unsigned global_row, unsigned i = 0) const;

  vsol_box_2d_sptr region(unsigned i) { return regions_[i]; }
  bool remove_region(unsigned i);

 protected:
  unsigned n_image_cols_;
  unsigned n_image_rows_;
  vcl_vector<vsol_box_2d_sptr> regions_;
};

#include <brip/brip_roi_sptr.h>

#endif // brip_roi_h_
