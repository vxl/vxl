#ifdef __GNUC__
#pragma implementation
#endif

#include <vcl_cstdlib.h>
#include <mil/mil_image_io.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
// Dflt ctor
//=======================================================================

mil_image_io::mil_image_io()
{
}

//=======================================================================
// Destructor
//=======================================================================

mil_image_io::~mil_image_io()
{
}

//=======================================================================
// Method: version_no
//=======================================================================

short mil_image_io::version_no() const
{
    return 1;
}

//=======================================================================
// Method: vsl_add_to_binary_loader
//=======================================================================

void vsl_add_to_binary_loader(const mil_image_io& b)
{
    vsl_binary_loader<mil_image_io>::instance().add(b);
}

//=======================================================================
// Method: is_a
//=======================================================================

vcl_string mil_image_io::is_a() const
{
  return vcl_string("mil_image_io");
}

//=======================================================================
// Method: is_class
//=======================================================================

bool mil_image_io::is_class(vcl_string const& s) const
{
  static const vcl_string s_ = "mil_image_io";
  return s==s_;
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const mil_image_io* b)
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

void vsl_b_write(vsl_b_ostream& bfs, const mil_image_io& b)
{
    b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, mil_image_io& b)
{
    b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
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
// Associated function: operator<<
//=======================================================================

vcl_ostream& operator<<(vcl_ostream& os,const mil_image_io* b)
{
    if (b)
        return os << *b;
    else
        return os << "No mil_image_io defined.";
}

