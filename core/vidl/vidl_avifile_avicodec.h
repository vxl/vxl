// This is core/vidl/vidl_avifile_avicodec.h
#ifndef vidl_avifile_avicodec_h
#define vidl_avifile_avicodec_h
//:
// \file
// \author Matt Leotta
// \date 05/12/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <avifile/avifile.h>
#include <vidl/vidl_codec.h>

//: Allows user to load an AVI video with avifile
// See also vidl_codec
class vidl_avicodec : public vidl_codec
{
 public:
  //: Constructor
  vidl_avicodec();
  //: Destructor
 ~vidl_avicodec();

  //-----------------------------------------------------

  virtual vil_image_view_base_sptr  get_view(int position,
                                             int x0, int xs,
                                             int y0, int ys ) const;
  virtual bool put_view(int position,
                        const vil_image_view_base &im,
                        int x0, int y0 );

  //-----------------------------------------------------
  virtual bool probe(vcl_string const& fname);
  virtual vidl_codec_sptr load(vcl_string const& fname, char mode = 'r' );
  virtual bool save(vidl_movie* movie, vcl_string const& fname) { return false; }
  virtual vcl_string type() const { return "AVI"; }
  virtual vidl_avicodec* castto_vidl_avicodec() { return this; }

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

#endif // vidl_avifile_avicodec_h
