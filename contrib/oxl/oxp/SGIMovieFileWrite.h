// This is oxl/oxp/SGIMovieFileWrite.h
#ifndef SGIMovieFileWrite_h_
#define SGIMovieFileWrite_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 11 Jan 99
//
//-----------------------------------------------------------------------------

struct SGIMovieFileWriteData;

class SGIMovieFileWrite
{
  SGIMovieFileWriteData* p;
 public:
  SGIMovieFileWrite(char const* filename,
                    int w, int h,
                    int length);
  ~SGIMovieFileWrite();

  //void PutFrame(int i, void* buf = 0);

  unsigned char* GetBuffer();
  void PutBuffer(int frame_index);
  void Finish();
};

#endif // SGIMovieFileWrite_h_
