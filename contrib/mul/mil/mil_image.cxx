#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl_cstdlib.h>
#include <mil/mil_image.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================

//=======================================================================
// Dflt ctor
//=======================================================================

mil_image::mil_image()
{
}

//=======================================================================
// Destructor
//=======================================================================

mil_image::~mil_image()
{
}

//=======================================================================
// Method: version_no
//=======================================================================

short mil_image::version_no() const
{
    return 1;
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mil_image::is_a() const
{
  static const vcl_string c_ = "mil_image";
  return c_;
}

//=======================================================================
// Method: is_class
//=======================================================================

bool mil_image::is_class(vcl_string const& s) const
{
  static const vcl_string s_ = "mil_image";
  return s==s_;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mil_image* b)
{
    if (b)
    {
        vsl_b_write(bfs,b->is_a());
        b->b_write(bfs);
    }
    else
        vsl_b_write(bfs,vcl_string("VSL_NULL_PTR"));
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mil_image& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mil_image& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mil_image& b)
{
    os << b.is_a() << ": ";
    vsl_indent_inc(os);
    b.print_summary(os);
    vsl_indent_dec(os);
    return os;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mil_image* b)
{
    if (b)
        return os << *b;
    else
        return os << "No mil_image defined.";
}

