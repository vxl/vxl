// This is core/vidl1/vidl1_codec.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
#include "vidl1_codec.h"
#include <vidl1/vidl1_image_list_codec.h>
#ifdef HAS_FFMPEG
#include <vidl1/vidl1_ffmpeg_codec.h>
#endif
#ifdef HAS_MPEG2
#include <vidl1/vidl1_mpegcodec.h>
#endif
#ifdef HAS_AVI
#include <vidl1/vidl1_avicodec.h>
#endif
#include "vidl1_frame_resource.h"

//: Local class to hold the codec list
// Clears list on deletion.
struct vidl1_codec_storage
{
  vidl1_codec_sptr* l;
  vidl1_codec_storage(): l(new vidl1_codec_sptr[5])
  {
    unsigned int c=0;
    l[c++] = new vidl1_image_list_codec;
#ifdef HAS_FFMPEG
    l[c++] = new vidl1_ffmpeg_codec;
#endif
#ifdef HAS_MPEG2
    l[c++] = new vidl1_mpegcodec;
#endif
#ifdef HAS_AVI
    l[c++] = new vidl1_avicodec;
#endif
    l[c++] = 0;
  }

  ~vidl1_codec_storage()
  {
    unsigned int c=0;
    while (l[c])
      l[c++]->close();
    delete [] l;
    l=0;
  }
};

vidl1_codec_sptr* vidl1_codec::all_codecs()
{
  static vidl1_codec_storage storage;
  return storage.l;
}


//: Return the resource to the image
vil_image_resource_sptr
vidl1_codec::get_resource(int position) const
{
  return new vidl1_frame_resource(const_cast<vidl1_codec*>(this), position);
  // Create a new resource of a view by default
  // since a resource is not always available
  //return vil_new_image_resource_of_view(*(this->get_view(position)));
}
