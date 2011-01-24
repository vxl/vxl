// This is core/vidl1/vidl1_avifile_avicodec.h
#ifndef vidl1_avifile_avicodec_h
#define vidl1_avifile_avicodec_h
//:
// \file
// \author Matt Leotta
// \date May 12, 2004
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_cstddef.h> // for size_t, used in some of the avifile #includes
#include <avifile.h>
#include <vidl1/vidl1_codec.h>

//: Allows user to load an AVI video with avifile
// See also vidl1_codec
class vidl1_avicodec : public vidl1_codec
{
 public:
  //: Constructor
  vidl1_avicodec();
  //: Destructor
 ~vidl1_avicodec();

  //-----------------------------------------------------

  virtual vil_image_view_base_sptr  get_view(int position,
                                             int x0, int xs,
                                             int y0, int ys ) const;
  virtual bool put_view(int position,
                        const vil_image_view_base &im,
                        int x0, int y0 );

  //-----------------------------------------------------
  virtual bool probe(vcl_string const& fname);
  virtual vidl1_codec_sptr load(vcl_string const& fname, char mode = 'r' );
  virtual bool save(vidl1_movie* /*movie*/, vcl_string const& /*fname*/) { return false; }
  virtual vcl_string type() const { return "AVI"; }
  virtual vidl1_avicodec* castto_vidl1_avicodec() { return this; }

 protected:
  int seek(int frame_num) const;
  int next_frame() const;

  bool load_avi(vcl_string const& fname, char mode);

 private:

  mutable int current_frame_;
  //vxl_byte* buffer_;
  IAviReadFile* moviefile_;
  IAviReadStream* moviestream_;
};

#endif // vidl1_avifile_avicodec_h
