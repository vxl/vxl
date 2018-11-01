#ifndef vil_pyramid_image_list_h_
#define vil_pyramid_image_list_h_
//:
// \file
// \brief A pyramid image resource based on multiple file-based image resources
// \author J.L. Mundy March 20, 2006
// \verbatim
//  Modifications:
//   Nhon Trinh  07/11/2009  Changed definition of difference in scale to be log-based
//                           New definition: diff(a,b) = abs(log(a/b))
// \endverbatim

#include <string>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_file_format.h>
#include <vil/vil_pyramid_image_resource.h>

//The pyramid resource is made up of a set of image resources in
// a single directory - the image list.
class vil_pyramid_image_list_format : public vil_file_format
{
 public:
  ~vil_pyramid_image_list_format() override = default;

  //: Return a character string which uniquely identifies this format.
  //E.g. "pnm", "jpeg", etc.
  char const* tag() const override {return "pyil";}//pyramid image list

  //: should return 0 so that no attempt is made to create a single image resource
  vil_image_resource_sptr make_input_image(vil_stream* /*vs*/) override
  { return nullptr; }

  //: Read a pyramid resource. Image list files are stored in directory.
  vil_pyramid_image_resource_sptr
  make_input_pyramid_image(char const* directory) override;


  //: Construct a pyramid image resource from a base image
  //  Each level has the same scale ratio (0.5) to the preceding level.
  //  Level 0 is the original base image. If copy base is false then
  //  Level 0 is already present in the directory and is used without
  //  modification. Each pyramid file in the directory is named
  //  filename + "level_index", e.g. R0, R1, ... Rn.
  vil_pyramid_image_resource_sptr
  make_pyramid_image_from_base(char const* directory,
                               vil_image_resource_sptr const& base_image,
                               unsigned int nlevels,
                               bool copy_base,
                               char const* level_file_format,
                               char const* filename) override;

  //: A non-pyramid output image doesn't make sense here
  vil_image_resource_sptr make_output_image(vil_stream* /*vs*/,
                                                    unsigned int /*ni*/,
                                                    unsigned int /*nj*/,
                                                    unsigned int /*nplanes*/,
                                                    enum vil_pixel_format) override
  { return nullptr; }

  vil_pyramid_image_resource_sptr
    make_pyramid_output_image(char const* directory) override;
};

struct pyramid_level
{
  pyramid_level(vil_image_resource_sptr const& image)
  : scale_(1.0f), image_(image), cur_level_(0) {}
  //: scale associated with level
  float scale_;

  //:the resource
  vil_image_resource_sptr image_;

  //:the current pyramid level for this resource
  unsigned int cur_level_;

  //:print ni and scale and values
  void print(const unsigned int l)
  {
    std::cout << "level[" << l <<  "]  scale: " << scale_
             << "  ni: " << image_->ni() << '\n';
  }
};


class vil_pyramid_image_list : public vil_pyramid_image_resource
{
 public:
  vil_pyramid_image_list(char const* directory);
  vil_pyramid_image_list(std::vector<vil_image_resource_sptr> const& images);
  ~vil_pyramid_image_list() override;
  //: The number of planes (or components) in the image.
  // This method refers to the base (max resolution) image
  // Dimensions:  Planes x ni x nj.
  // This concept is treated as a synonym to components.
  inline unsigned int nplanes() const override
  {
    if (levels_.size()>0)
      return levels_[0]->image_->nplanes();
    else
      return 0;
  }
  //: The number of pixels in each row.
  // This method refers to the base (max resolution) image
  // Dimensions:  Planes x ni x nj.
  inline unsigned int ni() const override
  {
    if (levels_.size()>0)
      return levels_[0]->image_->ni();
    else
      return 0;
  }
  //: The number of pixels in each column.
  // This method refers to the base (max resolution) image
  // Dimensions:  Planes x ni x nj.
  inline unsigned int nj() const override
  {
    if (levels_.size()>0)
      return levels_[0]->image_->nj();
    else
      return 0;
  }

  //: Pixel Format.

  inline enum vil_pixel_format pixel_format() const override
  {
    if (levels_.size()>0)
      return levels_[0]->image_->pixel_format();
    else
      return VIL_PIXEL_FORMAT_UNKNOWN;
  }

  //: Create a read/write view of a copy of this data.
  // Applies only to the base image
  inline vil_image_view_base_sptr get_copy_view(unsigned int i0,
                                                        unsigned int n_i,
                                                        unsigned int j0,
                                                        unsigned int n_j) const override
  {
    if (levels_.size()>0)
      return levels_[0]->image_->get_copy_view(i0, n_i, j0, n_j);
    else
      return nullptr;
  }

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  char const* file_format() const override { return "pryl"; }

        // --- Methods particular to pyramid resource ---

  //: number of levels in the pyramid
  unsigned int nlevels() const override {return (unsigned int)(levels_.size());}

  //:Copy an image resource to the pyramid.
  // If an image of the same scale already exists, then method returns false.
  bool put_resource(vil_image_resource_sptr const& image) override;

  //:Add an image resource directly to the pyramid without copying.
  // If an image of the same scale already exists, then method returns false.
  bool add_resource(vil_image_resource_sptr const& image);

  //: virtual method for getting a level of the pyramid
  vil_image_resource_sptr get_resource(const unsigned int level) const override
    {return get_level(level);}

  //: Get a level image resource of the pyramid
  inline vil_image_resource_sptr get_level(const unsigned int level) const
  { if (level<levels_.size()) return levels_[level]->image_; else return nullptr; }

  //:Get a partial view from the image from a specified pyramid level
  vil_image_view_base_sptr get_copy_view(unsigned int i0, unsigned int n_i,
                                                 unsigned int j0, unsigned int n_j,
                                                 unsigned int level) const override;

  //:Get a view from the image in the pyramid closest to scale
  vil_image_view_base_sptr get_copy_view(const float scale, float& actual_scale) const override
  { return get_copy_view(0, ni(), 0, nj(), scale, actual_scale); }

  //:Get a partial view from the image in the pyramid closest to scale.
  // The origin and size parameters are in the coordinate system of the base image.
  vil_image_view_base_sptr get_copy_view(unsigned int i0, unsigned int n_i,
                                         unsigned int j0, unsigned int n_j,
                                         const float scale,
                                         float& actual_scale) const override;

  void set_directory(char const* directory) { directory_ = directory; }

  //for debugging purposes
  void print(const unsigned int level) override
  { if (level<levels_.size()) levels_[level]->print(level); }

 protected:
  // no default constructor;
  vil_pyramid_image_list();

           //    --- utility functions ---

  //:normalize the scale factors so that the base image scale = 1.0
  void normalize_scales();

  //:find the image resource with scale closest to specified scale
  pyramid_level* closest(const float scale) const;

  //:input image is the same size as one already in the pyramid
  bool is_same_size(vil_image_resource_sptr const& image);

  //:find the nearest level to the image size
  float find_next_level(vil_image_resource_sptr const& image);

          //    ---  members ---

  std::string directory_;

  //The set of images in the pyramid. levels_[0] is the base image
  std::vector<pyramid_level*> levels_;
};

#endif // vil_pyramid_image_list_h_
