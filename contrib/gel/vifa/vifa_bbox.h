// This is contrib/gel/vifa/vifa_bbox.h
#ifndef _VIFA_BBOX_H_
#define _VIFA_BBOX_H_

//-----------------------------------------------------------------------------
//:
// \file
// \brief Timestamped 2D bounding box
//
// The vifa_bbox class is a timestamped 2D bounding box (vgl/vgl_box_2d) used
// by the vifa_group_pgram class to efficiently compute the bounding box of a
// collection of lines.
//
// \author Mike Petersen, June 2003
//-----------------------------------------------------------------------------

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vgl/vgl_box_2d.h>
#include <vul/vul_timestamp.h>


class vifa_bbox : public vul_timestamp, public vbl_ref_count,
	public vgl_box_2d<double>
{
public:
	//: Default constructor
	inline vifa_bbox(void) {}
};

typedef vbl_smart_ptr<vifa_bbox>	vifa_bbox_sptr;


#endif	// _VIFA_BBOX_H_

