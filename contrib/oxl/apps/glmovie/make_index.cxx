/*
  fsm@robots.ox.ac.uk
*/

// g++ -O3 -o ./mpeg_chunks ./mpeg_chunks.cxx -Wl,-Bstatic -lstdc++

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_memory.h>

struct scanner_t
{
  scanner_t(char const *filename)
    : m_fp(vcl_fopen(filename, "rb"))
    , m_p(m_buf)
    , m_q(m_buf)
    , m_where(0)
  {
    if (! m_fp) {
      vcl_fprintf(stderr, "%s:%d failed to open \'%s\' for reading\n", __FILE__, __LINE__, filename);
      assert(false);
    }
  }
  
  ~scanner_t()
  {
    if (m_fp) {
      vcl_fclose(m_fp);
      m_fp = 0;
    }
  }
  
  // seek to position.
  void seek(long pos)
  {
    m_p = m_q = m_buf;
    if (! m_fp)
      return;
    if (vcl_fseek(m_fp, pos, SEEK_SET) == 0) {
      m_where = pos;
    }
    else {
      // error.
      vcl_fclose(m_fp);
      m_fp = 0;
    }
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
      if (! m_fp)
        return -1;
      int n = vcl_fread(m_buf, 1, sizeof m_buf, m_fp);
      if (n > 0) {
        m_p = m_buf;
        m_q = m_p + n;
      }
      else {
	printf("fread=%d\n", n);
        // probably EOF.
        fclose(m_fp);
        m_fp = 0;
        return -1;
      }
    }
    
#if 1
    ++ m_where;
    return *m_p++;
#else
    unsigned char byte = *m_p++;
    vcl_fprintf(stdout, "read: 0x%08lX 0x%02X\n", m_where++, unsigned(byte));
    return byte;
#endif
  }
  
private: 
  vcl_FILE *m_fp;
  unsigned char *m_p, *m_q;
  unsigned char m_buf[65536];
  long m_where;
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
    vcl_fprintf(stderr, "Making index file [%s]\n", buf);
    out = buf;
  }
      
  vcl_FILE* fp = fopen(out, "w");
  vcl_fprintf(fp, "MPEG_IDX %s\n", in);
  
  unsigned shift = 0;
  long last_mb = 0;

  int n = 0;  
  while (true) {
    int c = scanner.get();
    ++n;
    if (c < 0) {
      // std :: printf("Brk on char %d, scnr = %d\n", n, c);
      break; // EOF
    }
    
    shift = (shift | c) << 8; // NB. last 8 bits will be zero.
    if (shift == 0x00000100) {
      long pos = scanner.tell() - 3;
      c = scanner.get(); shift = (shift | c) << 8;
      
      int code = c;
      //vcl_printf("code = 0x%02X\n", code);
      if (code == 0x00) {
        c = scanner.get(); shift = (shift | c) << 8;
        c = scanner.get(); shift = (shift | c) << 8;
        int type = (c >> 3) & 0x07;
        vcl_fprintf(fp, "0x%08lX 0x%02X %c\n", pos, code, "01234567"[type]);
      }
      else if (code < 0xB0)
        ; // ignore
      else {
        vcl_fprintf(fp, "0x%08lX 0x%02X\n", pos, code);
      }

      // show progress.
      if ((pos>>20) > last_mb) {
        last_mb = (pos>>20);
        vcl_fprintf(stderr, "[%ld Mb]", (pos>>20));
        vcl_fflush(stderr);
      }
    }
  }
  vcl_fclose(fp);
  vcl_printf("Index done\n");
  // long pos = scanner.tell();
  // vcl_printf("0x%08lX\n", pos);
}
