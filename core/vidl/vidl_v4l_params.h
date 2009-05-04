// This is core/vidl/vidl_v4l_params.h
#ifndef vidl_v4l_params_h_
#define vidl_v4l_params_h_
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


#include "vidl_pixel_format.h"
#include <linux/videodev.h>

//: A parameters struct for v4l1 camera streams

struct vidl_v4l_params
{
    //: convert from vidl pixel format to v4l pixel format
    static int vidlpf_to_v4lpf(vidl_pixel_format vid_pf);

    //: convert from v4l pixel format to vidl pixel format
    static vidl_pixel_format v4lpf_to_vidlpf(int v4l_pf);

    //: construct with default params
    vidl_v4l_params(): ni_(320), nj_(240), brightness_(31744), hue_(65535),
                        colour_(32768), contrast_(32768), whiteness_(49152),
                        depth_(24), pixel_format_(VIDL_PIXEL_FORMAT_YUV_420P)
    {}

    //: get the current resolution
    vidl_v4l_params& resolution(unsigned int ni, unsigned int nj){
        ni_=ni;nj_=nj;
        return *this;
    }

    //: brightness
    vidl_v4l_params& brightness(int brightness){
        brightness_=brightness;
        return *this;
    }

    //: hue
    vidl_v4l_params& hue(int hue){
        hue_=hue;
        return *this;
    }

    //: colour
    vidl_v4l_params& colour(int colour){
        colour_=colour;
        return *this;
    }

    //: contrast
    vidl_v4l_params& contrast(int contrast){
        contrast_=contrast;
        return *this;
    }

    //: whiteness
    vidl_v4l_params& whiteness(int whiteness){
        whiteness_=whiteness;
        return *this;
    }

    //: depth
    vidl_v4l_params& depth(int depth){
        depth_=depth;
        return *this;
    }

    //: palette
    vidl_v4l_params& pixel_format(vidl_pixel_format pf){
        pixel_format_=pf;
        return *this;
    }

    unsigned int ni_, nj_;
    int brightness_;
    int hue_;
    int colour_;
    int contrast_;
    int whiteness_;
    int depth_;
    vidl_pixel_format pixel_format_;
};

#endif
