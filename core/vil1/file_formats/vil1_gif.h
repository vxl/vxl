// This is core/vil1/file_formats/vil1_gif.h
#ifndef vil1_gif_h_
#define vil1_gif_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  fsm
//
// This is an ad hoc hack. If you write a better one, you are most
// welcome to replace mine with yours.
//
//\verbatim
//  Modifications
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

class vil1_stream;
#include <vcl_vector.h>
#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>

bool vil1_gif_probe(vil1_stream *s);

struct vil1_gif_file_format : public vil1_file_format
{
  virtual char const* tag() const;
  virtual vil1_image_impl *make_input_image(vil1_stream* vs);
  virtual vil1_image_impl *make_output_image(vil1_stream*, int, int, int, int, int, vil1_component_format);
};

struct vil1_gif_color_map
{
  int size;
  char *cmap;
  vil1_gif_color_map(int sz) : size(sz), cmap(new char [3*size]) { }
  ~vil1_gif_color_map() { delete [] cmap; cmap = 0; }
};

struct vil1_gif_image_record
{
  int offset;
  int x0, y0;
  int w, h;
  vil1_gif_color_map *color_map;
  bool interlaced;
  int bits_per_pixel;
  int bitmap_start;
};

struct vil1_gif_loader_saver : public vil1_image_impl
{
  vil1_gif_loader_saver(vil1_stream *);
#if 0
  vil1_gif_loader_saver(vil1_stream *is,
                        int planes,
                        int width,
                        int height,
                        int components,
                        int bits_per_component,
                        vil1_component_format format);
#endif
  ~vil1_gif_loader_saver();

  int planes() const { return images.size(); }
  int width() const { return screen_width_; }
  int height() const { return screen_height_; }
  int components() const { return is_grey ? 1 : 3; }
  int bits_per_component() const { return 8; }
  enum vil1_component_format component_format() const { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }

  vil1_image get_plane(unsigned int p) const;
  bool get_section(void *buf, int x0, int y0, int width, int height) const;
  bool get_section(int image, void *buf, int x0, int y0, int width, int height) const;
  bool put_section(void const *, int, int, int, int);
  bool put_section(int, void const *, int, int, int, int) { return false; }

  char const *file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

 private:
  vil1_stream *s;
  int screen_width_;
  int screen_height_;
  bool is_grey; // set to true if all entries in the colour map are grey

  vil1_gif_color_map *global_color_map;
  int background_index;

  vcl_vector<void*> images;
};

struct vil1_gif_loader_saver_proxy : public vil1_image_impl
{
  int image;
  vil1_gif_loader_saver *other;
  vil1_image up_ref_is_private;

  vil1_gif_loader_saver_proxy(int image_, vil1_gif_loader_saver *other_)
    : image(image_), other(other_) {
    // why doesn't this work?
    //up_ref_is_private(other);
  }

  ~vil1_gif_loader_saver_proxy() { }

  int planes() const { return 1; }
  int width() const { return other->width(); }
  int height() const { return other->height(); }
  int components() const { return other->components(); }
  int bits_per_component() const { return other->bits_per_component(); }

  enum vil1_component_format component_format() const { return other->component_format(); }

  bool get_section(void *buf, int x0, int y0, int w, int h) const
  { return other->get_section(image, buf, x0, y0, w, h); }

  bool put_section(void const *buf, int x0, int y0, int w, int h)
  { return other->put_section(image, buf, x0, y0, w, h); }

  char const *file_format() const { return other->file_format(); }
};

#endif // vil1_gif_h_
