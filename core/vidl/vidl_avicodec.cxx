//:
// \file

#include "vidl_avicodec.h"
#include <vidl/vidl_frame.h>
#include <vidl/vidl_movie.h>

#include <vcl_iostream.h>
#include <vcl_cstdio.h>

#include <vul/vul_file.h>
#include <vil/vil_image_view.h>
#include <vil/vil_flip.h>

// Microsoft files
#include <windows.h>
#include <vfw.h>
#include <windowsx.h> // for _fmemset function

// To improve performance, we could use a Look Up Table
// instead of computing  255/32
#define RGB16R(rgb)     ((((UINT)(rgb) >> 10) & 0x1F) * 255u / 31u)
#define RGB16G(rgb)     ((((UINT)(rgb) >> 5)  & 0x1F) * 255u / 31u)
#define RGB16B(rgb)     ((((UINT)(rgb) >> 0)  & 0x1F) * 255u / 31u)

//-----------------------------------------------------------------------------
vidl_avicodec::vidl_avicodec()
{
  avi_get_frame_ = NULL;
  avi_stream_ = NULL;
  avi_file_ = NULL;
  encoder_type = ASKUSER;
  _fmemset(&opts, 0, sizeof(AVICOMPRESSOPTIONS));
  encoder_options_valid=false;
}

//-----------------------------------------------------------------------------
vidl_avicodec::~vidl_avicodec()
{
  if (avi_get_frame_)
    AVIStreamGetFrameClose(avi_get_frame_); //This needs to be done, but only after a reading.
  if (avi_stream_)
    AVIStreamRelease(avi_stream_);
  if (avi_file_)
    AVIFileRelease(avi_file_);
  AVIFileExit();
}

//-----------------------------------------------------------------------------
bool vidl_avicodec::read_header()
{
  AVIFileInfo(avi_file_, &avi_file_info_, sizeof(AVIFILEINFO));
  AVIStreamInfo(avi_stream_, &avi_stream_info_, sizeof(AVISTREAMINFO));

  //Read in Width
  set_width(avi_file_info_.dwWidth);
  if (avi_stream_info_.rcFrame.right != avi_file_info_.dwWidth+avi_stream_info_.rcFrame.left)
  {
    vcl_cerr << "vidl_avicodec::read_header width size screwed up\n"
             <<   "          size of avi file : " << avi_file_info_.dwWidth
             << "\n          size of the stream : "
             << avi_stream_info_.rcFrame.right-avi_stream_info_.rcFrame.left
             << vcl_endl;
  }

  //Read in Height
  set_height(avi_file_info_.dwHeight);
  if (avi_stream_info_.rcFrame.bottom != avi_file_info_.dwHeight+avi_stream_info_.rcFrame.top)
  {
    vcl_cerr << "vidl_avicodec::read_header Height size screwed up\n"
             <<   "          size of avi file : " << avi_file_info_.dwHeight
             << "\n          size of the stream : "
             << avi_stream_info_.rcFrame.bottom-avi_stream_info_.rcFrame.top
             << vcl_endl;
  }

  //Read in Length
  set_number_frames(avi_file_info_.dwLength);

  // treat as blocked even though it isn't
  //SetBlockSizeX(width());
  //SetBlockSizeY(height());
  //set_widthBlocks((int)((width()+GetBlockSizeX()-1)/GetBlockSizeX()));
  //set_heightBlocks((int)((height()+GetBlockSizeY()-1)/GetBlockSizeY()));
  return true;
}

//-----------------------------------------------------------------------------
bool vidl_avicodec::write_header()
{
   vcl_fprintf(stderr, "vidl_avicodec::write_header Not implemented.\n");
   return false;
}

//-----------------------------------------------------------------------------
vil_image_view_base_sptr vidl_avicodec::get_view(
                                int position, //!< position of the frame in the stream
                                int x0, //!< starting x
                                int xs, //!< row size
                                int y0, //!< starting y
                                int ys ) const //!< col size
{
  byte* DIB = (byte*) AVIStreamGetFrame(avi_get_frame_, position);

  WORD BitsPerPixel = ((LPBITMAPINFOHEADER)DIB)->biBitCount;
#ifdef DEBUG
  vcl_cout << "Number of bits : " << BitsPerPixel << "  Number of bytes : " << get_bytes_pixel() << vcl_endl;

  WORD ColorsUsed = ((LPBITMAPINFOHEADER)DIB)->biClrUsed;
  vcl_cout << "Number of colors used : " << ColorsUsed << vcl_endl;
  if (ColorsUsed!=0) vcl_cout << "Not sure we can handle the stream if ColorsUsed!=0\n";
#endif

  //For the moment
  if ((BitsPerPixel!=16) && (BitsPerPixel!=24))
  {
    vcl_cerr << "vidl_avicodec : Don't know how to process a "
             << BitsPerPixel<< " bits per pixel AVI File.\n";
    return 0;
  }

  DIB += *(LPDWORD)DIB;
  byte* StartDIB = DIB;
  // Size of a row in number of bytes in the DIB structure
  // (a row contains a multiple of 4 bytes)
  int line_length = (width()*BitsPerPixel+31)/32*4;

  vxl_byte *db = new byte[xs*ys*get_bytes_pixel()]; // current output data
  vxl_byte *ib = db;

  // The byte swapping below is probabily unnecessary - use vil_flip_ud instead - FIXME

  // Store the DIB datas into ib (db).
  // Note : DIB is a flipped upside down
  switch (BitsPerPixel)
  {
   case 24:
    for (int j=height()-y0-1; j>=height()-y0-ys; j--)
    {
      DIB = StartDIB+ (j*line_length)+x0*(BitsPerPixel/8);
      for (int i=0; i<xs; i++)
      {
        *db = *(DIB+2);
        *(db+1) = *(DIB+1);
        *(db+2) = *(DIB);
        db+=3;
        DIB+=3;
      }
    }
    break;
   case 16:
    for (int j=height()-y0-1; j>=height()-y0-ys; j--)
    {
      DIB = StartDIB + (j*line_length) + x0*(BitsPerPixel/8);
      for (int i=0; i<xs; i++)
      {
        WORD* Pixel16 = (WORD*) DIB; // the current 16 bits pixel
        *db     = (BYTE) RGB16R(*Pixel16);
        *(db+1) = (BYTE) RGB16G(*Pixel16);
        *(db+2) = (BYTE) RGB16B(*Pixel16);
        db+=3;
        DIB+=2;
      }
    }
    break;
   default:
    vcl_cerr << "vidl_avicodec : Don't know how to process a "
             << BitsPerPixel << " bits per pixel AVI File.\n";
  } // end switch Bits per pixel

  vil_image_view_base_sptr image_sptr(new vil_image_view<vxl_byte>(ib, xs, ys, 3,
                                                                   3, ((xs*3+3)& -4), 1));
  return image_sptr;
}

//: put_view not implemented yet.
// we may need to change make_dib to
// be able to put a section different
// of the entire frame.
bool vidl_avicodec::put_view( int /*position*/,
                              const vil_image_view_base &/*im*/,
                              int /*x0*/, int /*y0*/)
{
  vcl_cerr << "vidl_avicodec::put_section not implemented\n";
  return false;
}

//-----------------------------------------------------------------------------
//: Probe the file fname, open it as an AVI file. If it works, return true, false otherwise.

bool vidl_avicodec::probe(const char* fname)
{
  int modenum = OF_READ | OF_SHARE_DENY_WRITE;
  AVIFileInit();
  if (AVIFileOpen(&avi_file_, fname, modenum, 0L)==0)
  {
    // The file was opened with success
    // So, release it
    AVIFileRelease(avi_file_);
    // Make sure it's set to NULL otherwise, delete() tries to release it again.
    avi_file_=NULL;
    // and return sucess
    return true;
  }

  return false;
}

//: Returns a clone of 'this' into which the given avi file is loaded.
//  This function creates a clone of 'this' (in order to allow
//  loading multiple avi videos at once) and loads the avi
//  into the cloned codec. The cloned codec is the one that is returned
//  by this function.
vidl_codec_sptr vidl_avicodec::load(const char* fname, char mode)
{
  vidl_avicodec *cloned_avi_codec=new vidl_avicodec;

  if (!cloned_avi_codec->load_avi(fname,mode)) return NULL;

  vidl_codec_sptr codec(cloned_avi_codec);

  return codec;
}

bool vidl_avicodec::load_avi(const char* fname, char mode)
{
  int modenum = OF_READ;
  DWORD videostreamcode = 0x73646976; // corresponds to char string "vids"

  switch(mode) {
   case 'r':
    modenum = OF_READ | OF_SHARE_DENY_WRITE;
    break;
   case 'w':
    modenum = OF_READWRITE;
    break;
  }

  AVIFileInit();
  AVIFileOpen(&avi_file_, fname, modenum, 0L);

  // only support first video stream
  if (AVIFileGetStream(avi_file_, &avi_stream_, videostreamcode, 0) != AVIERR_OK)
    vcl_cerr << "[vidl_avicodec: no stream 0]";

  if (!avi_file_ || !avi_stream_) return false;

  avi_get_frame_ = AVIStreamGetFrameOpen(avi_stream_, NULL);

  if (!read_header()) {
    vcl_fprintf(stderr, "vidl_avicodec: error reading header\n");
    return false;
  }

  set_format('L');
  set_image_class('C');
  set_name(vul_file::basename(fname).c_str());
  set_description(fname);

  // Open the first frame
  byte* DIB = (byte*) AVIStreamGetFrame(avi_get_frame_, 0);

  if ( ! DIB )
    return false;

  // Get the number of bits per pixel
  // and check the validity of width and height
  WORD BitsPerPixel = ((LPBITMAPINFOHEADER)DIB)->biBitCount;
  LONG iwidth = ((LPBITMAPINFOHEADER)DIB)->biWidth;
  LONG iheight = ((LPBITMAPINFOHEADER)DIB)->biHeight;

  if (width() != iwidth)
  {
    vcl_cerr << "vidl_avicodec::load ohoh, width of the first frame is different from the one specified for the avifile\n"
             << "          Movie width set with the first frame\n";
    set_width(iwidth);
  }

  if (height() != iheight)
  {
    vcl_cerr << "vidl_avicodec::load ohoh, height of the first frame is different from the one specified for the avifile\n"
             << "          Movie height set with the first frame\n";
    set_height(iheight);
  }

  // The movie will have 24 bits per pixel no matter of what
  // We can read 16 bits per pixel avi, but this will be
  // store in 24 bits per pixel frames
  set_bits_pixel(24); // set_bits_pixel(BitsPerPixel);

  //For the moment, we'll process 8 bits later
  if ((BitsPerPixel!=16) && (BitsPerPixel!=24))
  {
    vcl_cerr << "vidl_avicodec : Don't know how to process a "
             << BitsPerPixel << " bits per pixel AVI File.\n";
    return false;
  }

  return true;
}

bool vidl_avicodec::save(vidl_movie* movie, const char* fname)
{
  PAVIFILE avi_file;
  AVISTREAMINFO avi_stream_info;
  PAVISTREAM avi_stream = NULL;

  HRESULT hr;

  AVIFileInit();

  //
  // Open the movie file for writing....
  //
  hr = AVIFileOpen(&avi_file,               // returned file pointer
                   fname,                   // file name
                   OF_WRITE | OF_CREATE,    // mode to open file with
                   NULL);                   // use handler determined
  // from file extension....
  if (hr != AVIERR_OK) {
    vcl_cerr << "vidl_avicodec : Could not open the file " << fname << " for writing.\n";
    if (hr == AVIERR_BADFORMAT)
      vcl_cerr << "vidl_avicodec : The file couldn't be read, indicating a corrupt file or an unrecognized format.\n";
    if (hr== AVIERR_MEMORY)
      vcl_cerr << "vidl_avicodec : The file could not be opened because of insufficient memory.\n";
    if (hr== AVIERR_FILEREAD)
      vcl_cerr << "vidl_avicodec : A disk error occurred while reading the file.\n";
    if (hr== AVIERR_FILEOPEN)
      vcl_cerr << "vidl_avicodec : A disk error occurred while opening the file.\n";
    if (hr== REGDB_E_CLASSNOTREG)
    {
      vcl_cerr << "vidl_avicodec : According to the registry, the type of file"
               << " specified in AVIFileOpen does not have a handler to process it.\n"
               << "vidl_avicodec : This is usually the case when the file name given does not have the .avi extension\n";
    }
    return false;
  }

  // Fill in the header for the video stream....
  //
  // The video stream will run in 30ths of a second....

  _fmemset(&avi_stream_info, 0, sizeof(avi_stream_info));
  avi_stream_info.fccType                = streamtypeVIDEO;// stream type
  avi_stream_info.fccHandler             = 0;
  avi_stream_info.dwScale                = 1;
  avi_stream_info.dwRate                 = movie->frame_rate();
  avi_stream_info.dwLength               = movie->length();
  avi_stream_info.dwSuggestedBufferSize  = movie->width()*movie->height()*3;//codec->get_bytes_pixel();
  SetRect(&avi_stream_info.rcFrame, 0, 0,             // rectangle for stream
          (int) movie->width(),
          (int) movie->height());

  // And create the stream;
  hr = AVIFileCreateStream(avi_file,                // file pointer
                           &avi_stream,             // returned stream pointer
                           &avi_stream_info);       // stream header
  if (hr != AVIERR_OK) {
    vcl_cerr << "vidl_avicodec : Could not create the avi stream.\n";
    return false;
  }

  if (encoder_type==ASKUSER)
  {
    // Compression mode
    AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};

    if (!AVISaveOptions(NULL, 0, 1, &avi_stream, (LPAVICOMPRESSOPTIONS FAR *) &aopts))
    {
      vcl_cerr << "vidl_avicodec : AVI Saving Cancelled.\n";
      return false;
    }

    encoder_options_valid=true;
  }

  char *fcc=(char *)&(opts.fccHandler);

  vcl_cout << "Compressor options:\n"
           << "fccHandler       = " << fcc[0] << "','" << fcc[1] << "','" << fcc[2] << "','" << fcc[3] << "'\n"
           << "key frame every  = " << opts.dwKeyFrameEvery << vcl_endl
           << "quality          = " << opts.dwQuality << vcl_endl
           << "flags            = " << opts.dwFlags  << vcl_endl;
  if (opts.dwFlags & AVICOMPRESSF_DATARATE)
    vcl_cout << "                   AVICOMPRESSF_DATARATE\n";
  if (opts.dwFlags & AVICOMPRESSF_INTERLEAVE)
    vcl_cout << "                   AVICOMPRESSF_INTERLEAVE\n";
  if (opts.dwFlags & AVICOMPRESSF_KEYFRAMES)
    vcl_cout << "                   AVICOMPRESSF_KEYFRAMES\n";
  if (opts.dwFlags & AVICOMPRESSF_VALID)
    vcl_cout << "                   AVICOMPRESSF_VALID\n";
  vcl_cout << "lpFormat         = " << opts.lpFormat << vcl_endl
           << "cbFormat         = " << opts.cbFormat << vcl_endl
           << "lpParms          = " << opts.lpParms << vcl_endl
           << "cbParms          = " << opts.cbParms << vcl_endl
           << "dwInterleaveEvery= " << opts.dwInterleaveEvery << vcl_endl;

  PAVISTREAM avi_stream_compressed = NULL;
  hr = AVIMakeCompressedStream(&avi_stream_compressed, avi_stream, &opts, NULL);
  if (hr != AVIERR_OK)
    return false;

  // Set the stream format
  {
    LPBITMAPINFOHEADER lpbi =
      (LPBITMAPINFOHEADER)GlobalLock(make_dib(movie->get_frame(0), 24));
    if (!lpbi)
    {
      vcl_cerr << "vidl_avicodec : DIB (Device Independent Bitmap) creation failed.\n";
      return false;
    }
    hr = AVIStreamSetFormat(avi_stream_compressed, 0,
                            lpbi,           // stream format
                            lpbi->biSize +   // format size
                            lpbi->biClrUsed * sizeof(RGBQUAD));
    if (hr != AVIERR_OK)
    {
      vcl_cerr << "vidl_avicodec : Could not set the AVI stream format.\n"
               << "                The chosen compression mode may not be installed well.\n";
      return false;
    }
  }

  // Write every frame
  int i = 0;
  for (vidl_movie::frame_iterator pframe = movie->begin();
       pframe <= movie->last();
       ++pframe, ++i)
  {
    LPBITMAPINFOHEADER lpbi =
      (LPBITMAPINFOHEADER)GlobalLock(make_dib(pframe, 24));
    if (!lpbi)
    {
      vcl_cerr << "vidl_avicodec : DIB (Device Independent Bitmap) creation failed.\n"
               << "vidl_avicodec : Frame number " << i << vcl_endl;
      return false;
    }

    int time = i; // codec->GetT ...
    hr = AVIStreamWrite(avi_stream_compressed,
                        time,
                        1, // Number of samples to write
                        (LPBYTE) lpbi +               // pointer to data
                        lpbi->biSize +
                        lpbi->biClrUsed * sizeof(RGBQUAD),
                        lpbi->biSizeImage,
                        AVIIF_KEYFRAME,                        // flags....
                        NULL,
                        NULL);
    if (hr != AVIERR_OK)
    {
      vcl_cerr << "vidl_avicodec : Could not write to the AVI stream.\n";
      return false;
    }
  }

  if (avi_stream)
    AVIStreamRelease(avi_stream);
  if (avi_stream_compressed)
    AVIStreamRelease(avi_stream_compressed);
  if (avi_file)
    AVIFileRelease(avi_file);
  AVIFileExit();

  // Everything was OK
  return true;
}

#if 0
//: Converts a microsoft four cc code to a 32 bit unsigned int.
unsigned int vidl_avicodec::fccHandlerCoder(char c0, char c1, char c2, char c3)
{
  unsigned int code;

  char *codeStr=(char *)(&code);
  codeStr[0]=c0;
  codeStr[1]=c1;
  codeStr[2]=c2;
  codeStr[3]=c3;

  return code
}
#endif // 0

//: This function sets the encoder that is internally used to create the
//  AVI. Using this function avoids the windows dialog asking
//  the user for the compressor.
//  Depending on the choosen encoder, the parameters of opts are set by this
//  function.
//
//  @param encoder
//     if encoder==ASKUSER      a windows dialog is used to choose the encoder
//     if encoder==USEPREVIOUS  the options previously obtained from a call
//                              to choose_encoder() with or without ASKUSER is used.
//                              If not valid options have been obtained, UNCOMPRESSED is used.
//     if encoder==UNCOMPRESSED Video is saved uncompressed.
void vidl_avicodec::choose_encoder(AVIEncoderType encoder)
{
  encoder_type=encoder;

  if (encoder_type==ASKUSER ||
      (encoder_type==USEPREVIOUS && encoder_options_valid))
    return;

  encoder_options_valid=true;

  switch(encoder_type)
  {
   case USEPREVIOUS:
   case UNCOMPRESSED:
    opts.fccType=streamtypeVIDEO;
    opts.fccHandler=mmioFOURCC('D','I','B',' ');
    opts.dwKeyFrameEvery=0;
    opts.dwQuality=0;
    opts.dwFlags=AVICOMPRESSF_VALID;
    opts.lpFormat=0;
    opts.cbFormat=0;
    opts.lpParms=0;
    opts.cbParms=0;
    opts.dwInterleaveEvery=0;
    break;
   case CINEPACK:
    opts.fccType=streamtypeVIDEO;
    opts.fccHandler=mmioFOURCC('c','v','i','d');
    opts.dwKeyFrameEvery=0;
    opts.dwQuality=10000;
    opts.dwFlags=AVICOMPRESSF_VALID;
    opts.lpFormat=0;
    opts.cbFormat=0;
    //The pointer to lpParms should actually point to four bytes of
    //memory containing the data 0x726c6f63, however, it seems to
    //work with all zeros also.
    //opts.lpParms = (LPVOID *)GlobalAlloc(NULL, 4);
    //opts.cbParms = 4;
    // *((long*)opts.lpParms) = 0x726c6f63;
    opts.lpParms=0;
    opts.cbParms=0;
    opts.dwInterleaveEvery=0;
    break;
   default:
    encoder_options_valid=false;
  }
}

//: Create a DIB (Device Independent Bitmap) from a frame.
// (Note : make_dib is not guaranteed to work with bits!=24 for the moment)
HANDLE  vidl_avicodec::make_dib(vidl_frame_sptr frame, UINT bits)
{
  // 1st, Get the datas from the video frame
  vil_image_view<vxl_byte> image(frame->get_view());
  byte* TjSection = (byte*) image.top_left_ptr();

  // 2nd, Copy the array of bytes (and transform it),
  // so it is usable by a 'windows' BitMap
  //

  // The lenght of a row must be a multiple of 4 bytes
  // for windows bitmaps, so we will format them this way
  int line_length = (frame->width()*bits+31)/32 * 4;

  int data_size = line_length*frame->height()*(bits/8);
  // Create an array of bytes to receive the transformed datas
  byte* newbits = new byte[data_size];
  byte* db = (byte*) newbits;
  int i,j;
  for (i=0; i<data_size; i++)
  {
    *db = 0;
    ++db;
  }
 
  if(image.nplanes() == 3){
    image = vil_flip_ud(image);
    for (j=0; j<frame->height(); ++j)
    {
      //db = TjSection+ (j*frame->width())*frame->get_bytes_pixel();
      byte* DIB = newbits + j*line_length;
      for (i=0; i<frame->width(); ++i, DIB+=3, db+=3) {
        *DIB = image(i,j,2);
        *(DIB+1) = image(i,j,1);
        *(DIB+2) = image(i,j,0);
      }
    }
  }
  else{
    
    // The byte swapping below is probabily unnecessary - use vil_flip_ud instead - FIXME
    
    // Store the TargetJr data in a Bitmap way, DIB is a flipped upside down
    switch (frame->get_bytes_pixel())
    {
    case 3:
      for (j=frame->height()-1; j>=0;j--)
      {
        db = TjSection+ (j*frame->width())*frame->get_bytes_pixel();
        byte* DIB = newbits + (frame->height()-j-1)*line_length;
        for (i=0; i<frame->width(); ++i, DIB+=3, db+=3) {
          *DIB = *(db+2);
          *(DIB+1) = *(db+1);
          *(DIB+2) = *(db);
        }
      }
      break;
    case 1:
      for (j=frame->height()-1; j>=0;j--)
      {
        db = TjSection+ (j*frame->width())*frame->get_bytes_pixel();
        byte* DIB = newbits + (frame->height()-j-1)*line_length;
        for (i=0; i<frame->width(); ++i, DIB+=3, db+=1) {
          *DIB = *(db);
          *(DIB+1) = *(db);
          *(DIB+2) = *(db);
        }
      }
      break;
    default:
      vcl_cerr << "vidl_avicodec : Don't know how to deal with "
        << frame->get_bytes_pixel() << " bytes per pixel.\n";
      
    } // end switch byte per pixel
    
  }

  // 3rd, Create the Bitmap and stick the datas in it
  HDC hdc = GetDC(NULL);
  HBITMAP hbitmap;
  if (!(hbitmap = CreateCompatibleBitmap(hdc,frame->width(),frame->height())))
  {
    vcl_cerr << "vidl_avicodec : Could not create a compatible bitmap for frame.\n";
    return NULL;
  }
  BITMAP bitmap;
  GetObject(hbitmap,sizeof(BITMAP),&bitmap);
  int wColSize = sizeof(RGBQUAD)*((bits <= 8) ? 1<<bits : 0);
  int dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
    (DWORD)(UINT)line_length*(DWORD)(UINT)bitmap.bmHeight;

  //
  // Allocate room for a DIB and set the LPBI fields
  //
  HANDLE hdib = GlobalAlloc(GHND,dwSize);
  if (!hdib)
    return hdib;

  LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

  lpbi->biSize = sizeof(BITMAPINFOHEADER);
  lpbi->biWidth = bitmap.bmWidth;
  lpbi->biHeight = bitmap.bmHeight;
  lpbi->biPlanes = 1;
  lpbi->biBitCount = (WORD) bits;
  lpbi->biCompression = BI_RGB;
  lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
  lpbi->biXPelsPerMeter = 0;
  lpbi->biYPelsPerMeter = 0;
  lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;
  lpbi->biClrImportant = 0;

  hdc = CreateCompatibleDC(NULL); // Create Device Context

   // Put the bits in the bitmap
  int error_code = SetDIBits(hdc,hbitmap,0,bitmap.bmHeight,newbits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
  if (!error_code)
  {
    vcl_cerr << "vidl_avicodec : Could set the bits in the BitMap.\n";
    return NULL;
  }

  //
  // Get the bits from the bitmap and stuff them after the LPBI
  //
  LPBYTE lpBits = (LPBYTE)(lpbi+1)+wColSize;

  // 4th, Stick the bits in the DIB
  error_code = GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,lpBits,(LPBITMAPINFO)lpbi, DIB_RGB_COLORS);
  if (!error_code)
  {
    vcl_cerr << "vidl_avicodec : Could set the bits in the DIB (Device Independent Bitmap).\n";
    return NULL;
  }

  // Fix this if GetDIBits messed it up....
  lpbi->biClrUsed = (bits <= 8) ? 1<<bits : 0;

  // Delete ressources
  DeleteBitmap(hbitmap);
  delete [] newbits;
  ReleaseDC(NULL,hdc);
  GlobalUnlock(hdib);

  return hdib;
}
