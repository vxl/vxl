// This is core/vidl/vidl_codec.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
#include "vidl_codec.h"
#include <vidl/vidl_image_list_codec.h>
#ifdef HAS_MPEG2
#include <vidl/vidl_mpegcodec.h>
#endif
#ifdef HAS_AVI
#include <vidl/vidl_avicodec.h>
#endif


//: Local class to hold the codec list
// Clears list on deletion.
struct vidl_codec_storage
{
  vidl_codec_sptr* l;
  vidl_codec_storage(): l(new vidl_codec_sptr[4])
  {
    unsigned int c=0;
    l[c++] = new vidl_image_list_codec;
#ifdef HAS_MPEG2
    l[c++] = new vidl_mpegcodec;
#endif
#ifdef HAS_AVI
    l[c++] = new vidl_avicodec;
#endif
    l[c++] = 0;
  }

  ~vidl_codec_storage()
  {
    unsigned int c=0;
    while (l[c])
      l[c++]->close();
    delete [] l;
    l=0;
  }
};

vidl_codec_sptr* vidl_codec::all_codecs()
{
  static vidl_codec_storage storage;
  return storage.l;
}


//: Return the resource to the image
vil_image_resource_sptr 
vidl_codec::get_resource(int position) const
{ 
  // Create a new resource of a view by default
  // since a resource is not always available 
  return vil_new_image_resource_of_view(*(this->get_view(position))); 
}
