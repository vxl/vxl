//-*- c++ -*-------------------------------------------------------------------
#ifndef SGIMovieFileWrite_h_
#define SGIMovieFileWrite_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : SGIMovieFileWrite
//
// .SECTION Description
//    SGIMovieFileWrite is a class that awf hasn't documented properly. FIXME
//
// .NAME        SGIMovieFileWrite - Undocumented class FIXME
// .LIBRARY     vview
// .HEADER	Oxford Package
// .INCLUDE     vview/SGIMovieFileWrite.h
// .FILE        SGIMovieFileWrite.h
// .FILE        SGIMovieFileWrite.cxx
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

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS SGIMovieFileWrite.

