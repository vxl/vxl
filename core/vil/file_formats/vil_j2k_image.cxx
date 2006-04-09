// vil_j2k: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_j2k_image.h"

#include <NCSFile.h>

#include <vcl_algorithm.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h> // for ceil()
#include <vcl_limits.h>
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view.h>

#include "NCSJPCVilIOStream.h"
#include <NCSTypes.h>

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
  vil_j2k_file_format::make_output_image(vil_stream* /*vs*/,
                                          unsigned /*nx*/,
                                          unsigned /*ny*/,
                                          unsigned /*nplanes*/,
                                          enum vil_pixel_format /*format*/)
{
  //write not supported
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
    assert( 0 );
    return NCSCT_UINT8;
  }
}

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
    assert( 0 );
    return VIL_PIXEL_FORMAT_UNKNOWN;
  }
}

////////////////////////////////////////////////////
//                vil_j2k_image
////////////////////////////////////////////////////

vil_j2k_image::vil_j2k_image( const vcl_string& fileOrUrl )
  : vil_image_resource(),
    mFileResource( 0 ),
    mMaxLocalDimension( 5000 ), //default value
    mMaxRemoteDimension( 640 ), //default value
    mRemoteFile( false )
{
  mFileResource = new CNCSFile();
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
    mFileResource( 0 ),
    mMaxLocalDimension( 5000 ), //default value
    mMaxRemoteDimension( 640 ), //default value
    mRemoteFile( false )
{
  mFileResource = new CNCSFile();
  CNCSJPCVilIOStream* cstr = new CNCSJPCVilIOStream();
  cstr->Open( is );

  if ( (static_cast<CNCSJP2FileView*>(mFileResource))->Open( cstr ) != NCS_SUCCESS ) {
    mFileResource = 0;
    return;
  }
}

vil_j2k_image::~vil_j2k_image()
{
  if ( mFileResource ){
    mFileResource->Close( true );
  }
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

vil_image_view_base_sptr vil_j2k_image::get_copy_view_decimated(unsigned sample0,
                                                                unsigned num_samples,
                                                                unsigned line0,
                                                                unsigned numLines,
                                                                double i_factor,
                                                                double j_factor) const
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
  INT32* bandMap = (INT32*) malloc(sizeof(UINT32) * nBands );
  for ( int i = 0 ; i < nBands ; i++ ) { bandMap[i] = i; }

  //this guards us from returning an image that is too big for the computer's memory
  //(or would take too long to download in the remote case).
  //We don't want infinite hangs or application crashes.
  unsigned int maxDim = mRemoteFile ? mMaxRemoteDimension : mMaxLocalDimension;
  unsigned int output_width  = (unsigned int) ( ((double)num_samples)/i_factor );
  unsigned int output_height = (unsigned int) ( ((double)numLines)/j_factor );
  if ( output_width > maxDim || output_height > maxDim ){
    unsigned int biggestDim = vcl_max( output_width, output_height );
    double zoomFactor = ((double)maxDim) / ((double)biggestDim);
    output_width  = (unsigned int) ( ((double)output_width)  * zoomFactor );
    output_height = (unsigned int) ( ((double)output_height) * zoomFactor );
  }

  //set the view to be that specified by the function's input parameters
  //note that we don't want ECW to do any scaling for us.  That's why
  //the box created by (sample0,line0) and (sample0+num_samples-1,line0+numLines-1) is made to be exactly num_samplesXnumLines.

  NCSError setViewError = mFileResource->SetView( nBands, bandMap, output_width, output_height,
                                                  (INT32)sample0, (INT32)line0, (INT32)(sample0+num_samples-1), (INT32)(line0+numLines-1) );
  if ( setViewError != NCS_SUCCESS ){
    free( bandMap );
    return 0;
  }

  //number of samples times the bytes per sample in each band
  double bitsPerSample                 = mFileResource->GetFileInfo()->pBands[0].nBits;
  unsigned int bytesPerSample          = (unsigned int) vcl_ceil( bitsPerSample / 8.0 );
  unsigned int singleBandLineSizeBytes = output_width * bytesPerSample;
  unsigned int allBandLineSizeBytes    = singleBandLineSizeBytes * nBands;
  unsigned int dataPtrSizeBytes        = allBandLineSizeBytes * output_height;
  //void* data_ptr = malloc( dataPtrSizeBytes );
  vil_memory_chunk_sptr data_ptr = new vil_memory_chunk( dataPtrSizeBytes, convertType( mFileResource->GetFileInfo()->eCellType ) );
  void** linePtrPtr = (void**)malloc( nBands * sizeof( int* /*all pointers have same size, so eg char* would work too*/ ) );
  //now read all the lines that we want
  for ( unsigned int currLine = 0 ; currLine < output_height ; currLine++ ){
    for (int currBand = 0 ; currBand < nBands ; currBand++ ){
      linePtrPtr[currBand] = (void*) ( ((char*)data_ptr->data()) + currLine * allBandLineSizeBytes + currBand * singleBandLineSizeBytes );
    }
    NCSEcwReadStatus readStatus = mFileResource->ReadLineBIL( mFileResource->GetFileInfo()->eCellType, nBands, linePtrPtr, 0);
    if ( readStatus != NCSECW_READ_OK ){
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
  macro(VIL_PIXEL_FORMAT_FLOAT , float );
  macro(VIL_PIXEL_FORMAT_DOUBLE , double );
#undef macro
  default:
    assert( 0 );
    //"Unknown vil data type." );
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
  setMaxImageDimension( vcl_numeric_limits< unsigned int >::max(), remote );
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
  vil_image_view_base_sptr view = j2k_image->get_copy_view_decimated(i0, ni, j0, nj, i_factor, j_factor);
  delete j2k_image;
  return view;
}
