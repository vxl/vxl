#ifndef SGIMovieFileWrite_h_
#define SGIMovieFileWrite_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	SGIMovieFileWrite
// .LIBRARY	vview
// .HEADER	Oxford Package
// .INCLUDE	vview/SGIMovieFileWrite.h
// .FILE	SGIMovieFileWrite.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 11 Jan 99
//
//-----------------------------------------------------------------------------

struct SGIMovieFileWriteData;

struct SGIMovieFileWrite {
  SGIMovieFileWrite(char const* filename,
		    int w, int h,
		    int length);
  ~SGIMovieFileWrite();

  //void PutFrame(int i, void* buf = 0);

  unsigned char* GetBuffer();
  void PutBuffer(int frame_index);
  void Finish();

private:
  SGIMovieFileWriteData* p;
};

#endif // SGIMovieFileWrite_h_
