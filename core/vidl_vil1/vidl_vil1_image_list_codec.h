#ifndef vidl_vil1_image_list_codec_h
#define vidl_vil1_image_list_codec_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   Julien ESTEVE, June 2000 -  Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vidl_vil1/vidl_vil1_image_list_codec_sptr.h>
#include <vidl_vil1/vidl_vil1_codec.h>
#include <vil1/vil1_image.h>
#include <vcl_list.h>
#include <vcl_vector.h>

//: Coder/decoder for a list of images
//   A vidl_vil1_image_list_codec is the video I/O access for list of images.
class vidl_vil1_image_list_codec :  public vidl_vil1_codec
{
  // PUBLIC INTERFACE----------------------------------------------------------
 public:

  // Constructors
  vidl_vil1_image_list_codec();
  vidl_vil1_image_list_codec(vcl_list<vil1_image>& images);
  vidl_vil1_image_list_codec(vcl_vector<vil1_image>& images);
  // Destructor
  ~vidl_vil1_image_list_codec();

  // Operators-----------------------------------------------------------------
  // Safe cast to a parent from Image
  virtual vidl_vil1_image_list_codec* casttovidl_vil1_image_list_codec() { return this; }

  virtual bool get_section(int position, void* ib, int x0, int y0, int w, int h) const;
  virtual int put_section(int position, void* ib, int x0, int y0, int w, int h);
  // IO
  virtual vidl_vil1_codec_sptr load(const char* fname, char mode = 'r' );
  virtual vidl_vil1_codec_sptr load(const vcl_list<vcl_string> &fnames, char mode = 'r');
  virtual vidl_vil1_codec_sptr load(const vcl_vector<vcl_string> &fnames, char mode = 'r');
  virtual bool save(vidl_vil1_movie* movie, const char* fname);
  virtual bool save(vidl_vil1_movie* movie, const char* fname, const char* type);
  virtual bool probe(const char* fname);

  virtual const char* type() {return "ImageList";}

  // Specific to vidl_vil1_image_list_codec
  static void set_default_image_type(const char* type)
        {vcl_strcpy(default_initialization_image_type_,type);}

  virtual void set_image_type(const char* type)
        {vcl_strcpy(default_image_type_,type);}

  virtual const char* get_image_type() const {return default_image_type_;}
  vcl_vector<vil1_image> get_images() const {return images_;}

  // Register image loaders
  //virtual void register_image_loaders ();

 protected:
  virtual bool init();

  // Data Members--------------------------------------------------------------
  vcl_vector<vil1_image> images_;

 private:
  char* default_image_type_;
  static char* default_initialization_image_type_;
};

#endif // vidl_vil1_image_list_codec_h
