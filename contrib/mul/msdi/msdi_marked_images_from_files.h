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
#include <msm/msm_reflect_shape.h>
#include <vil/vil_flip.h>

#include <mbl/mbl_read_props.h>
#include <mbl/mbl_parse_int_list.h>
#include <vul/vul_string.h>


//: Define whether to use reflections of images and/or points.
// Raw: Return raw data
// ReflectOnly: Return reflected points and reflected images (but not originals)
// OnlyReflectIm: Return reflected images and original points from files (without reflecting them)
// ReflectSym: Return raw + reflected images, with original points + their reflections
// ReflectAsymRawPts: Return raw image+points, then reflected image and raw points (separate file)
// ReflectAsymRefPts: Return raw image+points, then reflected image and reflected points (separate file)
enum msdi_reflection_state { Raw, ReflectOnly, OnlyReflectIm, ReflectSym, ReflectAsymRawPts, ReflectAsymRefPts };

//: Return string (e.g. "Raw", "OnlyReflectIm" etc) for given state
std::string msdi_string_from_state(msdi_reflection_state);

//: Convert string to state, returning true if valid string supplied.
bool msdi_state_from_string(const std::string& str, msdi_reflection_state& state);


//: Iterator for images and points stored in files
//  Loads in points and images on demand. Also enables reflections of points/images.
//  Default behaviour is to load in image into a vil_image_view<vxl_byte>
//  using vil_load(), and assume an identity world to image transformation.
//  If the load_as_float_ flag is true, then loads the named image and associated
//  transformation into a vimt_image_2d_of<float>, using vimt_load(), with an additional
//  scaling factor of unit_scaling_.  This allows converting from metres to mm if
//  necessary.
//
// The way that reflections are used is controlled by the msdi_reflection_state value.
// In summary:
// Raw: Return raw data
// ReflectOnly: Return reflected points and reflected images (but not originals)
// OnlyReflectIm: Return reflected images and original points from files (without reflecting them)
// ReflectSym: Return raw image + raw points, then reflected image and reflected points.
// ReflectAsymRawPts: Return raw image+points, then reflected image and raw points (separate file)
// ReflectAsymRefPts: Return raw image+points, then reflected image and reflected points (separate file)
class msdi_marked_images_from_files : public msdi_marked_images
{
 private:
  //: Define behaviour of this iterator regarding reflections of points/images
  msdi_reflection_state ref_state_;

  //: Text to be added to beginning of points names to get reflected version
  //  Only significant for ref_state_==ReflectAsymRawPts
  std::string ref_prefix_;

  //: Define point mapping for reflected points
  //  ref_point_index_[i] is index in old list of reflected point i
  //  Only used for ReflectSym state.
  std::vector<unsigned> ref_point_index_;

  //: When true, convert multiplane images to single plane
  bool grey_only_;

  //: Scaling required to convert from units in image to desired world units
  // (e.g. 1000 for mm if image units are metres)
  // Only used if load_as_float_ is true.
  float unit_scaling_;

  //: Image directory
  std::string image_dir_;

  //: Image names
  std::vector<std::string> image_name_;

  //: Points directory
  std::string points_dir_;

  //: Points file names
  std::vector<std::string> points_name_;

  //  ==== Workspace variables ====

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


  //: True if image is reflection of that loaded from disk
  bool image_is_reflected_;

  //: True if points are a reflection of those loaded from disk
  bool points_are_reflected_;

  //: True if image_ is current
  bool image_ok_;

  //: True if image_pyr_ is current
  bool image_pyr_ok_;

  //: True if points_ are current
  bool points_ok_;

  //: When true, load image into a float image
  bool load_as_float_;


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
  msdi_marked_images_from_files& operator=(const msdi_marked_images_from_files&) = delete;

  //: Reflect current points
  void reflect_points();

  //: Reflect current image
  void reflect_image();

 public:
  //: Default constructor
  msdi_marked_images_from_files();

  //: Construct with directories and filenames
  msdi_marked_images_from_files(const std::string& image_dir,
                                const std::vector<std::string>& image_names,
                                const std::string& points_dir,
                                const std::vector<std::string>& points_names);

  //: Initialise with directories and filenames
  void set(const std::string& image_dir,
           const std::vector<std::string>& image_names,
           const std::string& points_dir,
           const std::vector<std::string>& points_names,
           bool load_as_float=false);

  //: Initialise with directories and filenames
  //  \a points_names[i] is set to \a image_names[i]+".pts"
  void set(const std::string& image_dir,
           const std::vector<std::string>& image_names,
           const std::string& points_dir,
           bool load_as_float=false);

  //: Define behaviour with reflections
  // Options: Raw,ReflectOnly,OnlyReflectIm,ReflectAsymRawPts,ReflectSym
  void set_state(msdi_reflection_state s);

  //: Indicate behaviour with reflections
  // Options: Raw,ReflectOnly,OnlyReflectIm,ReflectAsymRawPts,ReflectAsymRefPts,ReflectSym
  msdi_reflection_state reflection_state() const { return ref_state_; }

  //: Define prefix to be used for reflected points.
  // Default is "ref_".  Only used for ReflectAsymRawPts state.
  void set_ref_prefix(const std::string& ref_prefix);

  //: Define point mapping for reflected points
  //  rpi[i] is index in old list of reflected point i
  //  Only used for ReflectSym state.
  void set_ref_point_index(const std::vector<unsigned>& rpi);


  //: When true, all images converted to greyscale (1 plane) on loading
  void set_convert_to_greyscale(bool b);

  //: When true, all images converted to greyscale on loading
  bool grey_only() const { return grey_only_; }

  //: Scaling required to convert from units in image to desired world units
  // (e.g. 1000 for mm if image units are metres)
  // Only used if load_as_float_ is true.
  float unit_scaling() const { return unit_scaling_; }

  //: Scaling required to convert from units in image to desired world units
  // (e.g. 1000 for mm if image units are metres)
  // Only used if load_as_float_ is true.
  void set_unit_scaling(float);


  // Destructor
  ~msdi_marked_images_from_files() override;

  //: Pyramid builder to be used
  vimt_gaussian_pyramid_builder_2d<vxl_byte>& pyr_builder()
  { return pyr_builder_; }

  //: Pyramid builder to be used for float images
  vimt_gaussian_pyramid_builder_2d<float>& float_pyr_builder()
  { return float_pyr_builder_; }

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

  //: Image directory
  const std::string& image_dir() const { return image_dir_; }

  //: List of image names
  const std::vector<std::string>& image_names() const { return image_name_; }

  //: Points directory
  const std::string& points_dir() const { return points_dir_; }

  //: List of points file names
  const std::vector<std::string>& points_names() const { return points_name_; }

  //: Set up from information in props object
  // Expects to find parameters for reflection_symmetry, reflection_state and ref_prefix.
  void set_reflection_state_from_props(mbl_read_props_type& props);

  //: Set up from information in props object
  // Expects to find parameters for images, image_dir, points_dir, and optionally
  // reflection_symmetry, reflection_state and ref_prefix.
  void set_from_props(mbl_read_props_type& props);

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  //  Expects something like:
  //  \verbatim
  //  reflection_state: ReflectSym
  //  reflection_symmetry: { 7 6 5 4 3 2 1 0 }
  //  ref_prefix: ref-
  //  max_im_pyr_levels: 5
  //  image_dir: /home/images/
  //  points_dir: /home/points/
  //  images: {
  //    image1.pts : image1.jpg
  //    image2.pts : image2.jpg
  //  }
  //  \endverbatim
  void read_from_file(const std::string& path);

};

//: Fill in elements of params object from props
// Sets params.reflection_symmetry, params.reflection_state, params.ref_prefix
template<class paramT>
inline void msdi_get_reflection_params_from_props(mbl_read_props_type& props, paramT& params)
{
  std::string ref_sym_str=props.get_optional_property("reflection_symmetry","");
  params.reflection_symmetry.resize(0);
  if (ref_sym_str!="")
  {
    std::stringstream ss(ref_sym_str);
    mbl_parse_int_list(ss, std::back_inserter(params.reflection_symmetry),
                       unsigned());
  }

  // For backwards compatability
  bool only_reflect=vul_string_to_bool(props.get_optional_property("only_reflect","false"));

  std::string ref_prefix=props.get_optional_property("ref_prefix","");

  std::string ref_state_str=props.get_optional_property("reflection_state","Undefined");
  if (ref_state_str=="Undefined")
  {
    // For backwards compatability.
    if (params.reflection_symmetry.size()>0)
    {
      if (only_reflect) params.reflection_state=ReflectOnly;
      else              params.reflection_state=ReflectSym;
    }
    else
    {
      if (only_reflect) params.reflection_state=ReflectOnly;
      else              params.reflection_state=Raw;
    }
  }
  else
  {
    if (!msdi_state_from_string(ref_state_str,params.reflection_state))
    {
      throw (mbl_exception_parse_error("Unknown reflection_state: "+ref_state_str));
    }
  }

}

#endif // msdi_marked_images_from_files_h_
