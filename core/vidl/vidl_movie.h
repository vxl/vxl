#ifndef vidl_movie_h
#define vidl_movie_h
//-----------------------------------------------------------------------------
//
// .NAME vidl_movie - Video movie
// .LIBRARY vidl
// .HEADER vxl package
// .INCLUDE vidl/vidl_movie.h
// .FILE vidl_movie.cxx
// .EXAMPLE vidl_test.cxx
//
// .SECTION Description
//   A vidl_movie is a movie sequence
//   It is basically a list of clips
//
// .SECTION See also
//   vidl_frame
//   vidl_clip
//
// .SECTION Author
//   Nicolas Dano, march 1999
//
// .SECTION Modifications
//   Julien ESTEVE, May 2000
//   Ported from TargetJr
//
//-----------------------------------------------------------------------------


#include <vbl/vbl_ref_count.h>
#include <vidl/vidl_movie_sptr.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_frame_sptr.h>
#include <vidl/vidl_clip.h>
#include <vil/vil_image.h>
#include <vcl_list.h>

class vidl_movie : public vbl_ref_count
{
  // PUBLIC INTERFACE
public:

  // Constructors/Initializers/Destructors
  vidl_movie ();
  vidl_movie(vidl_clip_sptr clip);
  ~vidl_movie();
  vidl_movie(const vidl_movie&);

  // Operators
  vidl_movie& operator=(const vidl_movie&);

  // Data Access
  vidl_frame_sptr get_frame(int n);
  int length() const;

  vcl_list<vidl_clip_sptr> get_clips() const {return clip_;}

  int width() const;
  int height() const;

  // Data Control
  void add_clip(vidl_clip_sptr  clip);


protected:
  // Data Members
  vcl_list<vidl_clip_sptr> clip_;


  //----------------------------
  // Iterator definition
  //----------------------------
public :

  class frame_iterator
     {
     //
     // This class is an iterator for the frames in a vidl_movie.
     // One should think of a vidl_movie as being a list of vidl_frames.
     // The frame_iterator allows one to access the individual frames.
     // Thus, dereferencing the frame iterator gives a vidl_frame.
     // i.e. if we have frame_iterator it, then *it is a vidl_frame.
     // and it-> accesses the members of vidl_frame.
     //
     // The subclassing off Image is just done so that we can define
     // the method GetSection to get a buffer or pixels to display.
     //
     // Correct way of running through all the frames of a movie is
     // for (vidl_movie::frame_iterator frame = movie.begin();
     //      frame != movie.end();
     //      frame++)
     //
     // Also look at test.C for other examples

     private :
        vidl_movie_sptr movie_;
        int frame_number_;

     public :
        // Constructors / destructors
        frame_iterator (vidl_movie_sptr movie, int frame_number) :
           movie_(movie), frame_number_(frame_number) {}

        frame_iterator (vidl_movie_sptr movie) :
           movie_(movie), frame_number_(0) {}

        frame_iterator (const frame_iterator &fr) :
           movie_(fr.movie_), frame_number_(fr.frame_number_)
           {}

        ~frame_iterator () {}

        // Assigning one iterator to another
        frame_iterator &operator = (const frame_iterator &fr)
           {
           // We do not want to bother about copying the image
           movie_ = fr.movie_;
           frame_number_ = fr.frame_number_;
           return *this;
           }

        frame_iterator &operator = (int n)
           {
           // Just set the frame_number
           frame_number_ = n;
           return *this;
           }

        // Incrementing the frame number
        frame_iterator &operator ++ ()
           {
           frame_number_ ++;
           return *this;
           }

        frame_iterator &operator -- ()
           {
           frame_number_ --;
           return *this;
           }

        frame_iterator &operator + (int n)
           {
           frame_number_ += n;
           return *this;
           }

        frame_iterator &operator - (int n)
           {
           frame_number_ -= n;
           return *this;
           }

        // Treating as a vidl_frame_sptr
        operator vidl_frame_sptr ()
           {
           if (frame_number_ < 0 || frame_number_ >= movie_->length())
              return vidl_frame_sptr(0);
           vidl_frame_sptr frame = movie_->get_frame(frame_number_);
           return frame;
           }

        vidl_frame_sptr operator -> () { return (vidl_frame_sptr) *this; }

        // Comparison against other iterators
        friend bool operator == (const frame_iterator &fr1,
                                 const frame_iterator &fr2)
           { return fr1.frame_number_ == fr2.frame_number_; }

        friend bool operator != (const frame_iterator &fr1,
                                 const frame_iterator &fr2)
           { return fr1.frame_number_ != fr2.frame_number_; }

        friend bool operator <(const frame_iterator&fr1,
                               const frame_iterator&fr2)
           { return fr1.frame_number_ < fr2.frame_number_ ; }
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
        bool operator == (int n)
           { return frame_number_ == n; }

        bool operator != (int n)
           { return frame_number_ != n; }

        bool operator < (int n)
           { return frame_number_ < n; }

        bool operator > (int n)
           { return frame_number_ > n; }

        bool operator <= (int n)
           { return frame_number_ <= n; }

        bool operator >= (int n)
           { return frame_number_ >= n; }

        // Data member access
        int current_frame_number () { return frame_number_; }
        vidl_movie_sptr get_movie ()  { return movie_; }
     };

   //---------------------------------------------------------------------
   // Methods that return iterators
   frame_iterator first()
      {
      frame_iterator fr (this, 0);
      return fr;
      }

   frame_iterator last()
      {
      frame_iterator fr (this, this->length()-1);
      return fr;
      }

   frame_iterator begin()
      {
      return first();
      }

   frame_iterator end()
      {
      return last()+1;
      }
};
#endif // vidl_movie_h
