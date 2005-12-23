// This is contrib/brl/bbas/vidl2/vidl2_ffmpeg_init.h
#ifndef vidl2_ffmpeg_init_h_
#define vidl2_ffmpeg_init_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Initialization routine for FFMPEG
//
// \author Matt Leotta 
// \date 21 Dec 2005
//
// \verbatim
//  Modifications
//    Matt Leotta   21 Dec 2005   Adapted from codec by Amitha Perera
// \endverbatim

//: Initialize the ffmpeg codecs.
//
// This will be called by the vidl2_ffmpeg streams, so you
// shouldn't need to worry about it. This function can be called
// multiple times, but the real ffmpeg initialization
// routine will run only once.
void vidl2_ffmpeg_init();

#endif // vidl2_ffmpeg_init_h_

