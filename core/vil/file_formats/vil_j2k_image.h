//:
// \file
// vil_j2k: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.
// Write capability added by J. Mundy, April 2009
// Do not remove the following notice
// Modifications approved for public Release, distribution unlimited
// DISTAR Case 14074
//
#ifndef VIL_J2K_H
#define VIL_J2K_H

#include <vil/vil_image_resource.h>
#include <vil/vil_file_format.h>
#include <NCSECWClient.h>
class CNCSFile;
class vil_stream;
class CNCSJPCVilIOStream;
class vil_j2k_file_format : public vil_file_format
{
 public:
  vil_j2k_file_format() : compression_ratio_(1){}
  virtual char const *tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream *vs);
  virtual
    vil_image_resource_sptr make_output_image(vil_stream* vs,
                                              unsigned ni,
                                              unsigned nj,
                                              unsigned nplanes,
                                              enum vil_pixel_format format);
  //: This compression ratio is set as a target for the ermapper compression algorithm. The default is lossless compression (ratio == 1).
  void set_compression_ratio(unsigned ratio)
    {compression_ratio_ = ratio;}
 private:
  unsigned compression_ratio_;
};

//:
//  Class capable of reading JPEG2000 Part I files and ECW (ER Mapper's proprietary format)
//  image files.  They can either be local or hosted on an Image Web Server.  Either way, you
//  just pass in the file path or url (eg. "ecwp://www.earthetc.com/images/australia/Sydney.ecw")
//  to the ctor.  The call get_copy_view() to get the image data that you want.  The class efficiently
//  handles reading large images (Terrabytes) -- and can read image portions without loading the whole file
//  into memory.
//
//  Because the source image can be really big, it is possible (through the get_copy_view() API) to
//  ask for more data than you can handle in memory -- or more than you download for the remote case.
//  That is why the setMaxImageDimension() guard is in place.  You can set a max image dimension
//  for both remote and local files (different values for each).  Then if you call get_copy_view()
//  asking for an image that is too big, get_copy_view() will scale down the image to the max dimension
//  you specified.  It does this silently -- perhaps we could/should change this?
//
//  Note that, in order to use this class, you need to use cmake to configure VXL to link against
//  Er Mapper's freely available ECW JPEG 2000 SDK (http://ermapper.com/downloads/sdks.aspx#16).
//
// Writing implemented by J. Mundy April 2009
// The SDK appears not to support float or double pixel types only integral
// forms; both signed or unsigned are supported. Another caveat is that the
// actual compression rate is only loosely related to the targeted compression
// rate specified upon creating the resource
//
class vil_j2k_image : public vil_image_resource
{
 public:
  //:
  //  \param filelOrUrl: can either be a local file (eg. /home/beavis/file1.jp2) or
  //  it can be a url to a file hosted on an Image Web Server (eg.
  //  ecwp://www.earthetc.com/images/australia/Sydney.ecw or
  //  ecwp://www.earthetc.com/images/usa/1metercalif.ecw
  vil_j2k_image( const std::string& fileOrUrl );
  //:
  //  Read a jpeg 2000 image from a stream containing either a raw j2k codestream
  //  or a jp2 file stream.  is' current position needs to be pointing at the beginning of
  //  one of these two things.  In other words, the beginning of the stream must contain one
  //  of the two signatures:
  //  - Hex( FF 4F )                                -- (codestream)
  //  - Hex( 00 00 00 0C 6A 50 20 20 0D 0A 87 0A )  -- jp2 file
  //
  //  Note that some references state that jp2 files start with ( 00 00 00 0C 6A 50 1A 1A 0D 0A 87 0A )
  //  [note the 1A 1A difference].  I believe this was changed between the last draft and the final version
  //  of ISO/IEC 155444-1 (JPEG standard).  I have never seen a real jp2 file with this old signature (including
  //  the official jpeg conformance test files)
  //
  //  also note: Don't use is while I'm trying to use it... it'll screw us both up.
  vil_j2k_image( vil_stream* is );
  vil_j2k_image( vil_stream* vs, unsigned ni, unsigned nj,
                 unsigned nplanes, enum vil_pixel_format format,
                 unsigned compression_ratio = 1);
  ~vil_j2k_image();
  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;
  virtual enum vil_pixel_format pixel_format() const;
  //: returns j2k
  char const* file_format() const;
  virtual bool get_property(char const* /* tag */, void* /* property_value */ = 0) const
    { return false; }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  virtual vil_image_view_base_sptr get_copy_view_decimated(unsigned i0, unsigned ni,
                                                           unsigned j0, unsigned nj,
                                                           double i_factor, double j_factor) const;
  virtual vil_image_view_base_sptr
    get_copy_view_decimated_by_size(unsigned i0, unsigned ni,
                                    unsigned j0, unsigned nj,
                                    unsigned int output_width,
                                    unsigned int output_height) const;


  vil_image_view_base_sptr  get_copy_view () const
    { return get_copy_view( 0, ni(), 0, nj() ); }

  //:
  //  Call this after construction to see if you can get valid data from me.
  //  If this returns false, then this image is of no use to you
  bool is_valid() const { return mFileResource != 0; }

  //:
  //  When calling get_copy_view(), the function will scale down the output image_view
  //  so that neither dimension (x or y) is greater than widthOrHeight.  This feature
  //  is here to protect you in the case that your code asks for an excessively large
  //  image that will crash your program.  You can turn this checking off with \sa unsetMaxImageDimension()
  //  By default, this value is set to 5000.
  void setMaxImageDimension( unsigned int widthOrHeight, bool remote = false );
  //:
  //  Call this if you don't want get_copy_view() to do size checking.
  //  Be warned that jpeg 2000 codestreams can be really big, so you could
  //  cause a program crash.
  void unsetMaxImageDimension( bool remote = false );

  //:
  //  Static function that can be used to decode a JPEG2000 codestream
  //  or file (jp2 file).  The stream must start at vs' current position.
  static vil_image_view_base_sptr s_decode_jpeg_2000( vil_stream* vs,
                                                      unsigned i0, unsigned ni,
                                                      unsigned j0, unsigned nj,
                                                      double i_factor, double j_factor );
  static vil_image_view_base_sptr
    s_decode_jpeg_2000_by_size( vil_stream* vs,
                                unsigned i0, unsigned ni,
                                unsigned j0, unsigned nj,
                                unsigned int output_width,
                                unsigned int output_height );


  //:
  //  Encode an entire image by loading the input resource from stream
  //  and compressing the input line by line by extracting an image view
  //  of a block of lines at a time, thus works for arbitrarily large images.
  //  The num_lines_block parameter is the number of image rows in the
  //  block which is read into memory from the resource
  static bool s_encode_jpeg2000(vil_stream* vs,
                                const char* out_filename,
                                unsigned compression_ratio = 1,
                                unsigned num_lines_block = 1024,
                                bool verbose = false);

  //: encode an entire image by loading the input resource from file
  //  Uses the stream-based method of the same name
  static bool s_encode_jpeg2000(const char* in_filename,
                                const char* out_filename,
                                unsigned compression_ratio = 1,
                                unsigned num_lines_block = 1024,
                                bool verbose = false);

  //: JPEG2K compress the data from the full image view and insert in resource
  // The compression ratio is determined when the resource is created by
  // the file_format class.
  virtual bool put_view(const vil_image_view_base& im);

  //: JPEG2K compress the data from an image view and insert in resource
  // This method cannot be implemented because the J2K SDK does not support
  // compressing arbitrary views. Only inserting strictly successive lines
  // is allowed.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0){return false;}

  //: JPEG2K compress by inserting an image row (line) at a time.
  //  When the full image has been inserted, the resource is closed
  //  and is no longer valid. The lines must be inserted in strict row order.
  bool put_line(const vil_image_view_base& im);

  //: Check that a view will fit into the data at the given offset.
  virtual bool view_fits(const vil_image_view_base& im, unsigned i0, unsigned j0);
 protected:
  //: The ermapper file
  CNCSFile* mFileResource;
  //: The ermapper stream
  CNCSJPCVilIOStream* mStr;
  //:
  //  \sa setMaxImageDimension and \sa unsetMaxImageDimension
  //
  //  if this equals std::numeric_limits<unsigned int>::max(), then this feature is turned off
  //  Of course I'm ignored if mRemoteFile is true
  unsigned int mMaxLocalDimension;
  //:
  //  Same as \sa mMaxLocalDimension but applies to remote files.
  //  This is typically a smaller number because of the speed concerns of downloading
  //  a very largeimage
  unsigned int mMaxRemoteDimension;
  //:file is remote
  bool mRemoteFile;
  //: band information array
  NCSFileBandInfo* mBandinfo;
  //: the file information block
  NCSFileViewFileInfoEx* mFinfo;
  //: the current line being written for compression
  unsigned line_index_;
};

#endif // VIL_J2K_H
