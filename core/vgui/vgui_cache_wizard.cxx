// This is core/vgui/vgui_cache_wizard.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief See vgui_cache_wizard.h for a description of this file.

#include "vgui_cache_wizard.h"

#include <vil1/vil1_crop.h>
#include <vil1/vil1_pixel.h>

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vgui/vgui_pixel.h>
#include <vgui/vgui_macro.h>
#include <vgui/internals/vgui_accelerate.h>

#define DEFAULT_QUADRANT_WIDTH  256
#define DEFAULT_QUADRANT_HEIGHT 256
#define INVALID_TEXTURE_NAME -1

#define mb_jigerry_pokery(a,b) ((a/b)+(a%b ? 1:0))
#define mb_is_valid(x) (int(x) != INVALID_TEXTURE_NAME)

bool debug = false;

//: Constructor.
// Set the image quadrant width/height.
// Do use sensible values(i.e. powers of two) otherwise the cache
// wizard won't work.
vgui_cache_wizard::vgui_cache_wizard(int quadrant_width,
                                     int quadrant_height)
{
  if (debug)
    vcl_cerr << __FILE__": this is the constructor\n";
    // *(int*)0 = 1;

  //: Get the maximum texture size.
  // Note that this function returns the worst case scenario
  // i.e. assumes the texture is 4bpp... So we are on the safe ground
  GLint max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  if (max_texture_size>256)
    max_texture_size = 256;

  if (quadrant_width<max_texture_size)
    quadrant_width_ = quadrant_width;
  else
    quadrant_width_ = max_texture_size;
  if (quadrant_height<max_texture_size)
    quadrant_height_ = quadrant_height;
  else
    quadrant_height_ = max_texture_size;
  // Assume 8M for now
  max_texture_num_ = (unsigned int)(8388608.0f/(quadrant_width_*quadrant_height_*4.0f))*3;
  // Generate the names for those textures
  texture_names_ = new GLuint[max_texture_num_];
  glGenTextures(max_texture_num_,texture_names_);
}

//: Destructor
vgui_cache_wizard::~vgui_cache_wizard()
{
  vcl_list<GLuint>::iterator i = cache_queue_.begin();
  for (int j=0; i!=cache_queue_.end(); ++i,++j)
    texture_names_[j] = *i;
  glDeleteTextures(cache_queue_.size(),texture_names_);
  delete texture_names_;
}

vgui_cache_wizard *vgui_cache_wizard::Instance()
{
  static vgui_cache_wizard *instance_ = new vgui_cache_wizard(DEFAULT_QUADRANT_WIDTH, DEFAULT_QUADRANT_HEIGHT);
  return instance_;
}

//: Loads an image
int vgui_cache_wizard::load_image(vil1_image img)
{
  // Check whether the image pointer is already in memory
  vcl_vector<wizard_image *>::iterator i = images_.begin();
  for (int j = 0; i!=images_.end(); ++i,++j)
    if ((*i)->first == img)
      return j;

  // Find out the size of image in quadrants
  int cx = mb_jigerry_pokery(img.width(),quadrant_width_);
  int cy = mb_jigerry_pokery(img.height(),quadrant_height_);

  dimension *dim = new dimension(cx,cy);
  dimensions_.push_back(dim);
  // Initialize
  image_cache_quadrants *mapping = new image_cache_quadrants;
  for (int i = 0;i<cy;i++)
    for (int j = 0;j<cx;j++)
      mapping->push_back(unsigned(INVALID_TEXTURE_NAME));

  wizard_image *wz = new wizard_image(img,mapping);
  images_.push_back(wz);
  return images_.size()-1;
}

//: Returns the texture names associated with the specified image region
bool vgui_cache_wizard::get_section(int id,int x,int y,int width,int height,
                                    image_cache_quadrants *quadrants,
                                    dimension *pos,
                                    dimension *size)
{
  // Find out which image it is
  wizard_image *wz = images_[id];
  vil1_image img = wz->first;
  image_cache_quadrants *icq = wz->second;
  dimension *d = dimensions_[id];
  // Work out which quadrant (x,y) lies in
  int qx_c = x/quadrant_width_;
  int qy_c = y/quadrant_height_;
  // Work out section width and height in quadrant units
  int qw_c = (x+width)/quadrant_width_-qx_c;//mb_jigerry_pokery(x+width,quadrant_width_);
  int qh_c = (y+height)/quadrant_height_-qy_c;//mb_jigerry_pokery(y+height,quadrant_height_);
  if (debug)
    vcl_cerr<<"X: "<<qx_c<<"Y: "<<qy_c<<"W:"<<qw_c<<" H:"<<qh_c<<vcl_endl;
  pos->first = qx_c;
  pos->second = qy_c;
  size->first = qw_c;
  size->second = qh_c;

  // This is the index of the upper left quadrant of the section
  int ul_q = qy_c*d->first+qx_c;
  glEnable(GL_TEXTURE_2D);
  vgui_macro_report_errors;

  for (int i = 0;i<=qh_c;i++)
    for (int j = 0;j<=qw_c;j++)
    {
      // (i,j) relative to the upper left corner of the section
      // needs to be translated into image coordinates
      int index = ul_q+i*d->first+j;
      // Check to see whether the quadrant is in cache
      if (index<0 || index>=int(icq->size()))
      {
        vcl_cerr << __FILE__ ": index out of range\n";
        return false;
      }
      if (mb_is_valid((*icq)[index]))
      {
        quadrants->push_back((*icq)[index]);
        vcl_list<GLuint>::iterator i;
        for (i = cache_queue_.begin();i!=cache_queue_.end() && (*i)!=(*icq)[index]; i++)
          ;
        cache_queue_.erase(i);
        cache_queue_.push_back((*icq)[index]);
      }
      else
      {
        // If the quadrant is not in cache load it by dumping least recently used
        // texture block if necessary
        GLuint texture_name;
        if (cache_queue_.size() == max_texture_num_)
        {
          // Time to dump LRU texture and use it for the quadrant of this image section
          texture_name = cache_queue_.front();
          vcl_cerr<<"Texture name: "<<texture_name<<vcl_endl;
          cache_queue_.pop_front();
          // Find the image where texture_name has been used and invalidate
          // that qudrangle
          for (vcl_vector<wizard_image *>::iterator i = images_.begin(); i!=images_.end();i++)
            for (image_cache_quadrants::iterator k = (*i)->second->begin(); k!=(*i)->second->end();k++)
              if ((*k) == texture_name)
              {
                *k = GLuint(INVALID_TEXTURE_NAME);
                vcl_cerr<<"Invalidated!\n";
              }
        }
        else
        {
          unsigned int k = 0;
          while (k<max_texture_num_ && texture_names_[k] == -1u) ++k;
          texture_name = texture_names_[k];
          texture_names_[k] = GLuint(-1);
        }

        glBindTexture(GL_TEXTURE_2D,texture_name);
        TexImage2D_Brownie(vil1_crop(img,pos->first*quadrant_width_+j*quadrant_width_,
                                    pos->second*quadrant_height_+i*quadrant_height_,
                                    quadrant_width_,
                                    quadrant_height_));
        quadrants->push_back(texture_name);
        cache_queue_.push_back(texture_name);
        // Validate the appropriate quadrangle
        (*icq)[index] = texture_name;
      }
    }
  return true;
}

//--------------------------------------------------------------------------------
//
// pixel format conversions

// 'pix' is the pixel type supplied by the image.
// 'wh'  is a string describing the pixel type.
// uses: 'section_ok', 'image', 'data', 'x', 'y', 'w', 'h', 'allocw', 'alloch'
#define fsm_macro_begin(pix, wh) \
pix *data = new pix[ img.width()*img.height() ]; /* note: this buffer is w-by-h, not allocw-by-alloch */ \
section_ok = img.get_section( data, 0,0, img.width(),img.height() ); \
char const *what = wh; \
/* bool found = false; */ /* used to allow semicolons after macro calls */ \
if (false) {}

// 'fmt' is the GLenum format passed to glDrawPixels()
// 'typ' is the GLenum type passed to glDrawPixels()
// 'sto' is the pixel type to store the section as. it must agree with 'fmt' and 'typ'.
// uses: 'what', 'data', 'the_pixels', 'w', 'h'
#define fsm_macro_magic(fmt, typ, sto) \
if (format==fmt && type==typ) { \
  if (debug) \
    vcl_cerr << __FILE__ ": converting " << what << " image to " #fmt "," #typ " format\n"; \
  if (!the_pixels) \
    the_pixels = new sto[img.width()*img.height()]; \
  vgui_pixel_convert_span(data, static_cast<sto*>(the_pixels), \
                          img.width()*img.height()); \
}

// you *must* call this -- to deallocate the temp buffer.
#define fsm_macro_end \
delete [] data; \
assert(section_ok);

void vgui_cache_wizard::TexImage2D_Brownie(vil1_image img)
{
  void *the_pixels = 0;
  bool section_ok;
  // FIXME: the calls to fsm_macro_magic() are identical for each image pixel type.
  // They could be coalesced to reduce code maintenance.

  // 8bit greyscale
  GLenum format,type;
  vgui_accelerate::instance()->vgui_choose_cache_format(&format,&type);

  vil1_pixel_format_t pixel_format = vil1_pixel_format(img);

  if (pixel_format == VIL1_BYTE)
  {
    fsm_macro_begin(GLubyte, "8 bit greyscale");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // 24bit rgb
  else if (pixel_format == VIL1_RGB_BYTE)
  {
    fsm_macro_begin(vgui_pixel_rgb888, "24 bit RGB");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  // 32bit rgba
  else if (pixel_format == VIL1_RGBA_BYTE)
  {
    fsm_macro_begin(vgui_pixel_rgba8888, "32 bit RGBA");
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_BYTE,        vgui_pixel_rgb888);
    fsm_macro_magic(GL_RGBA,     GL_UNSIGNED_BYTE,        vgui_pixel_rgba8888);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
    fsm_macro_magic(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, vgui_pixel_rgb565);
#endif
#if defined(GL_BGRA)
    fsm_macro_magic(GL_BGRA,     GL_UNSIGNED_BYTE,        vgui_pixel_bgra8888);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
    fsm_macro_magic(GL_ABGR_EXT, GL_UNSIGNED_BYTE,        vgui_pixel_abgr8888);
#endif
    fsm_macro_end;
  }

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  vgui_macro_report_errors;

  // decal
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  vgui_macro_report_errors;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (debug) vcl_cerr<<"Loading texture...";
  glTexImage2D(GL_TEXTURE_2D, // target
               0,             // level
               3,             // internalformat (use only RGB. ignore alpha channel)
               img.width(),   // NB: must be power of 2
               img.height(),  // NB: must be power of 2
               0,             // border FIXME
               format,
               type,
               the_pixels);
  vgui_macro_report_errors;
}
