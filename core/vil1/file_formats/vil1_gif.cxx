/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_gif.h"

#include <vcl_cassert.h>
#include <vcl_ios.h> // for vcl_hex, vcl_dec
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_cstring.h>

#include <vil/vil_stream.h>
#include <vil/vil_16bit.h>
#include <vil/vil_property.h>

bool vil_gif_probe(vil_stream *s)
{
  // 47 49 46 38 37 61  "GIF87a"
  s->seek(0);
  char magic[6];
  s->read(magic, sizeof magic);

  if (magic[0] != 0x47 ||
      magic[1] != 0x49 ||
      magic[2] != 0x46 )
    return false;

  if (magic[3] != 0x38 ||
      magic[4] != 0x37 ||
      magic[5] != 0x61 ) {
    vcl_cerr << __FILE__ ": file format may be GIF, but is not v87" << vcl_endl;
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

vil_gif_loader_saver::vil_gif_loader_saver(vil_stream *s_) : s(s_), is_grey(false)
{
  s->ref();
  assert(vil_gif_probe(s));

  s->seek(6);

  // read screen descriptor
  screen_width_  = vil_16bit_read_little_endian(s);
  screen_height_ = vil_16bit_read_little_endian(s);
  vcl_cerr << "screen width and height : " << screen_width_ << ' ' << screen_height_ << vcl_endl;

  unsigned char b;

  // ---------- read screen descriptor

  {
    s->read(&b, 1);
    vcl_cerr << "b = 0x" << vcl_hex << int(b) << vcl_dec << vcl_endl;

    int bits_of_colour_res;
    if (b & 0x80) {
      vcl_cerr << "screen has global colour map" << vcl_endl;
      bits_of_colour_res = 1 + ((b & 0x70)>>4);
      vcl_cerr << "screen has " << bits_of_colour_res << " bits of colour resolution" << vcl_endl;
    }
    else
      bits_of_colour_res = 0;

    // bit 3 should be zero
    if (b & 0x08) {
      vcl_cerr << "bit 3 is not zero" << vcl_endl;
      assert(false);
    }

    int bits_per_pixel = 1 + (b & 0x07);
    vcl_cerr << "screen has " << bits_per_pixel << " bits per pixel" << vcl_endl;
    if (bits_per_pixel != 8) {
      vcl_cerr << "cannot read GIF with != 8 bits per pixel." << vcl_endl;
      assert(false);
    }

    // create global colour map, if needed.
    if (bits_of_colour_res > 0) {
      global_color_map = new vil_gif_color_map( 0x1 << bits_per_pixel );
      vcl_cerr << "global colour map has size " << global_color_map->size << vcl_endl;
    }

    // colour index of background.
    s->read(&b, 1);
    background_index = b;
    vcl_cerr << "background has colour index " << background_index << vcl_endl;

    // should be zero
    s->read(&b, 1);
    if (b) {
      vcl_cerr << "not zero" << vcl_endl;
      assert(false);
    }
  }

  // ---------- read global colourmap

  vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;
  if (global_color_map) {
    vcl_cerr << "read global colour map" << vcl_endl;
    s->read(global_color_map->cmap, 3*global_color_map->size);

    is_grey = true;
    for (int i=0; is_grey && i<global_color_map->size; ++i)
       if (global_color_map->cmap[3*i+0] !=
           global_color_map->cmap[3*i+1] ||
           global_color_map->cmap[3*i+1] !=
           global_color_map->cmap[3*i+2])
         is_grey = false;

    for (int i=0; i<16; ++i)
      vcl_cerr << vcl_setw(3) << i << ' '
           << int((unsigned char) global_color_map->cmap[3*i+0]) << ' '
           << int((unsigned char) global_color_map->cmap[3*i+1]) << ' '
           << int((unsigned char) global_color_map->cmap[3*i+2]) << vcl_endl;
  }

  // ---------- read image descriptors

  while (true) {
    int offset = s->tell();
    vcl_cerr << "position is 0x" << vcl_hex << offset << vcl_dec << vcl_endl;

    // read image separator or GIF terminator
    s->read(&b, 1);
    if (b == ';')   // terminator
      break;
    if (b != ',') { // separator
      vcl_cerr << "unexpected character \'" << char(b) << "\' (0x" << vcl_hex
               << int(b) << vcl_dec << ") in GIF stream" << vcl_endl;
      assert(false);
    }

    vil_gif_image_record *ir = new vil_gif_image_record;
    ir->offset = offset;

    ir->x0 = vil_16bit_read_little_endian(s);
    ir->y0 = vil_16bit_read_little_endian(s);
    ir->w  = vil_16bit_read_little_endian(s);
    ir->h  = vil_16bit_read_little_endian(s);
    vcl_cerr << "x0 y0 w h = " << ir->x0 << ' ' << ir->y0 << ' ' << ir->w << ' ' << ir->h << vcl_endl;

    vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;

    s->read(&b, 1);
    vcl_cerr << "b = 0x" << vcl_hex << int(b) << vcl_dec << vcl_endl;

    vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;
    if (b & 0x80) { // local colour map?
      vcl_cerr << "image has local colour map" << vcl_endl;
      int bits = 1 + (b & 0x07);
      vcl_cerr << "read local colour map (" << bits << " bits per pixel)" << vcl_endl;
      ir->color_map = new vil_gif_color_map(0x1 << bits);
      s->read(ir->color_map->cmap, 3*ir->color_map->size);
    }
    else {
      vcl_cerr << "no local colour map" << vcl_endl;
      ir->color_map = 0;
    }
    vcl_cerr << "position is 0x" << vcl_hex << s->tell() << vcl_dec << vcl_endl;

    // interlaced or sequential?
    ir->interlaced = ( (b & 0x40) != 0 );
    vcl_cerr << "image is " << (ir->interlaced ? "interlaced" : "sequential") << vcl_endl;
    if (ir->interlaced) {
      vcl_cerr << "can't read interlaced GIFs yet" << vcl_endl;
      assert(false);
    }

    // bits 543 should be zero
    if (b & 0x38) {
      vcl_cerr << "bits 543 are not zero" << vcl_endl;
      assert(false);
    }

    //
    if (ir->color_map) {
      ir->bits_per_pixel = 1 + (b & 0x07);
      vcl_cerr << "image has " << ir->bits_per_pixel << " bits per pixel" << vcl_endl;
      if (ir->bits_per_pixel != 8) {
        vcl_cerr << "cannot cope with that" << vcl_endl;
        assert(false);
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

  vcl_cerr << "read " << images.size() << " image descriptors" << vcl_endl;
  vcl_cerr << "------------ done : position = " << vcl_hex << s->tell() << vcl_dec << vcl_endl;
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
#else
  assert(0<=image && image<images.size());
  char *char_buf = (char*) buf;

  vil_gif_image_record *ir = static_cast<vil_gif_image_record*>( images[image] );

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
