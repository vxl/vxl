// This is oxl/apps/glmovie/make_index.cxx

//:
// \file
// \author fsm

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>

struct scanner_t
{
  scanner_t(char const *filename)
    : m_fp(filename, vcl_ios_binary)
    , m_p(m_buf)
    , m_q(m_buf)
    , m_where(0L)
  {
    if (! m_fp) {
      vcl_cerr << __FILE__ << ':' << __LINE__
               << " failed to open '" << filename << "' for reading:"
               << " error code " << m_fp.fail() << '\n';
      assert(false);
    }
  }

  ~scanner_t()
  {
    // if (m_fp.rdbuf()->is_open()) m_fp.close();
    // Not needed, since it is implicitly done by m_fp's destructor
  }

  // seek to position.
  void seek(vcl_streampos pos)
  {
    m_p = m_q = m_buf;
    if (! m_fp || ! m_fp.rdbuf()->is_open())
      return;
    m_fp.seekg(pos);
    if (m_fp.tellg() == pos)
      m_where = pos;
    else // error.
      m_fp.close();
  }

  // return position of next byte to be read.
  long tell() const
  {
    return m_where;
  }

  // get one byte, or -1 on EOF.
  int get()
  {
    if (m_p == m_q) {
      // this branch is taken when we need more data, i.e. rarely.
      if (! m_fp || ! m_fp.rdbuf()->is_open())
        return -1;
      m_fp.read(m_buf, sizeof m_buf);
      int n = m_fp.gcount();
      if (n > 0) {
        m_p = m_buf;
        m_q = m_p + n;
      }
      else {
        vcl_cout << "read=" << n << '\n';
        // probably EOF.
        m_fp.close();
        return -1;
      }
    }

#if 1
    m_where += 1;
    return *m_p++;
#else
    unsigned char byte = *m_p++;
    vcl_cout << "read: " << vcl_hex << m_where << ' ' << vcl_hex << unsigned(byte) << '\n';
    m_where += 1;
    return byte;
#endif
  }

private:
  vcl_ifstream m_fp;
  char *m_p, *m_q;
  char m_buf[65536];
  vcl_streampos m_where;
};

void make_mpeg_index(char const* in, char const* out)
{
  scanner_t scanner(in);

  char buf[1024];
  if (out == 0 || *out == 0) {
    vcl_strcpy(buf, in);
    char* p = vcl_strrchr(buf, '.');
    if (!p) {
      // No . in filename
      p = buf + vcl_strlen(buf)-1;
    }
    vcl_strcpy(p, ".idx");
    vcl_cerr << "Making index file [" << buf << "]\n";
    out = buf;
  }

  vcl_ofstream fp(out);
  fp << "MPEG_IDX " << in << '\n';

  unsigned shift = 0;
  long last_mb = 0;

  int n = 0;
  while (true) {
    int c = scanner.get();
    ++n;
    if (c < 0) {
      // vcl_cout << "Brk on char " << n << ", scnr = " << c << '\n';
      break; // EOF
    }

    shift = (shift | c) << 8; // NB. last 8 bits will be zero.
    if (shift == 0x00000100) {
      long pos = scanner.tell() - 3;
      c = scanner.get(); shift = (shift | c) << 8;

      int code = c;
      // vcl_printf("code = 0x%02X\n", code);
      if (code == 0x00) {
        c = scanner.get(); shift = (shift | c) << 8;
        c = scanner.get(); shift = (shift | c) << 8;
        int type = (c >> 3) & 0x07;
        fp << vcl_hex << pos << ' ' << vcl_hex << code << ' ' << type << '\n';
      }
      else if (code < 0xB0)
        ; // ignore
      else {
        fp << vcl_hex << pos << ' ' << vcl_hex << code << '\n';
      }

      // show progress.
      if ((pos>>20) > last_mb) {
        last_mb = (pos>>20);
        vcl_cerr << "[" << last_mb << " Mb]" << vcl_flush;
      }
    }
  }
  fp.close();
  vcl_cout << "Index done\n";
  // vcl_printf("0x%08lX\n", scanner.tell());
}
