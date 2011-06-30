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
  vcl_string image_dir_;

  //: Pointer to external vector of images
  const vcl_vector<vil_image_view<vxl_byte> > *images_;

  //: Pointer to external vector of points
  const vcl_vector<msm_points> *points_;

  //: Index of current image
  int index_;

  //: Generate suitable pyramid from current image
  void get_image();

  //: Pyramid builder
  vimt_gaussian_pyramid_builder_2d<vxl_byte> pyr_builder_;

  // Private copy operator to prevent copying
  msdi_array_of_marked_images& operator=(const msdi_array_of_marked_images&);
public:
    //: Default constructor
  msdi_array_of_marked_images();

    //: Construct with external vectors of images and points
    //  Pointers retained to both - they must stay in scope.
  msdi_array_of_marked_images(
                 const vcl_vector<vil_image_view<vxl_byte> >& images,
                 const vcl_vector<msm_points>& points,
                 bool grey_only=true);

    //: Initialise with external vectors of images and points
    //  Pointers retained to both - they must stay in scope.
  void set(const vcl_vector<vil_image_view<vxl_byte> >& images,
           const vcl_vector<msm_points>& points,
           bool grey_only=true);

    //: Destructor
  virtual ~msdi_array_of_marked_images();

  //: Pyramid builder to be used
  vimt_gaussian_pyramid_builder_2d<vxl_byte>& pyr_builder()
  { return pyr_builder_; }

  //: Move to start of data
  virtual void reset();

  //: Move to next item.  Return true until reach end of items
  virtual bool next();

  //: Return number of examples this will provide
  virtual unsigned size() const;

  //: Return current image
  virtual const vimt_image_2d& image();

  //: Return current image pyramid
  virtual const vimt_image_pyramid& image_pyr();

  //: points for the current image
  virtual const msm_points& points();

    //: Return current image file name
  virtual vcl_string image_name() const;

    //: Return current points file name
  virtual vcl_string points_name() const;
};

#endif // msdi_array_of_marked_images_h_



