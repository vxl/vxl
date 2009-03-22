// This is core/vidl1/vidl1_movie.h
#ifndef vidl1_movie_h
#define vidl1_movie_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   Julien ESTEVE, June 2000 -   Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   10/7/2003 Matt Leotta (Brown) Converted vil1 to vil
//   10/9/2004 Peter Vanroose Added copy ctor with explicit vbl_ref_count init
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vidl1/vidl1_movie_sptr.h>
#include <vidl1/vidl1_frame.h>
#include <vidl1/vidl1_clip_sptr.h>
#include <vil/vil_image_view_base.h>
#include <vcl_list.h>

//: Video movie
//   A vidl1_movie is a movie sequence.
//   It is basically a list of clips.
// See also vidl1_frame and vidl1_clip.
class vidl1_movie : public vbl_ref_count
{
  // PUBLIC INTERFACE
 public:

  // Constructors/Initializers/Destructors
  vidl1_movie ();
  vidl1_movie(vidl1_clip_sptr clip);
  ~vidl1_movie();
  vidl1_movie(vidl1_movie const& x)
    : vbl_ref_count(), frame_rate_(x.frame_rate_), clip_(x.clip_) {}

  // Operators
  vidl1_movie& operator=(const vidl1_movie&);

  // Data Access
  vidl1_frame_sptr get_frame(int n);
  vil_image_view_base_sptr get_view(int n);
  int length() const;
  unsigned int frame_rate() const {return frame_rate_;}
  void set_frame_rate(unsigned int fr) {frame_rate_ = fr;}

  vcl_list<vidl1_clip_sptr> get_clips() const {return clip_;}

  int width() const;
  int height() const;

  // Data Control
  void add_clip(vidl1_clip_sptr  clip);

 private:
  unsigned int frame_rate_;

 protected:
  // Data Members
  vcl_list<vidl1_clip_sptr> clip_;


  //----------------------------
  // Iterator definition
  //----------------------------
 public:

  class frame_iterator
  {
    //
    // This class is an iterator for the frames in a vidl1_movie.
    // One should think of a vidl1_movie as being a list of vidl1_frames.
    // The frame_iterator allows one to access the individual frames.
    // Thus, dereferencing the frame iterator gives a vidl1_frame.
    // i.e. if we have frame_iterator it, then *it is a vidl1_frame.
    // and it-> accesses the members of vidl1_frame.
    //
    // Correct way of running through all the frames of a movie is
    // for (vidl1_movie::frame_iterator frame = movie.begin();
    //      frame != movie.end();
    //      ++frame)
    //
    // Also look at examples/vidl1_mpegcodec_example.cxx for other examples

   private :
    vidl1_movie* movie_;
    int frame_number_;

   public :
    // Constructors / destructors
    frame_iterator (vidl1_movie* movie, int frame_number) :
      movie_(movie), frame_number_(frame_number) {}

    frame_iterator (vidl1_movie* movie) :
      movie_(movie), frame_number_(0) {}

    frame_iterator (const frame_iterator &fr) :
      movie_(fr.movie_), frame_number_(fr.frame_number_) {}

    ~frame_iterator () {}

    // Assigning one iterator to another
    frame_iterator& operator= (const frame_iterator &fr)
    {
      movie_ = fr.movie_;
      frame_number_ = fr.frame_number_;
      return *this;
    }

    frame_iterator& operator= (int n) { frame_number_ = n; return *this; }

    // Incrementing the frame number
    frame_iterator& operator++ () { frame_number_ ++; return *this; }
    frame_iterator& operator-- () { frame_number_ --; return *this; }
    frame_iterator& operator+= (int n) { frame_number_ += n; return *this; }
    frame_iterator& operator-= (int n) { frame_number_ -= n; return *this; }

    // Treating as a vidl1_frame_sptr
    operator vidl1_frame_sptr () const
    {
      if (frame_number_ < 0 || frame_number_ >= movie_->length())
        return vidl1_frame_sptr(0);
      return movie_->get_frame(frame_number_);
    }

    vidl1_frame_sptr operator -> () const { return (vidl1_frame_sptr) *this; }

    // Comparison against other iterators
    friend bool operator == (const frame_iterator &fr1,
                             const frame_iterator &fr2)
       { return fr1.frame_number_ == fr2.frame_number_; }

    friend bool operator != (const frame_iterator &fr1,
                             const frame_iterator &fr2)
       { return fr1.frame_number_ != fr2.frame_number_; }

    friend bool operator <(const frame_iterator&fr1,
                           const frame_iterator&fr2)
       { return fr1.frame_number_ < fr2.frame_number_; }
    friend bool operator > (const frame_iterator &fr1,
                            const frame_iterator& fr2)
       { return fr1.frame_number_ > fr2.frame_number_; }

    friend bool operator <= (const frame_iterator &fr1,
                             const frame_iterator &fr2)
       { return fr1.frame_number_ <= fr2.frame_number_; }

    friend bool operator >= (const frame_iterator &fr1,
                             const frame_iterator &fr2)
       { return fr1.frame_number_ >= fr2.frame_number_; }

    // Comparison against integers
    bool operator ==(int n) const { return frame_number_ == n; }
    bool operator !=(int n) const { return frame_number_ != n; }
    bool operator < (int n) const { return frame_number_ < n; }
    bool operator > (int n) const { return frame_number_ > n; }
    bool operator <=(int n) const { return frame_number_ <= n; }
    bool operator >=(int n) const { return frame_number_ >= n; }

    // Data member access
    int current_frame_number () const { return frame_number_; }
    vidl1_movie* get_movie() const { return movie_; }
  };

  //---------------------------------------------------------------------
  // Methods that return iterators
  frame_iterator first() { return frame_iterator(this, 0); }
  frame_iterator last()  { return frame_iterator(this,this->length()-1); }
  frame_iterator begin() { return first(); }
  frame_iterator end()   { return frame_iterator(this,this->length()); }
};

#endif // vidl1_movie_h
