#ifndef vidl_vil1_movie_h
#define vidl_vil1_movie_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//   Modifications
//    Julien ESTEVE, June 2000 -   Ported from TargetJr
//    10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vidl_vil1/vidl_vil1_movie_sptr.h>
#include <vidl_vil1/vidl_vil1_frame_sptr.h>
#include <vidl_vil1/vidl_vil1_clip_sptr.h>
#include <vil1/vil1_image.h>
#include <vcl_list.h>

//: Video movie
//   A vidl_vil1_movie is a movie sequence
//   It is basically a list of clips.
// See also vidl_vil1_frame and vidl_vil1_clip.
class vidl_vil1_movie : public vbl_ref_count
{
  // PUBLIC INTERFACE
 public:

  // Constructors/Initializers/Destructors
  vidl_vil1_movie ();
  vidl_vil1_movie(vidl_vil1_clip_sptr clip);
  ~vidl_vil1_movie();
  vidl_vil1_movie(const vidl_vil1_movie&);

  // Operators
  vidl_vil1_movie& operator=(const vidl_vil1_movie&);

  // Data Access
  vidl_vil1_frame_sptr get_frame(int n);
  vil1_image get_image(int n);
  int length() const;
  unsigned int frame_rate() const {return frame_rate_;}
  void set_frame_rate(unsigned int fr) {frame_rate_ = fr;}

  vcl_list<vidl_vil1_clip_sptr> get_clips() const {return clip_;}

  int width() const;
  int height() const;

  // Data Control
  void add_clip(vidl_vil1_clip_sptr  clip);

 private:
  unsigned int frame_rate_;

 protected:
  // Data Members
  vcl_list<vidl_vil1_clip_sptr> clip_;


  //----------------------------
  // Iterator definition
  //----------------------------
 public:

  class frame_iterator
  {
    //
    // This class is an iterator for the frames in a vidl_vil1_movie.
    // One should think of a vidl_vil1_movie as being a list of vidl_vil1_frames.
    // The frame_iterator allows one to access the individual frames.
    // Thus, dereferencing the frame iterator gives a vidl_vil1_frame.
    // i.e. if we have frame_iterator it, then *it is a vidl_vil1_frame.
    // and it-> accesses the members of vidl_vil1_frame.
    //
    // Correct way of running through all the frames of a movie is
    // for (vidl_vil1_movie::frame_iterator frame = movie.begin();
    //      frame != movie.end();
    //      ++frame)
    //
    // Also look at examples/vidl_vil1_mpegcodec_example.cxx for other examples

   private :
    vidl_vil1_movie_sptr movie_;
    int frame_number_;

   public :
    // Constructors / destructors
    frame_iterator (vidl_vil1_movie_sptr movie, int frame_number) :
      movie_(movie), frame_number_(frame_number) {}

    frame_iterator (vidl_vil1_movie_sptr movie) :
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

    // Treating as a vidl_vil1_frame_sptr
    operator vidl_vil1_frame_sptr () const
    {
      if (frame_number_ < 0 || frame_number_ >= movie_->length())
        return vidl_vil1_frame_sptr(0);
      return movie_->get_frame(frame_number_);
    }

    vidl_vil1_frame_sptr operator -> () const { return (vidl_vil1_frame_sptr) *this; }

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
    bool operator ==(int n) { return frame_number_ == n; }
    bool operator !=(int n) { return frame_number_ != n; }
    bool operator < (int n) { return frame_number_ < n; }
    bool operator > (int n) { return frame_number_ > n; }
    bool operator <=(int n) { return frame_number_ <= n; }
    bool operator >=(int n) { return frame_number_ >= n; }

    // Data member access
    int current_frame_number () { return frame_number_; }
    vidl_vil1_movie_sptr get_movie ()  { return movie_; }
  };

  //---------------------------------------------------------------------
  // Methods that return iterators
  frame_iterator first() { return frame_iterator(this, 0); }
  frame_iterator last()  { return frame_iterator(this,this->length()-1); }
  frame_iterator begin() { return first(); }
  frame_iterator end()   { return frame_iterator(this,this->length()); }
};

#endif // vidl_vil1_movie_h
