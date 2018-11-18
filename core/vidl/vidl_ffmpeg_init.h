// This is core/vidl/vidl_ffmpeg_init.h
#ifndef vidl_ffmpeg_init_h_
#define vidl_ffmpeg_init_h_
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
// This will be called by the vidl_ffmpeg streams, so you
// shouldn't need to worry about it. This function can be called
// multiple times, but the real ffmpeg initialization
// routine will run only once.
void vidl_ffmpeg_init();

#endif // vidl_ffmpeg_init_h_
