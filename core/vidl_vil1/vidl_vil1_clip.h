#ifndef vidl_vil1_clip_h
#define vidl_vil1_clip_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   June 2000 Julien ESTEVE          Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   10/9/2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vidl_vil1/vidl_vil1_clip_sptr.h>
#include <vidl_vil1/vidl_vil1_frame_sptr.h>
#include <vidl_vil1/vidl_vil1_codec_sptr.h>
#include <vil1/vil1_image.h>

#include <vcl_vector.h>
#include <vcl_list.h>

//: Sequence of frames, element of a movie
//   A clip is a set of frames, it is part of a Movie.
//   It has the notion of starting frame, ending frame
//   and increment, so that when we iterate through the
//   sequence, it will iterate only through the frames
//   of interest.
//
//   See also vidl_vil1_frame and vidl_vil1_movie.

class vidl_vil1_clip : public vbl_ref_count
{
  // PUBLIC INTERFACE
 public:

  // Constructors
  vidl_vil1_clip(vidl_vil1_codec_sptr codec,
                 int start = 0,
                 int end = 0,
                 int increment = 1);

  vidl_vil1_clip(vcl_list<vil1_image> &images,
                 int start = 0,
                 int end = 0,
                 int increment = 1);

  vidl_vil1_clip(vcl_vector<vil1_image> &images,
                 int start = 0,
                 int end = 0,
                 int increment = 1);

  vidl_vil1_clip(vidl_vil1_clip const& x);

  // Destructor
  ~vidl_vil1_clip() {}

  // Data Access
  vidl_vil1_frame_sptr get_frame(int n);

  //: Return the number of frames
  int length() { return (endframe_-startframe_)/increment_ + 1; }

  // Get the size in pixels
  int width() const;
  int height() const;

  //: Return the codec.
  vidl_vil1_codec_sptr get_codec();

 protected:
  void init(int start, int end, int increment);

  // Data Members
  vcl_vector<vidl_vil1_frame_sptr> frames_; //!< Where are the frames stored
  int startframe_;               //!< The clip begins at startframe_
  int endframe_;                 //!< The clip ends at startframe_
  int increment_;                //!< The clip uses 1 frame every "increment_"
  double frame_rate_;            //!< 1 frame every ?? secs
  vidl_vil1_codec_sptr coder_;   //!< video codec used for storage
};

#endif // vidl_vil1_clip_h
