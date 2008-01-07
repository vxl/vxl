// This is mul/vimt/vimt_image_pyramid_builder.cxx
#include "vimt_image_pyramid_builder.h"
//:
// \file
// \brief Class to load and save images from named files
// \author Tim Cootes

#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

vimt_image_pyramid_builder::vimt_image_pyramid_builder()
{
}

//=======================================================================

vimt_image_pyramid_builder::~vimt_image_pyramid_builder()
{
}

//=======================================================================

short vimt_image_pyramid_builder::version_no() const
{
    return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const vimt_image_pyramid_builder& b)
{
    vsl_binary_loader<vimt_image_pyramid_builder>::instance().add(b);
}

//=======================================================================

vcl_string vimt_image_pyramid_builder::is_a() const
{
  return vcl_string("vimt_image_pyramid_builder");
}

//=======================================================================

bool vimt_image_pyramid_builder::is_class(vcl_string const& s) const
{
  return s==vimt_image_pyramid_builder::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const vimt_image_pyramid_builder& b)
{
    b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, vimt_image_pyramid_builder& b)
{
    b.b_read(bfs);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const vimt_image_pyramid_builder& b)
{
    os << b.is_a() << ": ";
    vsl_indent_inc(os);
    b.print_summary(os);
    vsl_indent_dec(os);
    return os;
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const vimt_image_pyramid_builder* b)
{
    if (b)
        return os << *b;
    else
        return os << "No vimt_image_pyramid_builder defined.";
}
