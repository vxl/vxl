#ifndef mfpf_mr_point_finder_builder_h_
#define mfpf_mr_point_finder_builder_h_

//:
// \file
// \author Tim Cootes
// \brief Builder for mfpf_mr_point_finder objects.

#include <mbl/mbl_cloneable_ptr.h>
#include <mfpf/mfpf_point_finder_builder.h>

class vimt_image_pyramid;
class mfpf_mr_point_finder;
class mfpf_pose;

//: Builder for mfpf_mr_point_finder objects.
// Contains a set of mfpf_point_finder_builders, 
// each designed to work at a different resolution.  
class mfpf_mr_point_finder_builder {
protected:

  //: Set of cost function objects.
  vcl_vector<mbl_cloneable_ptr<mfpf_point_finder_builder> > builders_;

public:

    //: Dflt ctor
  mfpf_mr_point_finder_builder();

    //: Destructor
  virtual ~mfpf_mr_point_finder_builder();

  //: Number of builders
  unsigned size() const { return builders_.size(); }

  //: Builder at level L
  const mfpf_point_finder_builder& builder(unsigned L) const 
  { assert (L<builders_.size()); return *builders_[L]; }

  //: Builder at level L
  mfpf_point_finder_builder& builder(unsigned L)
  { assert (L<builders_.size()); return *builders_[L]; }

  //: Define builders.  Clone of each taken
  void set(const vcl_vector<mfpf_point_finder_builder*>& builders);

  //: Set up n builders, with step size step0*scale_step^L
  //  Takes clones of builder and sets up step sizes.
  //  Top level search parameters retained.
  //  Finer res models have search area and scale/angle
  //  ranges set to allow efficient refinement.
  void set(const mfpf_point_finder_builder& builder,
           unsigned n, double step0, double scale_step);

  //: Set up multiple builders to cover patch of size wi x wj
  //  Patch defined in world co-ords.  Selects suitable scales
  //  and levels so that region is covered and at any given
  //  level the model width (in pixels) is in the range 
  //  [min_pixel_width,max_pixel_width]
  void set_size_and_levels(
                const mfpf_point_finder_builder& builder0,
                double wi, double wj,
                double scale_step, 
                int min_pixel_width, 
                int max_pixel_width);

  //: Define region size in world co-ordinates
  //  Sets up ROI in each model to cover given box (in world coords),
  //  with ref point at centre.
  //  Assumes that models exist and have step size defined.
  void set_region_size(double wi, double wj);

  //: Select best level for building model using u as basis
  //  Selects pyramid level with pixel sizes best matching
  //  the model pixel size at given basis vector u.
  unsigned image_level(unsigned i, 
                       const vgl_vector_2d<double>& u,
                       const vimt_image_pyramid& im_pyr) const;

  //: Initialise building
  // Must be called before any calls to add_example(...)
  void clear(unsigned n_egs);

  //: Add one example to the model
  void add_example(const vimt_image_pyramid& image_pyr,
                   const vgl_point_2d<double>& p,
                   const vgl_vector_2d<double>& u);

  //: Build object from the data supplied in add_example()
  void build(mfpf_mr_point_finder&);

    //: Version number for I/O
  short version_no() const;

    //: Name of the class
  virtual vcl_string is_a() const;

    //: Print class to os
  virtual void print_summary(vcl_ostream& os) const;

    //: Save class to binary file stream
  virtual void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  virtual void b_read(vsl_b_istream& bfs);
};

  //: Stream output operator for class reference
vcl_ostream& operator<<(vcl_ostream& os,const mfpf_mr_point_finder_builder& b);

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mfpf_mr_point_finder_builder& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mfpf_mr_point_finder_builder& b);

#endif // mfpf_mr_point_finder_builder_h_


