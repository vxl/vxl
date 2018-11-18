#ifndef msdi_array_of_marked_images_h_
#define msdi_array_of_marked_images_h_

//:
// \file
// \author Tim Cootes
// \brief Iterator for images and points stored in arrays

#include <msdi/msdi_marked_images.h>
#include <msm/msm_points.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

//: Iterator for images and points stored in external arrays
//  Steps through external points and images on demand.
class msdi_array_of_marked_images : public msdi_marked_images {
private:
  //: Current image pyramid
  vimt_image_pyramid image_pyr_;

  //: Current base image
  vimt_image_2d_of<vxl_byte> image_;

  //: When true, convert multiplane images to single plane
  bool grey_only_;

  //: True if image_ is current
  bool image_ok_;

  //: True if image_pyr_ is current
  bool image_pyr_ok_;

  //: Image directory
  std::string image_dir_;

  //: Pointer to external vector of images
  const std::vector<vil_image_view<vxl_byte> > *images_;

  //: Pointer to external vector of points
  const std::vector<msm_points> *points_;

  //: Index of current image
  int index_;

  //: Generate suitable pyramid from current image
  void get_image();

  //: Pyramid builder
  vimt_gaussian_pyramid_builder_2d<vxl_byte> pyr_builder_;

  // Private copy operator to prevent copying
  msdi_array_of_marked_images& operator=(const msdi_array_of_marked_images&) = delete;
public:
    //: Default constructor
  msdi_array_of_marked_images();

    //: Construct with external vectors of images and points
    //  Pointers retained to both - they must stay in scope.
  msdi_array_of_marked_images(
                 const std::vector<vil_image_view<vxl_byte> >& images,
                 const std::vector<msm_points>& points,
                 bool grey_only=true);

    //: Initialise with external vectors of images and points
    //  Pointers retained to both - they must stay in scope.
  void set(const std::vector<vil_image_view<vxl_byte> >& images,
           const std::vector<msm_points>& points,
           bool grey_only=true);

    //: Destructor
  ~msdi_array_of_marked_images() override;

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

#endif // msdi_array_of_marked_images_h_
