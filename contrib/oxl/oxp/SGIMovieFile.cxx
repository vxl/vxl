#ifdef __GNUC__
#pragma implementation
#endif

#include "SGIMovieFile.h"
#include "SGIMovieFilePrivates.h"

#include <vcl_fstream.h>
#include <vcl_strstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>

#include <vul/vul_printf.h>
#include <oxp/JPEG_Decompressor.h>
#include <vxl_config.h>

/////////////////////////////////////////////////////////////////////////////
static int get_u16(vcl_istream& f);
static int get_u32(vcl_istream& f);

/////////////////////////////////////////////////////////////////////////////

SGIMovieFile::SGIMovieFile(char const* f)
{
  p = new SGIMovieFilePrivates(f);
}

SGIMovieFile::~SGIMovieFile()
{
  delete p;
}

int SGIMovieFile::GetLength()
{
  return p->video_indices[0].size();
}

vil_image SGIMovieFile::GetImage(int)
{
  return 0;
}


int SGIMovieFile::GetSizeX(int)
{
  return p->width;
}

int SGIMovieFile::GetSizeY(int)
{
  return p->height;
}

int SGIMovieFile::GetBitsPixel()
{
  return 24; // fixme
}

bool SGIMovieFile::IsInterlaced()
{
  return p->interlaced != 0;
}

int SGIMovieFile::GetFrameOffset(int frame_index)
{
  return p->video_indices[0][frame_index].offset;
}

int SGIMovieFile::GetFrameSize(int frame_index)
{
  return p->video_indices[0][frame_index].size;
}

bool SGIMovieFile::HasFrame(int frame_index)
{
  return (0 <= frame_index) && (frame_index < GetLength());
}

SGIMovieFilePrivates::SGIMovieFilePrivates(char const* fn):
  filename(fn)
{
  // File header is like:
  //   V2:
  //   byte MOVI[4];
  //   short v2; // 2
  //   short width;
  //   short height;
  //   short pad;
  //
  //   V3:
  //   byte MOVI[4];
  //   word version;
  //   word pad;

  vcl_ifstream f(fn);
  char buf[4];
  f.read(buf,4);
  if (vcl_strncmp(buf,"MOVI",4) != 0) {
    vul_printf(vcl_cerr, "SGIMovieFile: Not a movie file, magic = [%c%c%c%c]\n", buf[0], buf[1], buf[2], buf[3]);
    version = 0;
    return;
  }

  // Read version
  version = get_u16(f);

  if (version == 2) {
    // Old format
    if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "SGIMovieFile: Old format, version = %d\n", version);
    width = get_u16(f);
    height = get_u16(f);
    /* int pad = */ get_u16(f);
  } else {
    int version1 = get_u16(f);
    version = (version << 16) + version1;
    if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "SGIMovieFile: New format, version = %d\n", version);

    /* int pad = */ get_u32(f);
  }

//   SGIMV_Variables glob;
//   SGIMV_Variables audio[glob.__NUM_A_TRACKS];
//   SGIMV_Variables video[glob.__NUM_I_TRACKS];
//

  glob = new SGIMV_Variables(f);
  if (MovieFileInterface::verbose) glob->print(vcl_cerr);

  int NUM_I_TRACKS = glob->get_int("__NUM_I_TRACKS");
  int NUM_A_TRACKS = glob->get_int("__NUM_A_TRACKS");
  if (MovieFileInterface::verbose)
    vul_printf(vcl_cerr, "SGIMovieFile: Number of audio/video tracks: %d/%d\n", NUM_A_TRACKS, NUM_I_TRACKS);

  // Load Audio and video info
  for(int i = 0; i < NUM_A_TRACKS; ++i)
    audio.push_back(SGIMV_Variables(f));

  for(int i = 0; i < NUM_I_TRACKS; ++i)
    video.push_back(SGIMV_Variables(f));

  // Assign vars
  width = video[0].get_int("WIDTH");
  height = video[0].get_int("HEIGHT");
  interlaced = video[0].get_int("INTERLACING");
  compression = video[0].data["COMPRESSION"];

  if (MovieFileInterface::verbose) {
    // Print
    for(int i = 0; i < NUM_A_TRACKS; ++i)
      audio[i].print(vcl_cerr);

  // Video SGIMV_Variables:
  //       COMPRESSION = "1": MVC1, "2": RGB32, "10": JPEG, "MVC2"
  //               FPS = 25.000000
  //            HEIGHT = 576
  //       INTERLACING = 1
  //       ORIENTATION = 1100
  //           PACKING = 1001
  //      PIXEL_ASPECT = 1.000000
  //         Q_SPATIAL = 0.750000
  //        Q_TEMPORAL = 0.750000
  //             WIDTH = 768
  //       __DIR_COUNT = 750
  //
    for(int i = 0; i < NUM_I_TRACKS; ++i)
      video[i].print(vcl_cerr);
  }

  // Load indices
  for(int i = 0; i < NUM_A_TRACKS; ++i)
    audio_indices.push_back(SGIMV_FrameIndexArray(f, audio[i].get_int("__DIR_COUNT")));

  for(int i = 0; i < NUM_I_TRACKS; ++i) {
    int nframes = video[i].get_int("__DIR_COUNT");
    video_indices.push_back(SGIMV_FrameIndexArray(f, nframes));
    SGIMV_FrameIndexArray& frame_indices = video_indices[i];
    field_indices.push_back(vcl_vector<int>(nframes * 2, 0));
    // Fill every second field index.
    for(int ff = 0; ff < nframes; ++ff)
      field_indices[i][ff*2] = frame_indices[ff].offset;
  }

  SGIMV_FrameIndexArray& video_index = video_indices[0];
  if (MovieFileInterface::verbose) {
    for(unsigned i = 0; i < video_index.size(); ++i) {
      if (i > 10 && i < 740)
        continue;
      vul_printf(vcl_cerr, "SGIMovieFile: Frame %3d", i);
      if (NUM_A_TRACKS > 0)
        vul_printf(vcl_cerr, ",  Audio at 0x%08x [%1$9d], size %5d", audio_indices[0][i].offset, audio_indices[0][i].size);
      if (NUM_I_TRACKS > 0)
        vul_printf(vcl_cerr, ",  Video at 0x%08x [%1$9d], size %6d", video_indices[0][i].offset, video_indices[0][i].size);
      vul_printf(vcl_cerr, "\n");
    }

#ifdef GNU_LIBSTDCXX_V3
    vul_printf(vcl_cerr, "SGIMovieFile: Final position, after reading header, is %d\n", std::streamoff(f.tellg()));
#else
    vul_printf(vcl_cerr, "SGIMovieFile: Final position, after reading header, is %d\n", f.tellg());
#endif
  }
}

bool SGIMovieFile::GetFrame(int frame_index, void* buffer)
{
  if (p->compression != "2" && p->compression != "10") {
    vul_printf(vcl_cerr, "SGIMovieFile: Can't decompress ``%s'' format images\n", p->compression.c_str());
    return false;
  }

  // int n = p->video_indices[0][frame_index].size;
  int s = p->video_indices[0][frame_index].offset;

  // Need to open file every time...
  FILE * fp = vcl_fopen(p->filename.c_str(), "r");
  if (!fp) {
    vcl_cerr << "SGIMovieFile: File has disappeared\n";
    return false;
  }

  // Seek to image
  fseek(fp, s, SEEK_SET);

  int in_bytes_per_pixel = 4;  // RGB32
  int bytes_per_pixel = 3;
  int interlace_factor = p->interlaced ? 2 : 1;

  if (p->compression == "2") {
    // RGB32 extract to RGB byte-triplet buffer
    if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "[RGB32 %d @ %d ", frame_index, s);

    int w = p->width;
    int h = p->height / interlace_factor;
    int inrowsize = w * in_bytes_per_pixel;
    int outrowsize = w * bytes_per_pixel;
    char* row_buf = new char[inrowsize];

    char r,g,b;
    for (int i=0; i < interlace_factor; ++i) {
      if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "fld %d ", i);
      for (int y=h-1; y >= 0; --y) {
        vcl_fread(row_buf, 1, inrowsize, fp);
        char* buf_ptr = (char*)buffer + (interlace_factor * y + i) * outrowsize;
        char* row_ptr = row_buf;
        for (int x=0; x < w; ++x) {
          row_ptr++;   // Skip alpha byte
          b = *(row_ptr++);
          g = *(row_ptr++);
          r = *(row_ptr++);
          *(buf_ptr++) = r;  // R
          *(buf_ptr++) = g;  // G
          *(buf_ptr++) = b;  // B
        }
      }
    }

    if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "] ");
    vcl_fclose(fp);
    delete[] row_buf;
  }
  else if (p->compression == "10") {
    // JPEG
    if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "[JPEG %d @ %d ", frame_index, s);

    JPEG_Decompressor jpeg(fileno(fp));
    for(int i = 0; i < interlace_factor; ++i) {
      if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "fld %d ", i);
      if (i > 0) jpeg.StartNextJPEG();

      int w = jpeg.width();
      int h = jpeg.height();

      if (w != p->width) {
        vul_printf(vcl_cerr, "SGIMovieFile: Discrepancy between jpeg size and movie size."
                   "  jpeg x = %d, movie x = %d\n", h, p->height);
      }
      if (h*interlace_factor != p->height) {
        vul_printf(vcl_cerr, "SGIMovieFile: Discrepancy between jpeg size and movie size."
                   "  jpeg y = %d, movie y = %d, interlacing = %d\n", h, p->height, p->interlaced);
      }

      int outrowsize = w * bytes_per_pixel;
      for(int y = 0; y < jpeg.height(); ++y) {
        char *jbuf = (char*)jpeg.GetNextScanLine();
        if (!jbuf) {
          vul_printf(vcl_cerr, "SGIMovieFile: JPEG_Decompressor failed to load scanline %d, field %d, frame %d\n",
                     y, i, frame_index);
          return false;
        }
        char* bufptr = (char*)buffer + (interlace_factor * y + i) * outrowsize;
        vcl_memcpy(bufptr, jbuf, jpeg.width() * bytes_per_pixel);
      }
      if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "eof %d ", jpeg.GetFilePosition());

      // Now file position is at second field if interlaced, remember it in case anyone wants this field again.
      if (p->interlaced && i == 0)
        p->field_indices[0][frame_index * 2 + 1] = jpeg.GetFilePosition();
    }
    if (MovieFileInterface::verbose) vul_printf(vcl_cerr, "] ");
    vcl_fclose(fp);
  }

  return true;
}

bool SGIMovieFile::GetField(int field_index, void* buffer)
{
  return false;
#if 0
  if (!p->interlaced)
    return GetFrame(field_index, buffer);

  // Load a jpeg from fd...
  if (p->compression != "10")
    vul_printf(vcl_cerr, "SGIMovieFile: Can't decompress ``%s'' format images\n", p->compression.c_str());
  else {

    int field_start = p->field_indices[0][field_index];
    JPEG_Decompressor *jpeg;
    if (field_start == 0) {
      int frame_index = field_index / 2;
      int frame_start = p->video_indices[0][frame_index].offset;
      // Must go to the start of the frame and read forward
      p->fp->seekg(frame_start);

      jpeg = new JPEG_Decompressor(p->fp->rdbuf()->fd());
      for(int y = 0; y < jpeg->height(); ++y)
        jpeg->GetNextScanLine();

      // Now file position is at second field, remember it in case anyone wants this field again.
      p->field_indices[0][frame_index * 2 + 1] = jpeg->GetFilePosition();

      jpeg->StartNextJPEG();
    } else {
      p->fp->seekg(field_start);

      jpeg = new JPEG_Decompressor(p->fp->rdbuf()->fd());
    }

    // Read the image
    int w = jpeg->width();
    int h = jpeg->height();

    if (w != p->width) {
      vul_printf(vcl_cerr, "SGIMovieFile: Discrepancy between jpeg size and movie size."
                 "  jpeg x = %d, movie x = %d\n", h, p->height);
    }
    if (h*2 != p->height) {
      vul_printf(vcl_cerr, "SGIMovieFile: Discrepancy between jpeg size and movie size."
                 "  jpeg y = %d, movie y = %d, interlacing = %d\n", h, p->height, p->interlaced);
    }

    int bytes_per_pixel = GetBitsPixel() / 8;
    int outrowsize = w * bytes_per_pixel;
    for(int y = 0; y < jpeg->height(); ++y) {
      char *jbuf = (char*)jpeg->GetNextScanLine();
      if (!jbuf) {
        vul_printf(vcl_cerr, "SGIMovieFile: JPEG_Decompressor failed to load scanline %d, field %d\n",
                   y, field_index);
        return false;
      }
      char* bufptr = (char*)buffer + y * outrowsize;
      vcl_memcpy(bufptr, jbuf, jpeg->width() * bytes_per_pixel);
    }

    delete jpeg;
  }
  return true;
#endif
}

/////////////////////////////////////////////////////////////////////////////

void SGIMV_Variables::read(vcl_istream& f) {
  //   struct SGIMV_Variables {
  //     word pad;
  //     word num_vars;
  //     word pad;
  //     struct VarData {
  //     byte var_buf[16];
  //     word var_size;
  //     byte var_data[var_size];  // Asciiish variable value
  //     };
  //     VarData data[num_vars];
  //   };
  /*int pad =*/ get_u32(f);
  int n = get_u32(f);
  /*pad =*/ get_u32(f);
  if (n > 1000) {
    vul_printf(vcl_cerr, "SGIMovieFile: warning: A Variable list is %d elements long\n", n);
  }
  for(int i = 0; i < n; ++i) {
    VarData v;

    char var_buf[17];
    f.read(var_buf, 16);
    var_buf[16] = 0;

    vxl_uint_32 var_size = get_u32(f);
    vcl_vector<char> val_buf(var_size+1,0);
    f.read(&val_buf[0], var_size);

    data[vcl_string(var_buf)] = vcl_string(&val_buf[0]);
  }
}

int SGIMV_Variables::get_int(vcl_string const& s)
{
  const vcl_string& v = data[s];
  if (v.size() == 0)
    return -1;
  vcl_istrstream vs((const char*)v.data(), v.size());
  int x;
  vs >> x;
  return x;
}

double SGIMV_Variables::get_double(vcl_string const& s)
{
  const vcl_string& v = data[s];
  if (v.size() == 0)
    return -1.0;
  vcl_istrstream vs((char*)v.data(), v.size());
  double x;
  vs >> x;
  return x;
}

vcl_ostream& SGIMV_Variables::print(vcl_ostream& s) const
{
  vul_printf(s, "SGIMV_Variables:\n", data.size());
  for(VarData::const_iterator i = data.begin(); i != data.end(); ++i)
    vul_printf(s, "   %16s = %s\n", (*i).first.c_str(), (*i).second.c_str());
  return s;
}

/////////////////////////////////////////////////////////////////////////////

SGIMV_FrameIndexArray::SGIMV_FrameIndexArray(vcl_istream& f, int n):
  vcl_vector<SGIMV_FrameIndex>(n, SGIMV_FrameIndex())
{
//   struct Index {
//     word offset;
//     word size;
//     word pad;
//     word frame; // ????
//   };

  for(int i = 0; i < n; ++i) {
    (*this)[i].offset = get_u32(f);
    (*this)[i].size = get_u32(f);
    /* int pad1 = */ get_u32(f);
    /* int pad2 = */ get_u32(f);
    //    vul_printf(cerr, "FMV: %8d %8d %8d %8d\n", (*this)[i].offset, (*this)[i].size, pad1, pad2);
  }
}

/////////////////////////////////////////////////////////////////////////////
#if 0 // unused
static void hexdump(vcl_ifstream& f, int nframes)
{
  for(int j = 0; j < nframes; ++j) {
    int pos = f.tellg();
    vxl_uint_8 buf[16];
    f.read((char*)buf,16);
    vul_printf(vcl_cerr, "%08x:", pos);
    for(int i = 0; i < 16; ++i) {
      if (i % 4 == 0) vul_printf(vcl_cerr, " ");
      vul_printf(vcl_cerr, "%02x", buf[i]);
    }
    vul_printf(vcl_cerr, "\n");
  }
}
#endif

static int get_u16(vcl_istream& f)
{
  vxl_uint_8 buf[2];
  f.read((char*)buf, 2);
  return (buf[0] << 8) + buf[1];
}

static int get_u32(vcl_istream& f)
{
  vxl_uint_8 buf[4];
  f.read((char*)buf, 4);
  return (((unsigned long)buf[0] << 24) |
          ((unsigned long)buf[1] << 16) |
          ((unsigned long)buf[2] << 8) |
          buf[3]);
}
