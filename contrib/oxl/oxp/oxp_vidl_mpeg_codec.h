#ifndef oxp_vidl_mpeg_codec_h
#define oxp_vidl_mpeg_codec_h
//:
// \file
// \author awf
// \date Dec 2001

#include <vidl_vil1/vidl_vil1_codec.h>
#include <oxp/oxp_mpeg_codec.h>

//: Allows user to load MPEG files as vxl video.
// use set_demux if mpeg is a VOB
class oxp_vidl_mpeg_codec : public vidl_vil1_codec
{
 public:
  //-----------------------------------------------------

  virtual bool   get_section(int position, void* ib, int x0, int y0, int xs, int ys) const {
    return p.get_section(position, ib, x0, y0, xs, ys);
  }
  virtual int    put_section(int position, void* ib, int x0, int y0, int xs, int ys) {
    return p.put_section(position, ib, x0, y0, xs, ys);
  }

  //-----------------------------------------------------
  virtual bool probe(const char* fname) { return p.probe(fname); }
  virtual vidl_vil1_codec_sptr load(const char* fname, char mode = 'r' );
  virtual bool save(vidl_vil1_movie* movie, const char* fname);
  virtual const char* type();

  // Call before destruction to a void segv on exit
  void close() { p.close(); }

 private:
  oxp_mpeg_codec p;
};

#endif // oxp_vidl_mpeg_codec_h
