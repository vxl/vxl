// This is core/vidl/vidl_image_list_codec.h
#ifndef vidl_image_list_codec_h
#define vidl_image_list_codec_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   Julien ESTEVE, June 2000 -  Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   10/7/2003 Matt Leotta (Brown) Converted vil1 to vil
// \endverbatim

#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vidl/vidl_image_list_codec_sptr.h>
#include <vidl/vidl_codec.h>
#include <vil/vil_image_resource.h>
#include <vcl_list.h>
#include <vcl_vector.h>

//: Coder/decoder for a list of images
//   A vidl_image_list_codec is the video I/O access for list of images.
class vidl_image_list_codec :  public vidl_codec
{
  // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors
  vidl_image_list_codec();
  vidl_image_list_codec(vcl_list<vil_image_resource_sptr>& images);
  vidl_image_list_codec(vcl_vector<vil_image_resource_sptr>& images);
  // Destructor
  ~vidl_image_list_codec();

  // Operators-----------------------------------------------------------------
  // Safe cast to a parent from Image
  virtual vidl_image_list_codec* casttovidl_image_list_codec() { return this; }

  //: Return the resource to the image
  virtual vil_image_resource_sptr get_resource(int position) const;
  virtual vil_image_view_base_sptr get_view(int position, int x0, int w, int y0, int h) const;
  virtual bool put_view(int position, const vil_image_view_base &im, int x0, int y0);
  // IO
  virtual vidl_codec_sptr load(vcl_string const& fname, char mode = 'r' );
  virtual vidl_codec_sptr load(const vcl_list<vcl_string> &fnames, char mode = 'r');
  virtual vidl_codec_sptr load(const vcl_vector<vcl_string> &fnames, char mode = 'r');
  virtual bool save(vidl_movie* movie, vcl_string const& fname);
  virtual bool save(vidl_movie* movie, vcl_string const& fname, vcl_string const& type);
  virtual bool probe(vcl_string const& fname);

  virtual vcl_string type() const { return "ImageList"; }

  // Specific to vidl_image_list_codec
  static void set_default_image_type(vcl_string const& type)
  { default_initialization_image_type_ = type; }

  virtual void set_image_type(vcl_string const& type)
  { default_image_type_ = type; }

  virtual vcl_string const& get_image_type() const {return default_image_type_;}
  vcl_vector<vil_image_resource_sptr> get_images() const {return images_;}

  // Register image loaders
  //virtual void register_image_loaders ();

 protected:
  virtual bool init();

  // Data Members--------------------------------------------------------------
  vcl_vector<vil_image_resource_sptr> images_;

 private:
  vcl_string default_image_type_;
  static vcl_string default_initialization_image_type_;
};

#endif // vidl_image_list_codec_h
