// This is core/vidl/vidl_avicodec.h
#ifndef vidl_avicodec_h
#define vidl_avicodec_h
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
#include <vidl/vidl_avicodec_sptr.h>
#include <vidl/vidl_frame_sptr.h>


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
  virtual bool probe(const char* fname);
  virtual vidl_codec_sptr load(const char* fname, char mode = 'r' );
  virtual bool save(vidl_movie* movie, const char* fname) { return false; }
  virtual  const char* type() {return "AVI";}
  virtual vidl_avicodec* castto_vidl_avicodec(){return this;}

 protected:
  int seek(int frame_num);
  int next_frame();
  
  bool load_avi(const char* fname, char mode);
 
 private:

  int current_frame_;
  //vxl_byte* buffer_;
  IAviReadFile* moviefile_;
  IAviReadStream* moviestream_;

};


#endif // vidl_avicodec_h
