#ifndef vidl_vil1_codec_h
#define vidl_vil1_codec_h
//:
// \file
// \author Nicolas Dano, september 1999
//
// \verbatim
//  Modifications
//   June 2000 Julien ESTEVE          Ported from TargetJr
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
// \endverbatim

#include <vcl_cstring.h>
#include <vidl_vil1/vidl_vil1_codec_sptr.h>
#include <vbl/vbl_ref_count.h>

class vidl_vil1_movie;
class vidl_vil1_image_list_codec;
class vidl_vil1_mpegcodec;
class vidl_vil1_avicodec;

//: Base for video coder/decoder
//   A vidl_vil1_codec is a pure virtual class defining the codecs of video
//   See also vidl_vil1_io and vidl_vil1_image_list_codec and vidl_vil1_avicodec
class vidl_vil1_codec :  public vbl_ref_count
{
  vidl_vil1_codec(vidl_vil1_codec const&) : vbl_ref_count() {}
 public:

  // Constructors/Initializers/Destructors-------------------------------------
  vidl_vil1_codec() { clear_strings(); }
  virtual ~vidl_vil1_codec() {}

  //=====================================================
  // Casting methods -- lets use a standard form for these, namely
  // CastToXXX, where XXX is the subclass
  virtual vidl_vil1_image_list_codec* castto_vidl_vil1_image_list_codec() { return 0; }
  virtual vidl_vil1_mpegcodec* castto_vidl_vil1_mpegcodec() { return 0; }
  virtual vidl_vil1_avicodec* castto_vidl_vil1_avicodec() { return 0; }
  // Data Control--------------------------------------------------------------

  inline void set_number_frames(int n = 0) { numberframes = n; }
  inline void set_name(const char* n = "") { delete[] name;name=new char[vcl_strlen(n)+1];vcl_strcpy(name,n); }
  inline void set_description(const char* d = "")
  { delete[] description;description=new char[vcl_strlen(d)+1];vcl_strcpy(description,d); }
  inline void set_format(char f = '\0') { format = f; }
  inline void set_image_class(char t = 'M') { Class = t; }
  inline void set_bits_pixel(int d = 0) { B = d; }
  inline void set_width(int x = 0)  { X = x; }
  inline void set_height(int y = 0) { Y = y; }
  inline void set_size_z(int z = 1) { Z = z; }
  inline void set_size_t(int t = 1) { T = t; }

  // Data Access---------------------------------------------------------------

  inline int length() const { return numberframes; }
  inline const char* get_name() const  { return (name)?name:""; }
  inline const char* get_description() const { return (description)?description:""; }

  inline char get_image_class()const { return Class; }
  inline char get_format() const     { return format; }
  inline int  width() const          { return X; }
  inline int  height() const         { return Y; }
  inline int  get_bits_pixel() const { return B; }
  inline int  get_bytes_pixel()const { return (B+7)/8; }

  virtual bool get_section(
        int position,
        void* ib,
        int x0,
        int y0,
        int width,
        int heigth) const = 0;

  virtual int put_section(
        int position,
        void* ib,
        int x0,
        int y0,
        int xs,
        int ys) = 0;

  virtual const char* type() = 0;

  // IO

  //: Try to load fname, and if successful, return the codec that did it
  virtual vidl_vil1_codec_sptr load(const char* fname, char mode = 'r' ) = 0;

  //: Take a vidl_vil1_movie, and save in the format of this codec.
  virtual bool save(vidl_vil1_movie* movie, const char* fname) = 0;

  //: Return true if fname looks like something we can read.
  virtual bool probe(const char* fname) = 0;

  //: Perform any operations required to close down the codec.
  // This will typically be called just before program exit.
  virtual void close() {};

 private:

  inline void clear_strings() { name = description = date_time = NULL; }

  char*      name;             //!< Video Name
  char*      description;      //!< Video Descriptor
  char*      date_time;        //!< Date/Time Stamp
  char       format;           //!< Video format
  char       Class;            //!< Video class
  int        B;                //!< Pixel Precision
  int        X,Y,Z,T;          //!< Frame Size (width,height,up,time)
  int        numberframes;     //!< Length of the sequence
};

#endif // vidl_vil1_codec_h
