// This is oxl/oxp/ImageSequenceMovieFile.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "ImageSequenceMovieFile.h"

#include <vcl_iostream.h>
#include <vil1/vil1_file_image.h>
#include <oxp/ImageSequenceName.h>

struct ImageSequenceMovieFilePrivates {
  ImageSequenceMovieFilePrivates(const char* filepattern, int frame_index_to_search_for_extension):
    seqname(filepattern, frame_index_to_search_for_extension, 1, "r"),
    current_image_index(-1),
    base_index(frame_index_to_search_for_extension)
    {
      seqname.start_frame_ = 0;
    }

  vil1_image get_image(int index) {
    if (!current_image || (current_image_index != index)) {
      // Load new image
      vcl_string newname = seqname.name(index);
      if (MovieFileInterface::verbose)
        vcl_cerr << "ImageSequenceMovieFile: Loading [" << newname << "]: ";
      current_image.load(newname.c_str(), MovieFileInterface::verbose ? vil1_file_image::laconic : vil1_file_image::silent);
      current_image_index = index;
    }
    return current_image;
  }

 public:
  ImageSequenceName seqname;
  int current_image_index;
  vil1_file_image current_image;
  int base_index;
};

//: Ctor
ImageSequenceMovieFile::ImageSequenceMovieFile(char const* filepattern, int frame_index_to_search_for_extension):
  p(new ImageSequenceMovieFilePrivates(filepattern, frame_index_to_search_for_extension))
{
}

ImageSequenceMovieFile::~ImageSequenceMovieFile()
{
  delete p;
}

//: Will need to search...
int ImageSequenceMovieFile::GetLength()
{
  return p->seqname.n();
}

vil1_image ImageSequenceMovieFile::GetImage(int frame_index)
{
  return p->get_image(frame_index);
}

int ImageSequenceMovieFile::GetSizeX(int frame_index)
{
  return p->get_image(frame_index).width();
}

int ImageSequenceMovieFile::GetSizeY(int frame_index)
{
  return p->get_image(frame_index).height();
}

int ImageSequenceMovieFile::GetBitsPixel()
{
  vil1_image animage = (p->current_image != 0) ? vil1_image(p->current_image) : p->get_image(p->base_index);
  return animage.components() * animage.bits_per_component();
}

//: Assumes disk-stored images are never interlaced
bool ImageSequenceMovieFile::IsInterlaced()
{
  return false;
}

bool ImageSequenceMovieFile::HasFrame(int frame_index)
{
  return p->get_image(frame_index) != 0;
}

bool ImageSequenceMovieFile::GetFrame(int frame_index, void* buffer)
{
  vil1_image image = p->get_image(frame_index);
  image.get_section(buffer, 0, 0, image.width(), image.height());
  return true;
}

bool ImageSequenceMovieFile::GetField(int field_index, void* buffer)
{
  return GetFrame(field_index, buffer);
}
