// This is mul/mil/mil_image_io.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <vcl_cstdlib.h>
#include <mil/mil_image_io.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

mil_image_io::mil_image_io()
{
}

//=======================================================================

mil_image_io::~mil_image_io()
{
}

//=======================================================================

short mil_image_io::version_no() const
{
    return 1;
}

//=======================================================================

void vsl_add_to_binary_loader(const mil_image_io& b)
{
    vsl_binary_loader<mil_image_io>::instance().add(b);
}

//=======================================================================

vcl_string mil_image_io::is_a() const
{
  return vcl_string("mil_image_io");
}

//=======================================================================

bool mil_image_io::is_class(vcl_string const& s) const
{
  return s==mil_image_io::is_a();
}

//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mil_image_io& b)
{
    b.b_write(bfs);
}

//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mil_image_io& b)
{
    b.b_read(bfs);
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mil_image_io& b)
{
    os << b.is_a() << ": ";
    vsl_indent_inc(os);
    b.print_summary(os);
    vsl_indent_dec(os);
    return os;
}

//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mil_image_io* b)
{
    if (b)
        return os << *b;
    else
        return os << "No mil_image_io defined.";
}

