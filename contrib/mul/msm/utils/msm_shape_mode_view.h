#ifndef msm_shape_mode_view_h_
#define msm_shape_mode_view_h_

//:
// \file
// \brief Generates shapes displaying modes of a shape model.
// \author Tim Cootes

#include <vgl/vgl_box_2d.h>
#include <msm/msm_ref_shape_instance.h>

//: Generates shapes displaying modes of a shape model.
//  Generates a set of shapes by varying one model parameter,
//  placing shapes in a row.  This is useful for display programs.
//
//  Essentially two modes of output can be generated.  The shapes
//  are either all overlapping (suitable for showing change if
//  plotted all together, or for generating a moving if plotted
//  sequentially), or are arranged in a row (useful for creating
//  a picture for a document).  Use set_overlap_shapes(bool) to
//  select which mode is required.
class msm_shape_mode_view {
private:
  const msm_ref_shape_model* shape_model_;
  msm_ref_shape_instance sm_inst_;

  vnl_vector<double> b_sd_;
  vnl_vector<double> b_;

  std::vector<msm_points> points_;

    //: Number of shapes for each mode
  unsigned n_per_mode_;

    //: Current mode
  unsigned mode_;

    //: Range in units of sd
  double sd_range_;

    //: Scale relative to optimal scale to fit shapes to window
  double rel_scale_;

    //: When true, overlap all the shapes
  bool overlap_shapes_;

  vgl_box_2d<int>  display_win_;

public:

    //: Dflt ctor
  msm_shape_mode_view();

    //: Destructor
  virtual ~msm_shape_mode_view() = default;

    //: Define shape model
  void set_shape_model(const msm_ref_shape_model&);

    //: Current shape model
  const msm_ref_shape_model& shape_model() const;

    //: When true, overlap all the shapes to highlight changes
  bool overlap_shapes() const { return overlap_shapes_; }

    //: When true, overlap all the shapes
  void set_overlap_shapes(bool b);

    //: Current array of sets of points
    //  These are the shapes to be displayed
  const std::vector<msm_points>& points() const { return points_; }

  //: Compute the shapes so they fit into current window
  // \param n_shapes  Number of shapes to display
  // \param range  Range (in sd) to vary shapes by
  // \param vary_one  If one shape and this is true then use range as parameter, else use 0
  void compute_shapes(unsigned n_shapes, double range, bool vary_one);

    //: Compute the shapes so they fit into current window
  void compute_shapes();

    //: Define current display window size
  void set_display_window(const vgl_box_2d<int> & win);

    //: Define width of display region for a single shape
    //  Height estimated from aspect ratio of mean shape
    //  Width of display window is then a multiple of this,
    //  depending on the number of shapes along the row.
  void set_display_width(double);

    //: Current display window
  const vgl_box_2d<int> & display_window() const
  { return display_win_; }

    //: Define current mode to use.
    //  Mode=1 is first shape mode
  void set_mode(unsigned m);

    //: Current mode
  unsigned mode() const { return mode_; }

    //: Define number of shapes per mode
  void set_n_per_mode(unsigned n);

    //: Current number of shapes per mode
  unsigned n_per_mode() const { return n_per_mode_; }

    //: Maximum number of shape modes available
  unsigned max_modes() const;

    //: Set range (in units of SD)
  void set_range(double r);

    //: Get range (in units of SD)
  double range() const { return sd_range_; }
};

#endif // msm_shape_mode_view_h_
