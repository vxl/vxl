// This is oxl/oxp/SGIMovieFileWrite.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "SGIMovieFileWrite.h"

#include <vcl_cstring.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#undef sprintf // works around a bug in libintl.h
#undef fprintf
#include <vcl_cstdio.h>

#include <vil1/vil1_jpeglib.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

const int align = 4;
inline int ROUNDUP(int x)
{
  return int((x + align - 1) / align) * align;
}

void send4(FILE* fp, unsigned int val)
{
  vcl_fwrite(&val, 4, 1, fp);
}

struct Vars {
  vcl_vector<vcl_string> names;
  vcl_vector<vcl_string> values;
  char buf[1024];

  void add(char const* tag, const char* value) {
    names.push_back(vcl_string(tag));
    values.push_back(vcl_string(value));
  }

  void add(char const* tag, int value) {
    vcl_sprintf(buf, "%d", value);
    add(tag, buf);
  }
  void add(char const* tag, double value) {
    vcl_sprintf(buf, "%g", value);
    add(tag, buf);
  }

  void send(FILE* fp) {
    send4(fp, 0);
    send4(fp, names.size());
    send4(fp, 0);
    for (unsigned i = 0; i < names.size(); ++i) {
      // Send name
      int l = names[i].size();
      vcl_strncpy(buf, names[i].c_str(), l);
      while (l < 16) buf[l++] = 0;
      vcl_fwrite(buf, 16, 1, fp);
      // Send values
      l = values[i].size();
      send4(fp, l + 1);
      vcl_fwrite(values[i].c_str(), l+1, 1, fp);
    }
  }
};

struct SGIMovieFileWriteData {
  SGIMovieFileWriteData(char const* filename,
                        int w,
                        int h,
                        int length);

  void PutFrame(int i);
  void Finish();

  int w,h,l;
  vcl_vector<unsigned> frame_ends;

  bool interlaced;

  vil1_memory_image_of<vil1_rgb<unsigned char> > buffer;
  vcl_vector<JSAMPLE*> rows;
  FILE *fp;
  int directory_pos;
  int first_frame;

  jpeg_compress_struct cinfo;
  jpeg_error_mgr jerr;

  void send4(unsigned int val) { vcl_fwrite(&val, 4, 1, fp); }
};

SGIMovieFileWriteData::SGIMovieFileWriteData(char const* filename,
                                             int w_,
                                             int h_,
                                             int l_):
  w(w_),h(h_),l(l_),
  frame_ends(l, (unsigned)0),
  buffer(w_, h_),
  rows(h_)
{
  fp = vcl_fopen(filename, "w");
  if (!fp) {
    vcl_cerr << "SGIMovieFileWriteData: Can't open " << filename << '\n';
    return;
  }

  interlaced = true;

  // Write header
  vcl_fprintf(fp, "MOVI");
  send4(3);
  send4(0);

  Vars glob;
  glob.add("__NUM_I_TRACKS", 1);
  glob.add("__NUM_A_TRACKS", 0);
  glob.add("LOOP_MODE", 0);
  glob.add("OPTIMIZED", 0);
  glob.add("NUM_LOOPS", 0);
  glob.send(fp);

  Vars itrack;
  itrack.add("WIDTH", w);
  itrack.add("COMPRESSION", 10);
  itrack.add("ORIENTATION", 1100);
  itrack.add("Q_TEMPORAL", 0.750000);
  itrack.add("HEIGHT", h);
  itrack.add("PIXEL_ASPECT", 1.000000);
  itrack.add("__DIR_COUNT", l);
  itrack.add("INTERLACING", (int)interlaced);
  itrack.add("FPS", 30.000000);
  itrack.add("Q_SPATIAL", 0.750000);
  itrack.add("PACKING", 1001);
  itrack.send(fp);

  directory_pos = ftell(fp);
  first_frame = ROUNDUP(directory_pos + 4*4*l);
  fseek(fp, first_frame, 0);

  // Make jpegger
  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_compress(&cinfo);

  jpeg_stdio_dest(&cinfo, fp);

  cinfo.image_width = w;      /* image width and height, in pixels */
  cinfo.image_height = h / (1 + interlaced);
  cinfo.input_components = 3;     /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; /* colorspace of input image */

  jpeg_set_defaults(&cinfo);
  /* Make optional parameter settings here */

  // Build jpeg rows.
  if (interlaced)
    for (int y = 0; y < h / 2; ++y) {
      rows[y] = (JSAMPLE*)&buffer(0,2*y);
      rows[y + h/2] = (JSAMPLE*)&buffer(0,2*y+1);
    }
  else
    for (int y = 0; y < h; ++y)
      rows[y] = (JSAMPLE*)&buffer(0,y);
}

struct FrameIndex {
  int offset;
  int size;
  int pad;
  int frame;
};

void SGIMovieFileWriteData::Finish()
{
  if (l != (int)frame_ends.size()) {
    vcl_cerr << "ZOKZOK: " << l << " != " << frame_ends.size() << '\n';
    return;
  }
  fseek(fp, directory_pos, 0);
  for (int i = 0; i < l; ++i) {
    FrameIndex f;
    int start = (i == 0)?first_frame : frame_ends[i-1];
    int end = frame_ends[i];

    f.offset = start;
    f.size = end - start;
    f.pad = 0;
    f.frame = 0;
    vcl_fwrite(&f, 4, 4, fp);
  }

  jpeg_destroy_compress(&cinfo);
  vcl_fclose(fp);
}

extern unsigned long jpeg_stdio_ftell(jpeg_compress_struct*);

void SGIMovieFileWriteData::PutFrame(int i)
{
  bool write_all_tables = true;
  if (!interlaced) {
    jpeg_start_compress (&cinfo, write_all_tables);
    jpeg_write_scanlines(&cinfo, &rows[0], h);
    jpeg_finish_compress(&cinfo);
  } else {
    jpeg_start_compress (&cinfo, write_all_tables);
    jpeg_write_scanlines(&cinfo, &rows[0], h/2);
    jpeg_finish_compress(&cinfo);
    jpeg_start_compress (&cinfo, write_all_tables);
    jpeg_write_scanlines(&cinfo, &rows[h/2], h/2);
    jpeg_finish_compress(&cinfo);
  }

  int pos = ROUNDUP(ftell(fp));
  fseek(fp, pos, SEEK_SET);
  frame_ends[i] = pos;
}

/////////////////////////////////////////////////////////////////////////////

SGIMovieFileWrite::SGIMovieFileWrite(char const* filename,
                                     int w,
                                     int h,
                                     int length)
{
  p = new SGIMovieFileWriteData(filename, w, h, length);
}

SGIMovieFileWrite::~SGIMovieFileWrite()
{
  delete p;
}

void SGIMovieFileWrite::Finish()
{
  p->Finish();
}

unsigned char* SGIMovieFileWrite::GetBuffer()
{
  return (unsigned char*)p->buffer.get_buffer();
}

void SGIMovieFileWrite::PutBuffer(int frame_index)
{
  p->PutFrame(frame_index);
}
