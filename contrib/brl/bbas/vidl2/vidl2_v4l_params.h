// This is brl/bbas/vidl2/vidl2_v4l_params.h
#ifndef vidl2_v4l_params_h_
#define vidl2_v4l_params_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A parameters struct for v4l1 camera streams
//
// \author Brendan McCane
// \date 15 Mar 2006
//
// These parameters are described by the v4l1 API specification
// http://www.linuxtv.org/downloads/video4linux/API/V4L1_API.html


// not used? #include <vcl_string.h>
#include "vidl2_pixel_format.h"
#include <linux/videodev.h>

//: A parameters struct for v4l1 camera streams

struct vidl2_v4l_params
{
    //: convert from vidl2 pixel format to v4l pixel format
    static int vidl2pf_to_v4lpf(vidl2_pixel_format vid_pf);

    //: convert from v4l pixel format to vidl2 pixel format
    static vidl2_pixel_format v4lpf_to_vidl2pf(int v4l_pf);

    //: construct with default params
    vidl2_v4l_params(): ni_(320), nj_(240), brightness_(31744), hue_(65535),
                        colour_(32768), contrast_(32768), whiteness_(49152),
                        depth_(24), pixel_format_(VIDL2_PIXEL_FORMAT_YUV_420P)
    {};

    //: get the current resolution
    vidl2_v4l_params& resolution(unsigned int ni, unsigned int nj){
        ni_=ni;nj_=nj;
        return *this;
    };

    //: brightness
    vidl2_v4l_params& brightness(int brightness){
        brightness_=brightness;
        return *this;
    };

    //: hue
    vidl2_v4l_params& hue(int hue){
        hue_=hue;
        return *this;
    };

    //: colour
    vidl2_v4l_params& colour(int colour){
        colour_=colour;
        return *this;
    };

    //: contrast
    vidl2_v4l_params& contrast(int contrast){
        contrast_=contrast;
        return *this;
    };

    //: whiteness
    vidl2_v4l_params& whiteness(int whiteness){
        whiteness_=whiteness;
        return *this;
    };

    //: depth
    vidl2_v4l_params& depth(int depth){
        depth_=depth;
        return *this;
    };

    //: palette
    vidl2_v4l_params& pixel_format(vidl2_pixel_format pf){
        pixel_format_=pf;
        return *this;
    };

    unsigned int ni_, nj_;
    int brightness_;
    int hue_;
    int colour_;
    int contrast_;
    int whiteness_;
    int depth_;
    vidl2_pixel_format pixel_format_;
};

#endif
