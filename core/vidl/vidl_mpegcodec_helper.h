// This is core/vidl/vidl_mpegcodec_helper.h
#ifndef vidl_mpegcodec_helper_h
#define vidl_mpegcodec_helper_h
//:
// \file
// \brief Contains classes vidl_mpegcodec_data and vidl_mpegcodec_helper
// \author l.e.galup
// \date July 2002
//
//  This is a rewrite of the mpeg2dec program. it has been modified
//  to work with the video player, i.e., by implementing a get_section
//  method. this required some buffering, and the ability to decode
//  a file grab at a time. there is some extra legacy code here.
//  though some things seem to serve no purpose, i left them there as hooks
//  for possible evolution of this class.
//
//  the mpeg_codec sends a request to this helper via
//  a decode_request. data is stored in the mpeg_codec_data class, which
//  holds a frame buffer, owned by the mpeg_codec. decoded frames are
//  stored in this frame_buffer, and subsequent user requests for frames
//  just does a memcpy from this frame_buffer into the user supplied buffer.
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_iostream.h>

#define BUFFER_SIZE 4096
#define DEMUX_PAYLOAD_START 1
#define PACKETS (BUFFER_SIZE / 188)

extern "C" {
  typedef unsigned char uint8_t;

    // In some versions of Cygwin, uint32_t is typedefed to unsigned
    // long in /usr/include/stdint.h.  When that is the case, that
    // file also #defines __uint32_t_defined.

    // Instead of these typedefs, perhaps we should be #including the
    // system inttypes.h, or the win32/inttypes.h that v3p/mpeg2
    // provides.  To do that, look at how v3p/mpeg2/CmakeLists.txt
    // adds in include directories for config.h and inttypes.h and
    // duplicate that logic to add those directories to
    // MPEG2_INCLUDE_DIR in config/cmake/Modules/FindMPEG2.cmake.

    // The root problem here seems to be that mpeg2dec makes a
    // config.h and inttypes.h (for win32) for its own use, but does
    // not set them up in such a way that other applications using
    // mpeg2dec can use those files.  Or, maybe it does when you go
    // through its install procedure.

#ifndef __uint32_t_defined
  typedef unsigned int uint32_t;
#endif
#define this c_this
#include <mpeg2dec/video_out.h>
#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mm_accel.h>
#undef this
}

class vidl_file_sequence;

class decode_request
{
 public:
  enum request_type
    {
      SEEK, FILE_GRAB, SKIP, REWIND
    };
  //seek seeks from the current file pointer
  //position until it gets frame position
  //file grab does a single fread from the
  //file of size BUFFER_SIZE

  request_type rt;
  int position; //which frame is desired
  int x0, y0, w, h; // (x0,y0) is upper left corner of roi
  bool done; //when this flag is set, the decoder stops
};

class frame_buffer
{
 public:

  frame_buffer()
    {
      buffers_ = new vcl_map<int,unsigned char *>;
    }

  void init(int width, int height, int bits_pixel)
    {
      //allocate mem 4 ring buffer
      for (int i=1; i<30 ; i++)
        {
          unsigned char * buf = new unsigned char[width*height*(bits_pixel/8)];
          (*buffers_)[-i] = buf;
        }
      return;
    }

  ~frame_buffer()
    {
      vcl_cout << "frame_buffer DTOR. entering.\n";
      vcl_map<int,unsigned char *>::iterator vmiuit = buffers_->begin();
      for (;vmiuit != buffers_->end(); vmiuit++)
        delete[] (*vmiuit).second;
      delete buffers_;
      vcl_cout << "frame_buffer DTOR. exiting.\n";
    }

  unsigned char * get_buff(int i) {return (*buffers_)[i];}
  unsigned char * next(int framenum)
  {
    //first, get the oldest frame in the buffer
    unsigned char * buf = (*buffers_->begin()).second;

    //remove it from the map
    buffers_->erase(buffers_->begin());

    //use the memory for the new frame
    (*buffers_)[framenum] = buf;

    return buf;
  }

  void print()
    {
      vcl_map<int,unsigned char *>::const_iterator vmiucit = buffers_->begin();
      for (;vmiucit != buffers_->end(); vmiucit++)
        vcl_cout << (*vmiucit).first << vcl_endl;
    }

  int first_frame_num() { return (*buffers_->begin()).first;}

  bool reset()
    {
      vcl_map<int,unsigned char *>::iterator vmit = buffers_->begin();
      for (int i=-30; vmit != buffers_->end(); vmit++,i++)
        {
          unsigned char * buf = (*vmit).second;

          //remove it from the map
          buffers_->erase(vmit);

          //use the memory for the new frame
          (*buffers_)[i] = buf;
        }
      return true;
    }

 private:
  vcl_map<int,unsigned char *> * buffers_;
};

struct vidl_mpegcodec_data : public vo_instance_t
{
 public:
  enum output_format_t
    {
      GREY, RGB
    };

  int prediction_index;
  vo_frame_t * frame_ptr[3]; //legacy code
  vo_frame_t frame[3]; //is the current frame
  int width;    //doesn't get set until the first frame gets decoded
  int height;   //doesn't get set until the first frame gets decoded
  int framenum; //current frame number. starts at -2.
                // this is because we use the first frame to find
                // the stats on the frame.
  char header[1024];
  decode_request * pending_decode; //request sent by mpeg_codec
  output_format_t output_format; //is either gray or rgb
  int last_frame_decoded; //this gets incremented after every fread
  frame_buffer * buffers;
};

//this class does the actual decoding.
class vidl_mpegcodec_helper
{
  friend class vidl_mpegcodec;

 public:
  vidl_mpegcodec_helper(vo_open_t * vopen,
                        vcl_string filename,
                        frame_buffer * buffers);
  ~vidl_mpegcodec_helper();
  bool init();
  int execute(decode_request * p);
  void print();
  int get_width() {return output_->width;}
  int get_height() {return output_->height;}
  int get_last_frame() {return output_->last_frame_decoded;}
  vidl_mpegcodec_data::output_format_t get_format() {return output_->output_format;}

 private:
  /////////////////////////////////////////////////////////
  // the following methods were lifted straight from
  // mpeg2dec-0.2.1.
  /////////////////////////////////////////////
  void decode_mpeg2 (uint8_t * current, uint8_t * end);
  int demux (uint8_t * buf, uint8_t * end, int flags);
  /////////////////////////////////////////////////////////////

  //these methods were factored out from the above
  //they correspond to the demultiplexing done
  //per stream protocol
  bool decode_ps(int reads);
  bool decode_ts(int packets);
  bool decode_es(int reads);

  // each of these arguments were static
  // and global in the original implementation
  // of mpeg2dec
  uint8_t buffer_[BUFFER_SIZE];
  vcl_string filename_;
  vidl_file_sequence * in_file_;
  int demux_track_;
  int demux_pid_;
  int disable_accel_;
  mpeg2dec_t * mpeg2dec_;
  vo_open_t * output_open_;
  vidl_mpegcodec_data * output_;
  int chunk_size_;
  int chunk_number_;
  bool (vidl_mpegcodec_helper::*decoder_routine)(int);

  bool init_; //true once init() has completed
};

#endif // vidl_mpegcodec_helper_h
