#ifdef __GNUC__
#pragma implementation
#endif

//:
//  \file

#include "MovieFile.h"

#include <oxp/oxp_vidl_moviefile.h>
#include <oxp/ImageSequenceMovieFile.h>
#include <oxp/oxp_parse_seqname.h>

/////////////////////////////////////////////////////////////////////////////

//: The moviefile constructor takes the given filename, and opens it at a movie file.
//  The possible values for filename are:
//  - blah.mv                The name of an SGI movie file.
//  - blah.###.jpg           The pattern for an image sequence.
//  - blah                   Assumed to be the basename of a pattern such as
//                           blah.###.extension.   Various default extensions
//                           are searched for using blah.{start}.{ext} until
//                           one matches.
MovieFile::MovieFile(char const* filename, int start, int step, int end):
  filename_(filename),
  start_(start),
  step_(step),
  end_(end),
  qt(0),
  tmp_buf_(0)
{
  oxp_parse_seqname range(filename);
  vcl_string fn = range.filename_;
  if (range.start_ != -1) start_ = range.start_;
  if (range.step_  != -1) step_  = range.step_;
  if (range.end_   != -1) end_   = range.end_;

  // Attempt to open for reading.  need ios_in as it includes nocreate on windows...
  vcl_ifstream fd(fn.c_str(), vcl_ios_in);
  if (fd.good())
    {
      qt = new oxp_vidl_moviefile(fn.c_str());
    }
  else
    {
      qt = new ImageSequenceMovieFile(fn.c_str(), start_);
    }
}

// Destructor
MovieFile::~MovieFile()
{
  delete tmp_buf_;
  delete qt;
}

int MovieFile::index(int f)
{
  return start_ + f * step_;
}

int MovieFile::GetLength()
{
  return qt->GetLength();
}

int MovieFile::GetNumFrames()
{
  // Probe...
  if (end_ == -1) {
    vcl_cerr << "MovieFile: probing ";
    int i;
    for (i = 0; GetImage(i); ++i)
      ;
    --i;
    end_ = start_ + i * step_;
    vcl_cerr << " done\n";
  }
  return (end_ - start_) / step_ + 1;
}

vil_image MovieFile::GetImage(int frame_index)
{
  return qt->GetImage(index(frame_index));
}

int MovieFile::GetSizeX(int frame_index)
{
  return qt->GetSizeX(index(frame_index));
}

int MovieFile::GetSizeY(int frame_index)
{
  return qt->GetSizeY(index(frame_index));
}

int MovieFile::GetBitsPixel()
{
  return qt->GetBitsPixel();
}

bool MovieFile::HasFrame(int frame_index)
{
  return qt->HasFrame(index(frame_index));
}

void MovieFile::GetFrame(int frame_index, vil_memory_image_of<vil_rgb<unsigned char> >& frame)
{
  int sx = GetSizeX(frame_index);
  int sy = GetSizeY(frame_index);
  frame.resize(sx,sy);
  GetFrame(frame_index, (vil_rgb<unsigned char> *)frame.get_buffer());
}

void MovieFile::GetFrame(int frame_index, vil_memory_image_of<vil_byte>& frame)
{
  int sx = GetSizeX(frame_index);
  int sy = GetSizeY(frame_index);
  frame.resize(sx,sy);
  GetFrame(frame_index, (vil_byte*)frame.get_buffer());
}

void MovieFile::GetFrameRGB(int frame_index, vil_byte* frame)
{
  GetFrame(frame_index, (vil_rgb<unsigned char> *)frame);
}

void MovieFile::GetFrame(int frame_index, vil_rgb<unsigned char> * frame)
{
  if (qt->GetBitsPixel() == 24)
    qt->GetFrame(index(frame_index), frame);
  else {
    // Grab gray into first part and splay out into rgb
    qt->GetFrame(index(frame_index), frame);
    int sx = GetSizeX((frame_index));
    int sy = GetSizeY((frame_index));
    int size = sx * sy;
    vil_byte* base = (vil_byte*)frame;
    vil_byte* rgb_ptr = base + size*3;
    vil_byte* gray_ptr = base + size;
    do {
      --gray_ptr;
      *--rgb_ptr = *gray_ptr;
      *--rgb_ptr = *gray_ptr;
      *--rgb_ptr = *gray_ptr;
    } while (gray_ptr != base);
  }
}

void MovieFile::GetFrame(int frame_index, vil_byte* frame)
{
  if (qt->GetBitsPixel() == 8) {
    // Grab gray directly
    qt->GetFrame(index(frame_index), frame);
  } else {
    int sx = GetSizeX((frame_index));
    int sy = GetSizeY((frame_index));

    // Grab colour into temp buffer
    if (!tmp_buf_)
      tmp_buf_ = new vil_memory_image_of<vil_rgb<unsigned char> >(sx, sy);
    else
      tmp_buf_->resize(sx, sy);

    qt->GetFrame(index(frame_index), (vil_byte*)tmp_buf_->get_buffer());
    int size = sx * sy;
    vil_rgb<unsigned char> * rgb_ptr = (vil_rgb<unsigned char> *)tmp_buf_->get_buffer();
    vil_byte* gray_ptr = frame;
    for (int i = 0; i < size; ++i)
      *gray_ptr++ = rgb_ptr++->grey();
  }
}
