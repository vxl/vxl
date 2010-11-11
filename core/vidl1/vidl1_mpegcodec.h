// This is core/vidl1/vidl1_mpegcodec.h
#ifndef vidl1_mpegcodec_h
#define vidl1_mpegcodec_h
//:
// \file
// \author l.e.galup
// \date July 2002
//
//  this class sets up and executes the vidl1_mpegcodec_helper, which
//  is a wrapped port of mpeg2dec. it accepts the vo open function ptr
//  in the constructor. this was done this way to make it more extensible,
//  in case other video outputs would be added later.
//
//  To use:
//  1) load the file
//  2) set the attributes. format, demux, ...
//  3) then use get_section. the get section call will actualize
//     initialize the helper class. it's a one shot deal. once
//     initialized, you are stuck with that roi. i could change
//     this later, if it proves necessary.
//
//  this class works on both mpeg1 and mpeg2.

#include "vidl1_mpegcodec_helper.h"
#include <vidl1/vidl1_codec.h>
#include <vidl1/vidl1_codec_sptr.h>

class vil_image_resource;

//: Allows user to load MPEG files as vxl video.
class vidl1_mpegcodec : public vidl1_codec
{
  //--- these are set by load/save
  vidl1_mpegcodec_helper * decoder_;
  frame_buffer * buffers_;
  bool inited;

  //-------------------------------------------------

 public:
  vidl1_mpegcodec();
  ~vidl1_mpegcodec();

  //Casting methods overridden here...
  vidl1_mpegcodec* castto_vidl1_mpegcodec() { return this; }

  //-----------------------------------------------------
  //pure virtual methods
  //------------------------------------------

  vil_image_view_base_sptr get_view( int frame_position,
                                     int x0, int xs,
                                     int y0, int ys ) const;

  bool put_view( int /*frame_position*/,
                 const vil_image_view_base & /*im*/,
                 int /*x0*/, int /*y0*/ ) { return false; }

  virtual bool probe(vcl_string const& fname);
  virtual vidl1_codec_sptr load(vcl_string const& fname, char mode = 'r' );
  bool save(vidl1_movie* /*movie*/, vcl_string const& /*fname*/) { return true; }
  virtual vcl_string type() const { return "MPEG"; }

  //-----------------------------------------------
  //initialization methods.
  //these must be set by the user before init is called,
  //but after load. init must be called, before get_section is
  //called.
  void set_grey_scale(bool grey);
  void set_demux_video();
  void set_pid(vcl_string pid);
  bool init();
};

#endif // vidl1_mpegcodec_h
