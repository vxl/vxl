// This is vxl/vil/file_formats/vil_gif.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_gif.h"

#include <vcl_cassert.h>
#include <vcl_iomanip.h> // for vcl_hex, vcl_dec
#include <vcl_iostream.h>
#include <vcl_cstring.h>
#include <vcl_cctype.h> // for vcl_isprint

#include <vil/vil_stream.h>
#include <vil/vil_16bit.h>
#include <vil/vil_property.h>

#ifndef VERBOSE // set this to 1 on the compile line if you want verbose output
#define VERBOSE 0
#endif

bool vil_gif_probe(vil_stream *s)
{
  // 47 49 46 38 37 61  "GIF87a"
  s->seek(0L);
  char magic[6];
  s->read(magic, sizeof magic);

  if (magic[0] != 0x47 ||
      magic[1] != 0x49 ||
      magic[2] != 0x46 )
    return false;

  if (magic[3] != 0x38 ||
      magic[4] != 0x37 ||
      magic[5] != 0x61 ) {
    vcl_cerr << __FILE__ ": file format may be GIF, but is not v87\n";
    // may be GIF, but not GIF87a
    return false;
  }
  return true;
}

char const *vil_gif_file_format::tag() const { return "gif"; }

vil_image_impl *vil_gif_file_format::make_input_image(vil_stream *s)
{
  if (! vil_gif_probe(s))
    return 0;
  else
    return new vil_gif_loader_saver(s);
}

vil_image_impl *vil_gif_file_format::make_output_image(vil_stream*, int, int, int, int, int, vil_component_format)
{
  vcl_cerr << "GIF writer not yet implemented\n";
  return 0;
}

bool vil_gif_loader_saver::get_property(char const *tag, void *prop) const
{
  if (0==vcl_strcmp(tag, vil_property_top_row_first))
    return prop ? (*(bool*)prop) = true : true;

  if (0==vcl_strcmp(tag, vil_property_left_first))
    return prop ? (*(bool*)prop) = true : true;

  return false;
}

vil_gif_loader_saver::vil_gif_loader_saver(vil_stream *s_)
 : s(s_), is_grey(false)
{
  s->ref();
  assert(vil_gif_probe(s));

  s->seek(6L);

  // read screen descriptor
  screen_width_  = vil_16bit_read_little_endian(s);
  screen_height_ = vil_16bit_read_little_endian(s);
#if VERBOSE
  vcl_cerr << "screen width and height : "
           << screen_width_ << ' ' << screen_height_ << vcl_endl;
#endif

  unsigned char b;

  // ---------- read screen descriptor

  {
    s->read(&b, 1L);
#if VERBOSE
    vcl_cerr << "b = 0x" << vcl_hex << int(b) << vcl_dec << vcl_endl;
#endif

    int bits_of_colour_res;
    if (b & 0x80) {
      bits_of_colour_res = 1 + ((b & 0x70)>>4);
#if VERBOSE
      vcl_cerr << "screen has global colour map\n"
               << "screen has " << bits_of_colour_res << " bits of colour resolution\n";
#endif
    }
    else
      bits_of_colour_res = 0;

    // bit 3 should be zero
    if (b & 0x08) {
      vcl_cerr << "bit 3 is not zero\n";
      assert(!"bit 3 should be zero");
    }

    int bits_per_pixel = 1 + (b & 0x07);
#if VERBOSE
    vcl_cerr << "screen has " << bits_per_pixel << " bits per pixel\n";
#endif
    if (bits_per_pixel != 8) {
      vcl_cerr << "cannot read GIF with != 8 bits per pixel.\n";
      assert(!"GIF with != 8 bits per pixel not implemented");
    }

    // create global colour map, if needed.
    if (bits_of_colour_res > 0) {
      global_color_map = new vil_gif_color_map( 0x1 << bits_per_pixel );
#if VERBOSE
      vcl_cerr << "global colour map has size " << global_color_map->size << vcl_endl;
#endif
    }

    // colour index of background.
    s->read(&b, 1L);
    background_index = b;
#if VERBOSE
    vcl_cerr << "background has colour index " << background_index << vcl_endl;
#endif

    // should be zero
    s->read(&b, 1L);
    if (b) {
      vcl_cerr << "not zero\n";
      assert(!"this byte should be zero");
    }
  }

  // ---------- read global colourmap

#if VERBOSE
  vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;
#endif
  if (global_color_map) {
#if VERBOSE
    vcl_cerr << "read global colour map\n";
#endif
    s->read(global_color_map->cmap, 3*global_color_map->size);

    is_grey = true;
    for (int i=0; is_grey && i<global_color_map->size; ++i)
       if (global_color_map->cmap[3*i+0] !=
           global_color_map->cmap[3*i+1] ||
           global_color_map->cmap[3*i+1] !=
           global_color_map->cmap[3*i+2])
         is_grey = false;

#if VERBOSE
    for (int i=0; i<16; ++i)
      vcl_cerr << vcl_setw(3) << i << ' '
           << int((unsigned char) global_color_map->cmap[3*i+0]) << ' '
           << int((unsigned char) global_color_map->cmap[3*i+1]) << ' '
           << int((unsigned char) global_color_map->cmap[3*i+2]) << vcl_endl;
#endif
  }

  // ---------- read image descriptors

  while (true) {
    vil_streampos offset = s->tell();
#if VERBOSE
    vcl_cerr << "position is 0x" << vcl_hex << offset << vcl_dec << vcl_endl;
#endif

    // read image separator or GIF terminator
    s->read(&b, 1L);
    if (b == ';')   // terminator
      break;
    if (b != ',') { // separator
      vcl_cerr << "unexpected character \'";
      if (vcl_isprint(b)) {
        vcl_cerr << b;
      } else {
        vcl_cerr << "[unprintable]";
      }
      vcl_cerr<< "\' (0x" << vcl_hex<< int(b) << vcl_dec<< ") in GIF stream\n";
      assert(!"expected GIF separator here");
    }

    vil_gif_image_record *ir = new vil_gif_image_record;
    ir->offset = offset;

    ir->x0 = vil_16bit_read_little_endian(s);
    ir->y0 = vil_16bit_read_little_endian(s);
    ir->w  = vil_16bit_read_little_endian(s);
    ir->h  = vil_16bit_read_little_endian(s);
#if VERBOSE
    vcl_cerr << "x0 y0 w h = " << ir->x0 << ' ' << ir->y0 << ' '
             << ir->w << ' ' << ir->h << vcl_endl;

    vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;
#endif

    s->read(&b, 1L);
#if VERBOSE
    vcl_cerr << "b = 0x" << vcl_hex << int(b) << vcl_dec << vcl_endl;

    vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;
#endif
    if (b & 0x80) { // local colour map?
      int bits = 1 + (b & 0x07);
#if VERBOSE
      vcl_cerr << "image has local colour map\n"
               << "read local colour map (" << bits << " bits per pixel)\n";
#endif
      ir->color_map = new vil_gif_color_map(0x1 << bits);
      s->read(ir->color_map->cmap, 3*ir->color_map->size);
    }
    else {
#if VERBOSE
      vcl_cerr << "no local colour map\n";
#endif
      ir->color_map = 0;
    }
#if VERBOSE
    vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;
#endif

    // interlaced or sequential?
    ir->interlaced = ( (b & 0x40) != 0 );
#if VERBOSE
    vcl_cerr<< "image is "<< (ir->interlaced ? "interlaced\n" : "sequential\n");
#endif
    if (ir->interlaced) {
      vcl_cerr << "can't read interlaced GIFs yet\n";
      assert(!"interlaced GIF reading not implemented");
    }

    // bits 543 should be zero
    if (b & 0x38) {
      vcl_cerr << "bits 543 are not zero\n";
      assert(!"incorrect bits 3,4,5");
    }

    //
    if (ir->color_map) {
      ir->bits_per_pixel = 1 + (b & 0x07);
#if VERBOSE
      vcl_cerr << "image has " << ir->bits_per_pixel << " bits per pixel\n";
#endif
      if (ir->bits_per_pixel != 8) {
        vcl_cerr << "cannot cope with " << ir->bits_per_pixel << " bits per pixel\n";
        assert(!"bpp != 8 not implemented");
      }
    }
    else
      ir->bits_per_pixel = 0;

    //
    ir->bitmap_start = s->tell();

    // seek to end of raster data
    s->seek(ir->bitmap_start + ir->w * ir->h);

    images.push_back(ir);
    break;
  }

#if VERBOSE
  vcl_cerr << "read " << images.size() << " image descriptors\n"
           << "------------ done : position = "
           << vcl_hex << s->tell() << vcl_dec << vcl_endl;
#endif
}

vil_gif_loader_saver::~vil_gif_loader_saver()
{
  s->unref();

  if (global_color_map) {
    delete global_color_map;
    global_color_map = 0;
  }

  for (unsigned int i=0; i<images.size(); ++i) {
    vil_gif_image_record *ir = static_cast<vil_gif_image_record*>(images[i]);
    if (ir->color_map)
      delete ir->color_map;
    delete ir;
  }
  images.clear();
}

char const *vil_gif_loader_saver::file_format() const { return "gif"; }

vil_image vil_gif_loader_saver::get_plane(int i) const
{
  if (0<=i && i<int(images.size()))
    return new vil_gif_loader_saver_proxy(i, const_cast<vil_gif_loader_saver*>(this));
  else
    return 0;
}

bool vil_gif_loader_saver::get_section(void *buf, int x0, int y0, int w, int h) const
{
  if (planes() == 1)
    return get_section(0, buf, x0, y0, w, h);
  else
    return false;
}

bool vil_gif_loader_saver::put_section(void const *buf, int x0, int y0, int w, int h)
{
  if (planes() == 1)
    return put_section(0, buf, x0, y0, w, h);
  else
    return false;
}

bool vil_gif_loader_saver::get_section(int image, void* buf, int x0, int y0, int w, int h) const
{
#if 1
  // Damn! Have to implement LZW decompression here. Maybe some other day.
  vcl_cerr << "vil_gif_loader_saver::get_section(): LZW decompression not yet implemented\n";
#else
  assert(0<=image && image<images.size());
  char *char_buf = (char*) buf;

  vil_gif_image_record *ir = static_cast<vil_gif_image_record*>(images[image]);

#if 0
  for (int i=0; i<h; ++i) {
    s->seek(ir->bitmap_start + x0 + ir->w*(y0 + i));
    s->read(char_buf + w*i, w);
  }
#else
  unsigned char *tmp = new unsigned char [w];

  vil_gif_color_map *cm = ir->color_map ? ir->color_map : global_color_map;

  for (int i=0; i<h; ++i) {
    s->seek(ir->bitmap_start + x0 + ir->w*(y0 + i));
    s->read(tmp, w);
    for (int j=0; j<w; ++j) {
      int index = int(tmp[j]);
      (char_buf + 3*w*i)[3*j + 0] = cm->cmap[3*index + 0];
      (char_buf + 3*w*i)[3*j + 1] = cm->cmap[3*index + 1];
      (char_buf + 3*w*i)[3*j + 2] = cm->cmap[3*index + 2];
    }
  }

  delete [] tmp;
#endif
#endif
  return true;
}
