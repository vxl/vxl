#! /bin/sh
# -*- perl -*-
exec perl -w -pi% -x $0 ${1+"$@"}
#!perl -pi%
#line 6

# This perl script converts class names xxx_ref to xxx_sptr
# as this is the conventional way to name smart pointers
# (decision on the general TargetJr/vxl meeting of 30 March 2001)
#
# This conversion applies to the following class names
# ("xxx" stands for [a-z23D_]+) :
# gmvl_xxx_ref gst_xxx_ref gtrl_xxx_ref vcsl_xxx_ref vdgl_xxx_ref
# vgui_xxx_ref vidl_xxx_ref vsol_xxx_ref vtol_xxx_ref xcv_image_tableau_ref
#
# Written by Peter Vanroose, 2 April 2001.

# Usage: find $IUELOCALROOT -type f \( -name '*.[tc]xx' -o -name '*.[hd]' -o -name \*.dsp -o -name \*.cpp \) -print | \
#        xargs egrep -l '\<((gmvl|gst|gtrl|vcsl|vdgl|vgui|vidl|vsol|vtol|vxl|xcv)_[a-z23D_]+_ref)|vgui_tableau_ref_t\>' | \
#        xargs $IUEROOT/vxl/bin/ref_to_sptr.pl
# Original files are renamed to files with extension %

s/\b((gmvl|gst|gtrl|vcsl|vdgl|vgui|vidl|vsol|vtol|vxl|xcv)\_[a-z23D_]+)\_ref((\_[ht]\_?)?)\b/$1_sptr$3/g;
