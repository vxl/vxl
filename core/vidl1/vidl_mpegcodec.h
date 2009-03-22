// This is core/vidl/vidl_mpegcodec.h
#ifndef vidl_mpegcodec_h
#define vidl_mpegcodec_h
//:
// \file
//
// \author l.e.galup
//  this class sets up and executes the vidl_mpegcodec_helper, which
//  is a wrapped port of mpeg2dec. it accepts the vo open function ptr
//  in the constructor. this was done this way to make it more extensible,
//  in case other video outputs would be added later.
//
//  To use:
//  1) load the file
//  2) set the attributes. format, demux, ...
//  3) then use get_section. the get section call will actualize
//     initialize the helper class. its a one shot deal. once
//     initialized, you are stuck with that roi. i could change
//     this later, if it proves necessary.
//
//  this class works on both mpeg1 and mpeg2.
// \date July 2002

#include "vidl_mpegcodec_helper.h"
#include <vidl/vidl_codec.h>
#include <vidl/vidl_codec_sptr.h>

class vil_image_resource;

//: Allows user to load MPEG files as vxl video.
class vidl_mpegcodec : public vidl_codec
{
  //--- these are set by load/save
  vidl_mpegcodec_helper * decoder_;
  frame_buffer * buffers_;
  bool inited;

  //-------------------------------------------------

 public:
  vidl_mpegcodec();
  ~vidl_mpegcodec();

  //Casting methods overridden here...
  vidl_mpegcodec* castto_vidl_mpegcodec() { return this; }

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
  virtual vidl_codec_sptr load(vcl_string const& fname, char mode = 'r' );
  bool save(vidl_movie* /*movie*/, vcl_string const& /*fname*/) { return true; }
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

#endif // vidl_mpegcodec_h
