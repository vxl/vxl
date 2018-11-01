// This is core/vul/vul_redirector.cxx
//:
// \file

#include <iostream>
#include <cstdio>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "vul_redirector.h"

//----------------------------------------------------------------------
// This class is used as a stream buffer that can
// redirect output from cout, cerr, clog to a CoutWindow class.
class vul_redirector_streambuf : public std::streambuf
{
  vul_redirector_data* p;
 public:
  vul_redirector_streambuf(vul_redirector_data* p_):p(p_) {}
  int sync () override;
  int overflow (int ch) override;
  int underflow() override{return 0;}
  std::streamsize xsputn (const char* text, std::streamsize n) override;
};

struct vul_redirector_data
{
  vul_redirector* owner;
  std::streambuf* old_cerrbuf;
  vul_redirector_streambuf* buf;
  std::ostream* s;
};

/////////////////////////////////////////////////////////////////////////////
// streambuf stuff

int vul_redirector_streambuf::sync ()
{
  std::ptrdiff_t n = pptr () - pbase ();
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

std::streamsize vul_redirector_streambuf::xsputn (const char* text, std::streamsize n)
{
  return sync () == EOF ? 0 : p->owner->putchunk ( text, n);
}

//////////////// Data for debugging


vul_redirector::vul_redirector(std::ostream& s):
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

std::streamsize vul_redirector::put_passthru(char const* buf, std::streamsize n)
{
  return p->old_cerrbuf->sputn(buf, n);
}

//: Default action is just to pass text on the old stream.
std::streamsize vul_redirector::putchunk(char const* buf, std::streamsize n)
{
  return put_passthru(buf, n);
}
