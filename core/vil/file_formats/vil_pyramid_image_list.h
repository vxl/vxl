#ifndef vil_pyramid_image_list_h_
#define vil_pyramid_image_list_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A pyramid image resource based on multiple file-based image resources
// \author J.L. Mundy March 20, 2006

#include <vcl_string.h>
#include <vil/vil_file_format.h>
#include <vil/vil_pyramid_image_resource.h>
//The pyramid resource is made up of a set of image resources in 
// a single directory - the image list. 
class vil_pyramid_image_list_format : public vil_file_format
{
 public:
  ~vil_pyramid_image_list_format(){}

  //: Return a character string which uniquely identifies this format.
  //E.g. "pnm", "jpeg", etc.
  virtual char const* tag() const {return "pyil";}//pyramid image list
  //                                                ---           -
  //:should return 0 so that no attempt is made to create a 
  // single image resource
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs)
    {return 0;}

  //: Read a pyramid resource. Image list files are stored in directory. 
  virtual vil_pyramid_image_resource_sptr 
    make_input_pyramid_image(char const* directory);


  //: Construct a pyramid image resource from a base image
  //  Each level has the same scale ratio (0.5) to the preceeding level. 
  //  Level 0 is the original base image. If copy base is false then
  //  Level 0 is already present in the directory and is used without 
  //  modification. Each pyramid file in the directory is named 
  //  filename + "level_index", e.g. R0, R1, ... Rn.
virtual vil_pyramid_image_resource_sptr 
  make_pyramid_image_from_base(char const* directory,
                               vil_image_resource_sptr const& base_image,
                               unsigned nlevels,
                               bool copy_base,
                               char const* level_file_format,
                               char const* filename
                               );

  //: A non-pyramid output image doesn't make sense here
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format)
    {return 0;}

  virtual vil_pyramid_image_resource_sptr
    make_pyramid_output_image(char const* directory);


};

struct pyramid_level
{
  pyramid_level(vil_image_resource_sptr const& image):
    scale_(1.0f), image_(image), cur_level_(0)
  {}
  //:scale associated with level
  float scale_;

  //:the resource
  vil_image_resource_sptr image_;

  //:the currel pyramid level for this resource
  unsigned cur_level_;

  //:print ni and scale and values
  void print(const unsigned l){vcl_cout << "level[" << l <<  "]  scale: " << scale_ 
                        << "  ni: " << image_->ni() << '\n';}

};


class vil_pyramid_image_list : public vil_pyramid_image_resource

{
 public:
  vil_pyramid_image_list(char const* directory);
  vil_pyramid_image_list(vcl_vector<vil_image_resource_sptr> const& images);
  virtual ~vil_pyramid_image_list();
  //: The following methods refer to the base (max resolution) image 
  //: Dimensions:  Planes x ni x nj.
  // This concept is treated as a synonym to components.
  inline virtual unsigned nplanes() const
    {if(levels_.size()>0)return levels_[0]->image_->nplanes(); else return 0;}
  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each row.
  inline virtual unsigned ni() const
    {if(levels_.size()>0)return levels_[0]->image_->ni(); else return 0;}

  //: Dimensions:  Planes x ni x nj.
  // The number of pixels in each column.
  inline virtual unsigned nj() const
    {if(levels_.size()>0)return levels_[0]->image_->nj(); else return 0;}
  //: Pixel Format.

  inline virtual enum vil_pixel_format pixel_format() const
    {if(levels_.size()>0)return levels_[0]->image_->pixel_format(); else return VIL_PIXEL_FORMAT_UNKNOWN;}

  //: Create a read/write view of a copy of this data.
  // applies only to the base image
  inline virtual vil_image_view_base_sptr get_copy_view(unsigned i0,
                                                        unsigned n_i,
                                                        unsigned j0,
                                                        unsigned n_j) const
    {if(levels_.size()>0) return levels_[0]->image_->get_copy_view(i0, n_i, j0, n_j); else return 0;}
      
  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return "pryl"; }

  //:Methods particular to pyramid resource

  //: number of levels in the pyramid
  virtual unsigned nlevels() const {return levels_.size();}

  //:Copy an image resource to the pyramid. If an image of the same 
  // scale already exists, then method returns false.
  bool put_resource(vil_image_resource_sptr const& image);
 
//:Add an image resource directly to the pyramid without copying. If an image of the same scale already exists, then method returns false.
  bool add_resource(vil_image_resource_sptr const& image);
  
  //: Get a level image resource of the pyramid
  inline vil_image_resource_sptr get_level(const unsigned level) const
    {if(level<levels_.size()) return levels_[level]->image_; else return 0;}

  //:Get a partial view from the image from a specified pyramid level
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                                 unsigned j0, unsigned n_j,
                                                 unsigned level) const;

      

  //:Get a view from the image in the pyramid closest to scale
  vil_image_view_base_sptr get_copy_view(const float scale, float& actual_scale) const
    {return get_copy_view(0, ni(), 0, nj(), scale, actual_scale);}

  //:Get a partial view from the image in the pyramid closest to scale. The origin and size parameters are in the coordinate system of the base image.
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned n_i,
                                    unsigned j0, unsigned n_j,
                                    const float scale,
                                    float& actual_scale) const;

  void set_directory(char const* directory)
    {directory_ = directory;}

  //for debugging purposes
  void print(const unsigned level)
    {if(level<levels_.size()) levels_[level]->print(level);}

 protected:
  // no default constructor;
  vil_pyramid_image_list();
  //utility functions

  //:normalize the scale factors so that the base image scale = 1.0
  void normalize_scales();

  //:find the image resource with scale closest to specified scale
  pyramid_level* closest(const float scale) const;

  //:input image is the same size as one already in the pyramid
  bool is_same_size(vil_image_resource_sptr const& image);

  //:find the nearest level to the image size
  float find_next_level(vil_image_resource_sptr const& image);

  //members
  vcl_string directory_;

  //The set of images in the pyramid. levels_[0] is the base image
  vcl_vector<pyramid_level*> levels_;
};

#endif // vil_pyramid_image_list_h_
