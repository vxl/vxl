#ifndef msdi_marked_images_from_files_h_
#define msdi_marked_images_from_files_h_
//:
// \file
// \author Tim Cootes
// \brief Iterator for images and points stored in files

#include <msdi/msdi_marked_images.h>
#include <msm/msm_points.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

//: Iterator for images and points stored in files
//  Loads in points and images on demand.
//  Default behaviour is to load in image into a vil_image_view<vxl_byte>
//  using vil_load(), and assume an identity world to image transformation.
//  If the load_as_float_ flag is true, then loads the named image and associated
//  transformation into a vimt_image_2d_of<float>, using vimt_load(), with an additional
//  scaling factor of unit_scaling_.  This allows converting from metres to mm if
//  necessary. 
class msdi_marked_images_from_files : public msdi_marked_images
{
 private:
  //: Current points
  msm_points points_;

  //: Current image pyramid
  vimt_image_pyramid image_pyr_;

  //: Current image pyramid (using float)
  vimt_image_pyramid float_image_pyr_;

  //: Current base image
  vimt_image_2d_of<vxl_byte> image_;

  //: Current base image (using float)
  vimt_image_2d_of<float> float_image_;

  //: When true, convert multiplane images to single plane
  bool grey_only_;

  //: True if image_ is current
  bool image_ok_;

  //: True if image_pyr_ is current
  bool image_pyr_ok_;

  //: True if points_ are current
  bool points_ok_;

  //: When true, load image into a float image
  bool load_as_float_;
  
  //: Scaling required to convert from units in image to desired world units 
  // (e.g. 1000 for mm if image units are metres)
  // Only used if load_as_float_ is true.
  float unit_scaling_;

  //: Image directory
  vcl_string image_dir_;

  //: Image names
  vcl_vector<vcl_string> image_name_;

  //: Points directory
  vcl_string points_dir_;

  //: Points file names
  vcl_vector<vcl_string> points_name_;

  //: Index of current image
  int index_;

  //: Load in current image and generate suitable pyramid
  void get_image();

  //: Load in current points
  void get_points();

  //: Pyramid builder
  vimt_gaussian_pyramid_builder_2d<vxl_byte> pyr_builder_;

  //: Float pyramid builder
  vimt_gaussian_pyramid_builder_2d<float> float_pyr_builder_;

  // Private copy operator to prevent copying
  msdi_marked_images_from_files& operator=(const msdi_marked_images_from_files&);
 public:
  //: Default constructor
  msdi_marked_images_from_files();

  //: Construct with directories and filenames
  msdi_marked_images_from_files(const vcl_string& image_dir,
                                const vcl_vector<vcl_string>& image_names,
                                const vcl_string& points_dir,
                                const vcl_vector<vcl_string>& points_names);

  //: Initialise with directories and filenames
  void set(const vcl_string& image_dir,
           const vcl_vector<vcl_string>& image_names,
           const vcl_string& points_dir,
           const vcl_vector<vcl_string>& points_names,
           bool load_as_float=false);

  //: Initialise with directories and filenames
  //  \a points_names[i] is set to \a image_names[i]+".pts"
  void set(const vcl_string& image_dir,
           const vcl_vector<vcl_string>& image_names,
           const vcl_string& points_dir);
  
  //: Scaling required to convert from units in image to desired world units 
  // (e.g. 1000 for mm if image units are metres)
  // Only used if load_as_float_ is true.
  float unit_scaling() const { return unit_scaling_; }

  //: Scaling required to convert from units in image to desired world units 
  // (e.g. 1000 for mm if image units are metres)
  // Only used if load_as_float_ is true.
  void set_unit_scaling(float);


  // Destructor
  virtual ~msdi_marked_images_from_files();

  //: Pyramid builder to be used
  vimt_gaussian_pyramid_builder_2d<vxl_byte>& pyr_builder()
  { return pyr_builder_; }

  //: Pyramid builder to be used for float images
  vimt_gaussian_pyramid_builder_2d<float>& float_pyr_builder()
  { return float_pyr_builder_; }

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

#endif // msdi_marked_images_from_files_h_
