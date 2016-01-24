// This is core/vul/vul_redirector.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include <vcl_compiler.h>

#include "vul_redirector.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h> // for EOF

//----------------------------------------------------------------------
// This class is used as a stream buffer that can
// redirect output from cout, cerr, clog to a CoutWindow class.
class vul_redirector_streambuf : public vcl_streambuf
{
  vul_redirector_data* p;
 public:
  vul_redirector_streambuf(vul_redirector_data* p_):p(p_) {}
  int sync ();
  int overflow (int ch);
  int underflow(){return 0;}
  // Some libraries which take char const *, which is
  // non-standard, but we have to live with it. A
  // better cpp test would be welcome. fsm.
#if defined(__INTEL_COMPILER)
  // RogueWave or ISO?
# define xsputn_const const
# define xsputn_sizet vcl_streamsize
#else
# define xsputn_const /* */
# define xsputn_sizet vcl_streamsize
#endif
  xsputn_sizet xsputn (xsputn_const char* text, xsputn_sizet n);
};

struct vul_redirector_data
{
  vul_redirector* owner;
  vcl_streambuf* old_cerrbuf;
  vul_redirector_streambuf* buf;
  vcl_ostream* s;
};

/////////////////////////////////////////////////////////////////////////////
// streambuf stuff

int vul_redirector_streambuf::sync ()
{
  vcl_ptrdiff_t n = pptr () - pbase ();
  return (n && p->owner->putchunk ( pbase (), n) != n) ? EOF : 0;
}

int vul_redirector_streambuf::overflow (int ch)
{
  int n = static_cast<int>(pptr () - pbase ());
  if (n && sync ())
    return EOF;
  if (ch != EOF)
  {
    char cbuf[1];
    cbuf[0] = (char)ch;
    if (p->owner->putchunk ( cbuf, 1) != 1)
      return EOF;
  }
  pbump (-n);  // Reset pptr().
  return 0;
}

xsputn_sizet vul_redirector_streambuf::xsputn (xsputn_const char* text, xsputn_sizet n)
{
  return sync () == EOF ? 0 : p->owner->putchunk ( text, n);
}

//////////////// Data for debugging


vul_redirector::vul_redirector(vcl_ostream& s):
  p(new vul_redirector_data)
{
  p->owner = this;
  p->buf = new vul_redirector_streambuf(p);
  p->old_cerrbuf = s.rdbuf();
  s.rdbuf(p->buf);
  p->s = &s;
}

vul_redirector::~vul_redirector()
{
  p->s->rdbuf(p->old_cerrbuf);
  delete p->buf;
  delete p;
}

int vul_redirector::sync_passthru()
{
  return p->old_cerrbuf->pubsync();
}

vcl_streamsize vul_redirector::put_passthru(char const* buf, vcl_streamsize n)
{
  return p->old_cerrbuf->sputn(buf, n);
}

//: Default action is just to pass text on the old stream.
vcl_streamsize vul_redirector::putchunk(char const* buf, vcl_streamsize n)
{
  return put_passthru(buf, n);
}
