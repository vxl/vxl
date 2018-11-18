#include <iostream>
#include "brad_io_image_metadata.h"
//:
// \file
#include <brad/brad_image_metadata.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Binary save brad_image_metadata to stream.
void
vsl_b_write(vsl_b_ostream & /*os*/, const brad_image_metadata&  /*m*/)
{
}

//: Binary load brad_image_metadata from stream.
void
vsl_b_read(vsl_b_istream & /*is*/, brad_image_metadata&  /*m*/)
{
}

//: Print summary
void
vsl_print_summary(std::ostream &os, const brad_image_metadata&  /*m*/)
{
  os << "brad_image_metadata ";
}

//: Binary write brad_image_metadata to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, const brad_image_metadata* & /*p*/){}
//: Binary write brad_image_metadata to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, brad_image_metadata_sptr&  /*sptr*/){}
//: Binary write brad_image_metadata to stream
void vsl_b_write(vsl_b_ostream&  /*os*/, brad_image_metadata_sptr const&  /*sptr*/){}

//: Binary load brad_image_metadata scene from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, brad_image_metadata*  /*p*/){}
//: Binary load brad_image_metadata scene from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, brad_image_metadata_sptr&  /*sptr*/){}
//: Binary load brad_image_metadata scene from stream.
void vsl_b_read(vsl_b_istream&  /*is*/, brad_image_metadata_sptr const&  /*sptr*/){}
