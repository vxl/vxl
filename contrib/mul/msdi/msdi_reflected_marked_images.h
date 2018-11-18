#ifndef msdi_reflected_marked_images_h_
#define msdi_reflected_marked_images_h_

//:
// \file
// \author Tim Cootes
// \brief Adaptor which generates reflected versions of images/points

#include <msdi/msdi_marked_images.h>
#include <msm/msm_points.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

//: Adaptor which generates reflected versions of images/points
//  Given a source msdi_marked_images, generates a reflected version
//  of each image/points pair.
//  If only_reflect, then returns reflected version only.
//  If not only_reflect, then return both reflected and original
//  examples.
class msdi_reflected_marked_images : public msdi_marked_images {
private:
  //: Original data
  msdi_marked_images& marked_images_;

  //: Indicate correspondences between shape and its reflection
  std::vector<unsigned> sym_pts_;

  //: When true, supply only reflection of original data
  //  When false, supply original and reflection.
  bool only_reflect_;

  //: Current image pyramid
  vimt_image_pyramid image_pyr_;

  //: Current base image
  vimt_image_2d_of<vxl_byte> image_;

  //: Current points
  msm_points points_;

  //: True on first pass
  bool first_pass_;

  //: True if points are current
  bool points_ok_;

  //: True if image_ is current
  bool image_ok_;

  //: True if image_pyr_ is current
  bool image_pyr_ok_;

  //: Generate current image
  void get_image();

  //: Generate current points
  void get_points();

  //: Pyramid builder
  vimt_gaussian_pyramid_builder_2d<vxl_byte> pyr_builder_;

  // Private copy operator to prevent copying
  msdi_reflected_marked_images& operator=(const msdi_reflected_marked_images&);
public:
    //: Default constructor
    // /params sym_pts[i] indicates symmetric match to point i
    // /params only_reflect: When true, only supply reflected shapes.
  msdi_reflected_marked_images(msdi_marked_images& raw_data,
                             std::vector<unsigned>  sym_pts,
                             bool only_reflect=false);

    //: Construct with external vectors of images and points
    //  Pointers retained to both - they must stay in scope.
  msdi_reflected_marked_images();

    //: Destructor
  ~msdi_reflected_marked_images() override;

  //: Pyramid builder to be used
  vimt_gaussian_pyramid_builder_2d<vxl_byte>& pyr_builder()
  { return pyr_builder_; }

  //: Move to start of data
  void reset() override;

  //: Move to next item.  Return true until reach end of items
  bool next() override;

  //: Return number of examples this will provide
  unsigned size() const override;

  //: Return current image
  const vimt_image_2d& image() override;

  //: Return current image pyramid
  const vimt_image_pyramid& image_pyr() override;

  //: points for the current image
  const msm_points& points() override;

    //: Return current image file name
  std::string image_name() const override;

    //: Return current points file name
  std::string points_name() const override;
};

#endif // msdi_reflected_marked_images_h_
