#ifndef vidl_clip_h
#define vidl_clip_h
//-----------------------------------------------------------------------------
//
// .NAME vidl_clip - sequence of frames, element of a movie
// .LIBRARY vidl
// .HEADER vxl package
// .INCLUDE vidl/vidl_clip.h
// .FILE vidl_clip.cxx
//
// .SECTION Description
//   A clip is a set of frames, it is part of a Movie.
//   It has the notion of starting frame, ending frame
//   and increment, so that when we iterate through the
//   sequence, it will iterate only through the frames
//   of interest.
//
// .SECTION See also
//   vidl_frame
//   vidl_movie
//
// .SECTION Author
//    Nicolas Dano, march 1999
//
// .SECTION Modifications
//   Julien Esteve, May 2000
//   Ported from TargetJr
//
//-----------------------------------------------------------------------------


#include <vbl/vbl_ref_count.h>
#include <vidl/vidl_clip_sptr.h>
#include <vidl/vidl_frame_sptr.h>
#include <vidl/vidl_codec_sptr.h>
#include <vil/vil_image.h>

#include <vcl_vector.h>
#include <vcl_list.h>

class vidl_clip : public vbl_ref_count
{
  // PUBLIC INTERFACE
public:
  
  // Constructors
  vidl_clip(
	vidl_codec_sptr codec, 
	int start = 0, 
	int end = 0, 
	int increment = 1
	);

  vidl_clip(
	vcl_list<vil_image> &images, 
	int start = 0, 
	int end = 0, 
	int increment = 1
	); 

  vidl_clip(
	vcl_vector<vil_image> &images, 
	int start = 0, 
	int end = 0, 
	int increment = 1
	); 

  // Destructor
  ~vidl_clip();
  
  // Data Access
  vidl_frame_sptr get_frame(int n);
  int length();
  
  // Get the size in pixels
  int width() const ;
  int height() const ;
  
  vidl_codec_sptr get_codec();
  
protected:
  void init(int start, int end, int increment);
  
  // Data Members
  vcl_vector<vidl_frame_sptr> frames_; // Where are stored the frames
  int startframe_;               // The clip begins at startframe_ 
  int endframe_;                 // The clip ends at startframe_ 
  int increment_;                // The clip use 1 frame every "increment_" 
  double frame_rate_;            // 1 frame every ?? secs
  vidl_codec_sptr coder_;          // video codec used for storage
};
#endif // vidl_clip_h
