#ifndef vidl_vil1_mpegcodec_h
#define vidl_vil1_mpegcodec_h
//:
// \file
//
// \author l.e.galup
//  this class sets up and executes the vidl_vil1_mpegcodec_helper, which
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

#include <vidl_vil1/vidl_vil1_mpegcodec_helper.h>
#include <vidl_vil1/vidl_vil1_codec.h>
#include <vidl_vil1/vidl_vil1_codec_sptr.h>

class vil1_image;

//: Allows user to load MPEG files as vxl video.
class vidl_vil1_mpegcodec : public vidl_vil1_codec
{
  //--- these are set by load/save
  vidl_vil1_mpegcodec_helper * decoder_;
  frame_buffer * buffers_;
  bool inited;

  //-------------------------------------------------

 public:
  vidl_vil1_mpegcodec();
  ~vidl_vil1_mpegcodec();

  //Casting methods overridden here...
  vidl_vil1_mpegcodec* castto_vidl_vil1_mpegcodec(){return this;}

  //-----------------------------------------------------
  //pure virtual methods
  //------------------------------------------

  bool   get_section(int frame_position,
                     void* ib,
                     int x0,
                     int y0,
                     int xs,
                     int ys) const;
  int    put_section(int /*frame_position*/,
                     void* /*ib*/,
                     int /*x0*/,
                     int /*y0*/,
                     int /*xs*/,
                     int /*ys*/){return 0;}

  virtual bool probe(const char* fname);
  virtual vidl_vil1_codec_sptr load(const char* fname, char mode = 'r' );
  bool save(vidl_vil1_movie*  /*movie*/, const char*  /*fname*/){return true;}
  virtual const char* type() {return "MPEG";}

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

#endif // vidl_vil1_mpegcodec_h
