#include <algorithm>
#include <cmath>
#include <limits>
#include <cstdlib>
#include "vil_j2k_image.h"
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

#include <NCSFile.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_open.h>
#include <vil/vil_new.h>
#include "NCSJPCVilIOStream.h"
#include <NCSTypes.h>

// Fix me - need to add UINT64 and INT64 - JLM
// Also note, float and double are defined but not handled by SDK 3.3
// they are reinterpreted as INT32 and INT64
NCSFileBandInfo bandInfo( const vil_pixel_format& vilType )
{
  NCSFileBandInfo info;
  switch ( vil_pixel_format_component_format( vilType ) )
  {
    case VIL_PIXEL_FORMAT_UINT_32:{
      info.nBits = sizeof(vxl_uint_32)*8;
      info.bSigned = std::numeric_limits<vxl_uint_32>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_INT_32:{
      info.nBits = sizeof(vxl_int_32)*8;
      info.bSigned = std::numeric_limits<vxl_int_32>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_UINT_16:{
      info.nBits =  sizeof(vxl_uint_16)*8;
      info.bSigned =  std::numeric_limits<vxl_uint_16>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_INT_16:{
      info.nBits = sizeof(vxl_int_16)*8;
      info.bSigned = std::numeric_limits<vxl_int_16>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_BYTE:{
      info.nBits = sizeof(vxl_byte)*8;
      info.bSigned = std::numeric_limits<vxl_byte>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_SBYTE:{
      info.nBits = sizeof(vxl_sbyte)*8;
      info.bSigned = std::numeric_limits<vxl_sbyte>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_FLOAT:{
      info.nBits = sizeof(float)*8;
      info.bSigned = std::numeric_limits<float>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_DOUBLE:{
      info.nBits = sizeof(double)*8;
      info.bSigned = std::numeric_limits<double>::is_signed;
      info.szDesc = 0;
      return info;
    }
    case VIL_PIXEL_FORMAT_BOOL:
    case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
    case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
    case VIL_PIXEL_FORMAT_UINT_64:
    case VIL_PIXEL_FORMAT_INT_64:
    case VIL_PIXEL_FORMAT_UNKNOWN:
    default:{
      assert( 0 );
      info.nBits = 0; info.bSigned = false; info.szDesc = 0;
      return info;
    }
  }
}

//--------------------------------------------------------------------------------
// class vil_j2k_file_format

static char const j2k_string[] = "j2k";

char const* vil_j2k_file_format::tag() const
{
  return j2k_string;
}

vil_image_resource_sptr  vil_j2k_file_format::make_input_image(vil_stream *vs)
{
  vil_j2k_image* im = new vil_j2k_image( vs );
  if ( !im->is_valid() ) {
    delete im;
    im = 0;
  }
  return im;
}


vil_image_resource_sptr
vil_j2k_file_format::make_output_image(vil_stream* vs,
                                       unsigned ni,
                                       unsigned nj,
                                       unsigned nplanes,
                                       enum vil_pixel_format format)
{
  vil_j2k_image* j2k_img = new vil_j2k_image(vs, ni, nj, nplanes, format,
                                             compression_ratio_);
  if (j2k_img->is_valid())
    return static_cast<vil_image_resource*>(j2k_img);
  return 0;
}

NCSEcwCellType convertType( const vil_pixel_format& vilType )
{
  switch ( vil_pixel_format_component_format( vilType ) ) {
    case VIL_PIXEL_FORMAT_UINT_64: return NCSCT_UINT64;
    case VIL_PIXEL_FORMAT_INT_64: return NCSCT_INT64;
    case VIL_PIXEL_FORMAT_UINT_32: return NCSCT_UINT32;
    case VIL_PIXEL_FORMAT_INT_32: return NCSCT_INT32;
    case VIL_PIXEL_FORMAT_UINT_16: return NCSCT_UINT16;
    case VIL_PIXEL_FORMAT_INT_16: return NCSCT_INT16;
    case VIL_PIXEL_FORMAT_BYTE: return NCSCT_UINT8;
    case VIL_PIXEL_FORMAT_SBYTE: return NCSCT_INT8;
    case VIL_PIXEL_FORMAT_FLOAT: return NCSCT_IEEE4;
    case VIL_PIXEL_FORMAT_DOUBLE: return NCSCT_IEEE8;
    case VIL_PIXEL_FORMAT_BOOL:
    case VIL_PIXEL_FORMAT_COMPLEX_FLOAT:
    case VIL_PIXEL_FORMAT_COMPLEX_DOUBLE:
    case VIL_PIXEL_FORMAT_UNKNOWN:
    default:
      assert(0); return NCSCT_UINT8;
  }
}

// Note the J2K SDK defines IEEE4 and IEEE8 but they are not
// handled as float or double, they are reinterpreted as INT32 and INT64
vil_pixel_format convertType( const NCSEcwCellType& ecwType )
{
  switch ( ecwType ) {
    case NCSCT_UINT64: return VIL_PIXEL_FORMAT_UINT_64;
    case NCSCT_INT64: return VIL_PIXEL_FORMAT_INT_64;
    case NCSCT_UINT32: return VIL_PIXEL_FORMAT_UINT_32;
    case NCSCT_INT32: return VIL_PIXEL_FORMAT_INT_32;
    case NCSCT_UINT16: return VIL_PIXEL_FORMAT_UINT_16;
    case NCSCT_INT16: return VIL_PIXEL_FORMAT_INT_16;
    case NCSCT_UINT8: return VIL_PIXEL_FORMAT_BYTE;
    case NCSCT_INT8: return VIL_PIXEL_FORMAT_SBYTE;
    case NCSCT_IEEE4: return VIL_PIXEL_FORMAT_FLOAT;
    case NCSCT_IEEE8: return VIL_PIXEL_FORMAT_DOUBLE;
    default:
      assert(0); return VIL_PIXEL_FORMAT_UNKNOWN;
  }
}

////////////////////////////////////////////////////
//                vil_j2k_image
////////////////////////////////////////////////////

vil_j2k_image::vil_j2k_image( const std::string& fileOrUrl )
  : vil_image_resource(),
    mFileResource( new CNCSFile() ),
    mStr(0),
    mMaxLocalDimension( 5000 ), //default value
    mMaxRemoteDimension( 640 ), //default value
    mRemoteFile( false ),
    mFinfo(0),
    mBandinfo(0),
    line_index_(0)
{
  if ( mFileResource->Open( (char*)fileOrUrl.c_str(), false, false ) != NCS_SUCCESS ) {
    mFileResource = 0;
    return;
  }
  if ( fileOrUrl.substr( 0, 7 ) == "ecwp://" ||
       fileOrUrl.substr( 0, 7 ) == "ECWP://" )
    {
      mRemoteFile = true;
    }
}

vil_j2k_image::vil_j2k_image( vil_stream* is )
  : vil_image_resource(),
    mFileResource( new CNCSFile() ),
    mStr(new CNCSJPCVilIOStream()),
    mMaxLocalDimension( 5000 ), //default value
    mMaxRemoteDimension( 640 ), //default value
    mRemoteFile( false ),
    mFinfo(0),
    mBandinfo(0),
    line_index_(0)
{
  mStr->Open( is );

  if ( (static_cast<CNCSJP2FileView*>(mFileResource))->Open( mStr ) != NCS_SUCCESS ) {
    mFileResource = 0;
    return;
  }
}

vil_j2k_image::vil_j2k_image( vil_stream* vs, unsigned ni, unsigned nj,
                              unsigned nplanes, enum vil_pixel_format format,
                              unsigned compression_ratio)
  : vil_image_resource(),
    mFileResource( new CNCSFile()),
    mStr(new CNCSJPCVilIOStream()),
    mMaxLocalDimension( 5000 ), //default value
    mMaxRemoteDimension( 640 ), //default value
    mRemoteFile( false ),
    mFinfo(new NCSFileViewFileInfoEx()),
    line_index_(0)
{
  mBandinfo = new NCSFileBandInfo[nplanes];
  CNCSError Error;
  for (unsigned i = 0; i<nplanes; ++i)
    mBandinfo[i] = bandInfo(format);

  //String names for each band, should specialize according to color space
  mBandinfo[0].szDesc ="grey";
  if (nplanes==3) {
    mBandinfo[0].szDesc = "Red";
    mBandinfo[1].szDesc = "Green";
    mBandinfo[2].szDesc = "Blue";
  }
  NCSEcwCellType t = convertType(format);
  NCSFileViewFileInfoEx finfo=*mFinfo;
  finfo.pBands = mBandinfo;
  finfo.nSizeX = ni;
  finfo.nSizeY = nj;
  finfo.nBands = nplanes;
  finfo.eCellType = t;
  finfo.nCompressionRate = compression_ratio;
  finfo.eCellSizeUnits = ECW_CELL_UNITS_METERS;
  finfo.fCellIncrementX = 1.0;
  finfo.fCellIncrementY = 1.0;
  finfo.fOriginX = 0.0;
  finfo.fOriginY = 0.0;
  finfo.szDatum = "RAW";
  finfo.szProjection = "RAW";
  finfo.fCWRotationDegrees = 0.0;
  if (nplanes ==1)
    finfo.eColorSpace = NCSCS_GREYSCALE;
  else if (nplanes ==3)
    finfo.eColorSpace = NCSCS_sRGB;
  else {
    delete mFileResource;
    mFileResource = 0;
  }
  Error = mFileResource->SetFileInfo(finfo);
  if (Error != NCS_SUCCESS) {
    if (mFileResource)
      delete mFileResource;
    mFileResource = 0;
  }
  Error = mStr->Open(vs, true);
  if (Error != NCS_SUCCESS) {
    if (mFileResource)
      delete mFileResource;
    mFileResource = 0;
  }
  CNCSJP2FileView* fview = static_cast<CNCSJP2FileView*>(mFileResource);
  Error = fview->Open(static_cast<CNCSJPCIOStream*>(mStr));
  if (Error != NCS_SUCCESS) {
    if (mFileResource)
      delete mFileResource;
    mFileResource = 0;
    return;
  }
}

vil_j2k_image::~vil_j2k_image()
{
  if ( mFileResource ) {
    mFileResource->Close( true );
  }
  if (mStr)
    delete mStr;
  if (mBandinfo)
    delete mBandinfo;
  if (mFinfo)
    delete mFinfo;
}

unsigned vil_j2k_image::nplanes() const
{
  assert( mFileResource );
  return mFileResource->GetFileInfo()->nBands;
}

unsigned vil_j2k_image::ni() const
{
  assert( mFileResource );
  return mFileResource->GetFileInfo()->nSizeX;
}

unsigned vil_j2k_image::nj() const
{
  assert( mFileResource );
  return mFileResource->GetFileInfo()->nSizeY;
}

enum vil_pixel_format vil_j2k_image::pixel_format() const
{
  assert( mFileResource );
  return convertType( mFileResource->GetFileInfo()->eCellType );
}

char const* vil_j2k_image::file_format() const
{
  return "j2k";
}

vil_image_view_base_sptr
vil_j2k_image::get_copy_view_decimated(unsigned sample0,
                                       unsigned num_samples,
                                       unsigned line0,
                                       unsigned numLines,
                                       double i_factor,
                                       double j_factor) const
{
  return get_copy_view_decimated_by_size(sample0,
                                         num_samples,
                                         line0,
                                         numLines,
                                         (unsigned int)(((double)num_samples)/i_factor),
                                         (unsigned int)(((double)numLines)/j_factor));
}

vil_image_view_base_sptr
vil_j2k_image::get_copy_view_decimated_by_size(unsigned sample0,
                                               unsigned num_samples,
                                               unsigned line0,
                                               unsigned numLines,
                                               unsigned int output_width,
                                               unsigned int output_height) const
{
  if ( !( mFileResource ) ||
       !( ( sample0 + num_samples - 1 ) < ni() &&
          ( line0 + numLines - 1 ) < nj() ) )
  {
    return 0;
  }

  //we want all bands mapped in the same order as they come in the input file
  //eg. bandMap = {0,1,2,3...nBands}
  INT32 nBands = nplanes();
  INT32* bandMap = (INT32*) std::malloc(sizeof(UINT32) * nBands );
  for ( int i = 0 ; i < nBands ; i++ ) { bandMap[i] = i; }

  //this guards us from returning an image that is too big for the computer's memory
  //(or would take too long to download in the remote case).
  //We don't want infinite hangs or application crashes.
  unsigned int maxDim = mRemoteFile ? mMaxRemoteDimension : mMaxLocalDimension;
  if ( output_width > maxDim || output_height > maxDim ) {
    unsigned int biggestDim = (std::max)( output_width, output_height );
    double zoomFactor = ((double)maxDim) / ((double)biggestDim);
    output_width  = (unsigned int) ( ((double)output_width)  * zoomFactor );
    output_height = (unsigned int) ( ((double)output_height) * zoomFactor );
  }

  //set the view to be that specified by the function's input parameters
  //note that we don't want ECW to do any scaling for us.  That's why
  //the box created by (sample0,line0) and (sample0+num_samples-1,line0+numLines-1) is made to be exactly num_samplesXnumLines.

  NCSError setViewError = mFileResource->SetView( nBands, bandMap, output_width, output_height,
                                                  (INT32)sample0, (INT32)line0, (INT32)(sample0+num_samples-1), (INT32)(line0+numLines-1) );
  if ( setViewError != NCS_SUCCESS ) {
    free( bandMap );
    return 0;
  }

  //number of samples times the bytes per sample in each band
  double bitsPerSample                 = mFileResource->GetFileInfo()->pBands[0].nBits;
  unsigned int bytesPerSample          = (unsigned int) std::ceil( bitsPerSample / 8.0 );
  unsigned int singleBandLineSizeBytes = output_width * bytesPerSample;
  unsigned int allBandLineSizeBytes    = singleBandLineSizeBytes * nBands;
  unsigned int dataPtrSizeBytes        = allBandLineSizeBytes * output_height;
  //void* data_ptr = std::malloc( dataPtrSizeBytes );
  vil_memory_chunk_sptr data_ptr = new vil_memory_chunk( dataPtrSizeBytes, convertType( mFileResource->GetFileInfo()->eCellType ) );
  void** linePtrPtr = (void**)std::malloc( nBands * sizeof( int* /*all pointers have same size, so eg char* would work too*/ ) );
  //now read all the lines that we want
  for ( unsigned int currLine = 0 ; currLine < output_height ; currLine++ ) {
    for (int currBand = 0 ; currBand < nBands ; currBand++ ) {
      linePtrPtr[currBand] = (void*) ( ((char*)data_ptr->data()) + currLine * allBandLineSizeBytes + currBand * singleBandLineSizeBytes );
    }
    NCSEcwReadStatus readStatus = mFileResource->ReadLineBIL( mFileResource->GetFileInfo()->eCellType, nBands, linePtrPtr, 0);
    if ( readStatus != NCSECW_READ_OK ) {
      free( bandMap );
      free( linePtrPtr );
      return 0;
    }
  }

  //free our temp resources
  free( bandMap );
  free( linePtrPtr );

  vil_image_view_base_sptr view = 0;
  //now create our vil_image_view
  //note that float and double are defaulted since the J2K SDK doesn't
  //implement these types properly
  switch ( vil_pixel_format_component_format( data_ptr->pixel_format() ) )
    {
#define macro( F, T ) \
  case F: \
    view = new vil_image_view< T > ( data_ptr, reinterpret_cast<T*>(data_ptr->data()), \
                                     output_width, output_height, nBands, 1, output_width*nBands, output_width); \
    break
      macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte );
      macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte );
      macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 );
      macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 );
      macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 );
      macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 );
      macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 );
      macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 );
      macro(VIL_PIXEL_FORMAT_BOOL , bool );
#undef macro
    default:
      std::cerr << "Pixel format not supported by ERMapper SDK\n";
      assert( 0 );
      break;
    }

  return view;
}

vil_image_view_base_sptr vil_j2k_image::get_copy_view(unsigned sample0,
                                                      unsigned num_samples,
                                                      unsigned line0,
                                                      unsigned numLines ) const
{
  return get_copy_view_decimated( sample0, num_samples, line0, numLines, 1.0, 1.0 );
}

void vil_j2k_image::unsetMaxImageDimension( bool remote )
{
#undef max
  setMaxImageDimension( std::numeric_limits< unsigned int >::max(), remote );
}

void vil_j2k_image::setMaxImageDimension( unsigned int widthOrHeight, bool remote )
{
  if ( remote ) {
    mMaxRemoteDimension = widthOrHeight;
  } else {
    mMaxLocalDimension = widthOrHeight;
  }
}

vil_image_view_base_sptr vil_j2k_image::s_decode_jpeg_2000( vil_stream* vs,
                                                            unsigned i0, unsigned ni,
                                                            unsigned j0, unsigned nj,
                                                            double i_factor, double j_factor )
{
  vil_j2k_image* j2k_image = new vil_j2k_image(vs);
  //remove limit by default, since vil is not typically used remotely
  //but more commonly with large image files - JLM Jan 07, 2012
  j2k_image->unsetMaxImageDimension();
  vil_image_view_base_sptr view = j2k_image->get_copy_view_decimated(i0, ni, j0, nj, i_factor, j_factor);
  delete j2k_image;
  return view;
}

vil_image_view_base_sptr
vil_j2k_image::s_decode_jpeg_2000_by_size( vil_stream* vs,
                                           unsigned i0, unsigned ni,
                                           unsigned j0, unsigned nj,
                                           unsigned int output_width,
                                           unsigned int output_height )
{
  vil_j2k_image* j2k_image = new vil_j2k_image(vs);
  vil_image_view_base_sptr view = j2k_image->get_copy_view_decimated_by_size(i0, ni, j0, nj, output_width, output_height);
  delete j2k_image;
  return view;
}

template < class T >
static bool write_line_BIL(vil_memory_chunk_sptr& chunk,
                           unsigned ni, unsigned nplanes, unsigned istep,
                           unsigned planestep, unsigned bytes_per_pixel,
                           CNCSFile* f_resource, NCSEcwCellType t)
{
  T* cdata = reinterpret_cast<T*>(chunk->data());
  T** line_ptr = new T*[nplanes];
  for (unsigned p = 0; p<nplanes; ++p)
    line_ptr[p] = new T[ni*bytes_per_pixel];

  for (unsigned p = 0; p<nplanes; ++p) {
    T* wline = line_ptr[p];
    for (unsigned i = 0; i<ni; ++i) {
      *(wline+i) = *(cdata+ i*istep + p*planestep);
    }
  }
  bool good = true;
  void ** outbuf = reinterpret_cast<void**>(line_ptr);
  CNCSError writeError  = f_resource->WriteLineBIL(t, nplanes, outbuf);
  if ( writeError != NCS_SUCCESS ) good = false;

  for (unsigned p = 0; p<nplanes; ++p)
    delete [] line_ptr[p];
  delete [] line_ptr;
  return good;
}


//: JPEG2K compress by inserting an image row (line) at a time
//  When the full image has been inserted, call put_line with
// image_row == nj(). This call causes the resource to be closed
// and is no longer valid. The lines must be inserted in strict row order.
bool
vil_j2k_image::put_line(const vil_image_view_base& im)
{
  if (!mFileResource)
    return false;
  vil_pixel_format format = this->pixel_format();
  unsigned ni = this->ni(), nj = this->nj(), nplanes = this->nplanes();
  if (line_index_>=nj) {
    mFileResource->Close(true);
    if (mFileResource)
      delete mFileResource;
    mFileResource = 0;
    return true;
  }
  unsigned bytes_per_pixel = 0;
  NCSEcwCellType t = convertType(format);
  vil_memory_chunk_sptr chunk;
  //now write out the image line
  //note that float and double are defaulted since the J2K SDK doesn't
  //implement these types properly
  switch ( vil_pixel_format_component_format( format ) )
    {
#define macro( F, T )\
  case F: {\
   bytes_per_pixel = sizeof(T); \
   const vil_image_view<T>& view = static_cast<const vil_image_view<T>&>(im); \
   chunk = view.memory_chunk(); \
   if (!write_line_BIL<T>(chunk, ni, nplanes, view.istep(), view.planestep(),\
                         bytes_per_pixel, mFileResource, t)) \
    return false; \
  } \
  break
      macro(VIL_PIXEL_FORMAT_BYTE , vxl_byte );
      macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte );
      macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 );
      macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 );
      macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 );
      macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 );
      macro(VIL_PIXEL_FORMAT_BOOL , bool );
#undef macro
    default:
      std::cerr << "Pixel format not supported by ERMapper SDK\n";
      assert( 0 );
      break;
    }
  ++line_index_;
  return true;
}

//: JPEG2K compress the view and of the full image and insert in resource
//  The file is closed after putting the view into the resource
//  and becomes an invalid resource.
bool vil_j2k_image::
put_view(const vil_image_view_base& im)
{
  if (!this->view_fits(im, 0, 0))
    return false;
  if (!mFileResource)
    return false;
  unsigned ni= im.ni(), nj = im.nj();
  vil_image_resource_sptr mem_res =
    vil_new_image_resource_of_view(im);
    vil_image_view_base_sptr view;
  for (unsigned j = 0; j<nj; ++j) {
    view = mem_res->get_copy_view(0, ni, j, 1);
    if (!this->put_line(*view)) return false;
  }
  return true;
}

//: Check that a view will fit into the data at the given offset.
bool vil_j2k_image::
view_fits(const vil_image_view_base& im, unsigned i0, unsigned j0)
{
  unsigned ni_view = im.ni(), nj_view = im.nj(), nplanes_view = im.nplanes();
  return i0+1 < ni_view
      && j0+1 < nj_view
      && ni_view <= this->ni()
      && nj_view <= this->nj()
      && nplanes_view <= this->nplanes();
}

//:
//  Encode an entire image by loading the input resource from stream
//  and compressing the input line by line by extracting an image view
//  of a block of lines at a time, thus works for arbitrarily large images.
//  The num_lines_block parameter is the number of image rows in the
//  block which is read into memory from the resource
bool vil_j2k_image::s_encode_jpeg2000(vil_stream* vs,
                                      const char* out_filename,
                                      unsigned compression_ratio,
                                      unsigned num_lines_block,
                                      bool verbose)
{
  vil_image_resource_sptr in_res = vil_load_image_resource_raw(vs);
  if (!in_res)
    return false;
  unsigned ni = in_res->ni(), nj = in_res->nj(), nplanes = in_res->nplanes();
  vil_pixel_format format = in_res->pixel_format();
  vil_stream* os = vil_open(out_filename, "w");
  if (!vs) return false;
  vil_j2k_file_format fmt;
  fmt.set_compression_ratio(compression_ratio);
  vil_image_resource_sptr res = fmt.make_output_image(os, ni, nj, nplanes,
                                                      format);
  if (!res) return false;
  vil_j2k_image* j2k_img = reinterpret_cast<vil_j2k_image*>(res.ptr());

  //number of full blocks in image height
  unsigned n_blocks = nj/num_lines_block;
  unsigned jb = 0;
  for (unsigned b = 0; b<n_blocks; b++, jb += num_lines_block)
  {
    // read a block from the file: width = ni, height = num_lines_block
    vil_image_view_base_sptr block_view = in_res->get_view(0, ni, jb, num_lines_block);
    if (!block_view) return false;

    //wrap the view in a memory resident resource
    vil_image_resource_sptr block_res =
      vil_new_image_resource_of_view(*block_view);

    //compress the block, line by line
    for (unsigned j = 0; j<num_lines_block; ++j) {
      vil_image_view_base_sptr line_view =
        block_res->get_copy_view(0, ni, j, 1);
      if (!j2k_img->put_line(*line_view)) return false;
      if (verbose)
        if (j%100 == 0) //output a dot every 100 lines
          std::cout << '.';
    }
  }
  //output the remaining lines left over after loading block-sized chunks
  unsigned remaining_lines = nj-jb;
  if (remaining_lines) {
    vil_image_view_base_sptr residual_view =
      in_res->get_view(0, ni, jb, remaining_lines);
    vil_image_resource_sptr residual_res =
      vil_new_image_resource_of_view(*residual_view);
    vil_image_view_base_sptr view;
    for (unsigned j = 0; j<remaining_lines; ++j) {
      view = residual_res->get_copy_view(0, ni, j, 1);
      if (!j2k_img->put_line(*view)) return false;
      if (verbose)
        if (j%100 == 0) //output a dot every 100 lines
          std::cout << '.';
    }
  }
  if (verbose) std::cout << '\n';
  return true;
}

bool vil_j2k_image::s_encode_jpeg2000(const char* in_filename,
                                      const char* out_filename,
                                      unsigned compression_ratio,
                                      unsigned num_lines_block,
                                      bool verbose )
{
  vil_stream* vs = vil_open(in_filename);
  vs->ref();
  bool success =
    vil_j2k_image::s_encode_jpeg2000(vs, out_filename,
                                     compression_ratio,
                                     num_lines_block,
                                     verbose);
  vs->unref();
  return success;
}
