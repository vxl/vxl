// This is core/vul/vul_url.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott
// Based on vil_stream_url by fsm
// \verbatim
//  Modifications
//   8 Nov 2002 - Peter Vanroose - corrected HTTP client request syntax
// \endverbatim

#include "vul_url.h"
#include <vcl_cstdio.h>  // sprintf()
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_sstream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vul/vul_file.h>

#if defined(unix) || defined(__unix)

# include <unistd.h>       // read(), write(), close()
# include <netdb.h>        // gethostbyname(), sockaddr_in()
# include <sys/socket.h>
# include <netinet/in.h>   // htons()
# ifdef __alpha
#  include <fp.h>          // htons() [ on e.g. DEC alpha, htons is in machine/endian.h ]
# endif
# define SOCKET int

#elif defined (VCL_WIN32) && !defined(__CYGWIN__)

# include <winsock2.h>

#endif // unix

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
// So that we don't call WSAStartup more than we need to
static int called_WSAStartup = 0;
#endif

//: only call this method with a correctly formatted http URL
vcl_istream * vul_http_open(char const *url)
{
  // split URL into auth, host, path and port number.
  vcl_string host;
  vcl_string path;
  vcl_string auth;
  int port = 80; // default

  // check it is an http URL.
  assert (vcl_strncmp(url, "http://", 7) == 0);

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = vcl_string(url+7, p);


  if (*p)
    path = p+1;
  else
    path = "";

  //authentication
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = vcl_string(host.c_str(), host.c_str()+i);
      host = vcl_string(host.c_str()+i+1, host.c_str() + host.size());
      break;
    }

  // port?
  for (unsigned int i=host.size()-1; i>0; --i)
    if (host[i] == ':') {
      port = vcl_atoi(host.c_str() + i + 1);
      host = vcl_string(host.c_str(), host.c_str() + i);
      break;
    }

  // do character translation
  unsigned k =0;
  while (k < path.size())
  {
    if (path[k] == ' ')
      path.replace(k, 1, "%20");
    else if (path[k] == '%')
      path.replace(k, 1, "%25");
    k++;
  }

  // so far so good.
#ifdef DEBUG
  vcl_cerr << "auth = \'" << auth << "\'\n"
           << "host = \'" << host << "\'\n"
           << "path = \'" << path << "\'\n"
           << "port = " << port << vcl_endl;
#endif

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  if (called_WSAStartup==0)
  {
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );

    /* int err = */ WSAStartup( wVersionRequested, &wsaData );
  }
#endif

  // create socket endpoint.
  SOCKET tcp_socket = socket(PF_INET,      // IPv4 protocols.
                             SOCK_STREAM,  // two-way, reliable,
                                           // connection-based stream socket.
                             PF_UNSPEC);   // protocol number.
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  if (tcp_socket == INVALID_SOCKET) {
# ifndef NDEBUG
    vcl_cerr << __FILE__ "error code : " << WSAGetLastError() << '\n';
# endif
#else
  if (tcp_socket < 0) {
#endif
    vcl_cerr << __FILE__ ": failed to create socket.\n";
    return 0;
  }

#ifdef DEBUG
  vcl_cerr << __FILE__ ": tcp_socket = " << tcp_socket << '\n';
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    vcl_cerr << __FILE__ ": failed to lookup host\n";

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif

    return 0;
  }

  // make socket address.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  // convert port number to network byte order..
  my_addr.sin_port = htons(port);
  vcl_memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  // connect to server.
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0) {
    vcl_cerr << __FILE__ ": failed to connect to host\n";
    //perror(__FILE__);

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif

    return 0;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.1 request.
  vcl_sprintf(buffer, "GET %s HTTP/1.1\r\nUser-Agent: vul_url\r\nHost: %s\r\nAccept: */*\r\n",
              url, host.c_str());

  if (auth != "")
    vcl_sprintf(buffer+vcl_strlen(buffer),
                "Authorization: Basic %s\r\n",
                vul_url::encode_base64(auth).c_str());

  vcl_sprintf(buffer+vcl_strlen(buffer), "\r\n");

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  if (send(tcp_socket, buffer, vcl_strlen(buffer), 0) < 0) {
#else
  if (::write(tcp_socket, buffer, vcl_strlen(buffer)) < 0) {
#endif
    vcl_cerr << __FILE__ ": error sending HTTP request\n";

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return 0;
  }

  // force the data to be sent.
#if 1
  shutdown(tcp_socket, 1); // disallow further sends.
#else
  for (int i=0; i<4096; ++i) ::write(tcp_socket, "\n\n\n\n", 4);
#endif

  // read from socket into memory.
  vcl_string contents;
  {
    int n;
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    while ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0) {
#else
    while ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
#endif
      contents.append(buffer, n);
      //vcl_cerr << n << " bytes\n";
    }
  }

  // close connection to server.
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif

#ifdef DEBUG
  vcl_cerr << "HTTP server returned:\n" << contents << '\n';
#endif

  if (contents.find("HTTP/1.1 200") == contents.npos)
  {
    return 0;
  }
  vcl_string::size_type n = contents.find("\r\n\r\n");
  if (n == contents.npos)
  {
    return 0;
  }

  contents.erase(0,n+4);
#ifdef DEBUG
  vcl_cerr << "vul_url::vul_http_open() returns:\n" << contents << '\n';
#endif
  return new vcl_istringstream(contents);
}


//: only call this method with a correctly formatted http URL
bool vul_http_exists(char const *url)
{
  // split URL into auth, host, path and port number.
  vcl_string host;
  vcl_string path;
  vcl_string auth;
  int port = 80; // default
  assert (vcl_strncmp(url, "http://", 7) == 0);

  char const *p = url + 7;
  while (*p && *p!='/')
    ++ p;
  host = vcl_string(url+7, p);


  if (*p)
    path = p+1; // may be the empty string, if URL ends in a slash
  else
    path = "";

  //authentication
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = vcl_string(host.c_str(), host.c_str()+i);
      host = vcl_string(host.c_str()+i+1, host.c_str() + host.size());
      break;
    }

  // port?
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == ':') {
      port = vcl_atoi(host.c_str() + i + 1);
      host = vcl_string(host.c_str(), host.c_str() + i);
      break;
    }

  // do character translation
  unsigned k =0;
  while (k < path.size())
  {
    if (path[k] == ' ')
      path.replace(k, 1, "%20");
    else if (path[k] == '%')
      path.replace(k, 1, "%25");
    k++;
  }

  // so far so good.
#ifdef DEBUG
  vcl_cerr << "auth = \'" << auth << "\'\n"
           << "host = \'" << host << "\'\n"
           << "path = \'" << path << "\'\n"
           << "port = " << port << vcl_endl;
#endif

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  if (called_WSAStartup==0)
  {
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );

    /* int err = */ WSAStartup( wVersionRequested, &wsaData );
  }
#endif

  // create socket endpoint.
  SOCKET tcp_socket = socket(PF_INET,      // IPv4 protocols.
                             SOCK_STREAM,  // two-way, reliable,
                                           // connection-based stream socket.
                             PF_UNSPEC);   // protocol number.

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  if (tcp_socket == INVALID_SOCKET) {
# ifndef NDEBUG
    vcl_cerr << "error code : " << WSAGetLastError() << vcl_endl;
# endif
#else
  if (tcp_socket < 0) {
#endif
    vcl_cerr << __FILE__ ": failed to create socket.\n";
    return false;
  }

#ifdef DEBUG
  vcl_cerr << __FILE__ ": tcp_socket = " << tcp_socket << vcl_endl;
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    vcl_cerr << __FILE__ ": failed to lookup host\n";
    return false;
  }

  // make socket address.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
    // convert port number to network byte order..
  my_addr.sin_port = htons(port);
  vcl_memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  // connect to server.
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0)
  {
    vcl_cerr << __FILE__ ": failed to connect to host\n";
    //perror(__FILE__);
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif

    return false;
  }

  // buffer for data transfers over socket.
  char buffer[4096];

  // send HTTP 1.1 request.
  vcl_sprintf(buffer, "HEAD %s HTTP/1.1\r\nUser-Agent: vul_url\r\nHost: %s\r\nAccept: */*\r\n",
              url, host.c_str());
  if (auth != "")
    vcl_sprintf(buffer+vcl_strlen(buffer), "Authorization: Basic %s\r\n",
                vul_url::encode_base64(auth).c_str());
  vcl_sprintf(buffer+vcl_strlen(buffer),"\r\n");

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  if (send(tcp_socket, buffer, vcl_strlen(buffer), 0) < 0) {
#else
  if (::write(tcp_socket, buffer, vcl_strlen(buffer)) < 0) {
#endif
    vcl_cerr << __FILE__ ": error sending HTTP request\n";

#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return false;
  }

  // force the data to be sent.
#if 1
  shutdown(tcp_socket, 1); // disallow further sends.
#else
  for (int i=0; i<4096; ++i) ::write(tcp_socket, "\n\n\n\n", 4);
#endif

  // read from socket into memory.
  vcl_string contents;
  {
    int n;
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
    if ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0) {
#else
    if ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0) {
#endif
      contents.append(buffer, n);
      //vcl_cerr << n << " bytes\n";
    }
    else
    {
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
      closesocket(tcp_socket);
#else
      close(tcp_socket);
#endif
      return false;
    }
  }

  // close connection to server.
#if defined(VCL_WIN32) && !defined(__CYGWIN__)
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif

#ifdef DEBUG
  vcl_cerr << "HTTP server returned:\n" << contents << '\n';
#endif

  return contents.find("HTTP/1.1 200") != contents.npos;
}


vcl_istream * vul_url::open(const char * url, vcl_ios_openmode mode)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return 0;
  unsigned l = vcl_strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && vcl_strncmp(url, "file://", 7) == 0)
    return new vcl_ifstream(url+7,mode);

  // maybe it's an http URL?
  if (l > 7 && vcl_strncmp(url, "http://", 7) == 0)
    return vul_http_open(url);

  // maybe it's an ftp URL?
  if (l > 6 && vcl_strncmp(url, "ftp://", 6) == 0)
  {
    vcl_cerr << __LINE__ << "ERROR:\n vul_read_url(const char * url)\n"
      "Doesn't support FTP yet, url=" << url << vcl_endl;
    return 0;
  }

  // try an ordinary filename
  return new vcl_ifstream(url, mode);
}


//: Does that URL exist
bool vul_url::exists(const char * url)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return false;
  unsigned l = vcl_strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && vcl_strncmp(url, "file://", 7) == 0)
    return vul_file::exists(url+7);

  // maybe it's an http URL?
  if (l > 7 && vcl_strncmp(url, "http://", 7) == 0)
    return vul_http_exists(url);

  // maybe it's an ftp URL?
  if (l > 6 && vcl_strncmp(url, "ftp://", 6) == 0)
  {
    vcl_cerr << "ERROR: vul_read_url(const char * url)\n"
      "Doesn't support FTP yet, url=" << url << vcl_endl;
    return false;
  }

  // try an ordinary filename
  return vul_file::exists(url);
}

//: Is that a URL
bool vul_url::is_url(const char * url)
{
  // check for null pointer or empty strings.
  if (!url || !*url)
    return false;
  unsigned l = vcl_strlen(url);

  // check for filenames beginning "file:".
  if (l > 7 && vcl_strncmp(url, "file://", 7) == 0)
    return true;

  // maybe it's an http URL?
  if (l > 7 && vcl_strncmp(url, "http://", 7) == 0)
    return true;

  // maybe it's an ftp URL?
  if (l > 6 && vcl_strncmp(url, "ftp://", 6) == 0)
    return true;

  return false;
}

//=======================================================================

bool vul_url::is_file(const char * fn)
{
  if (vul_url::is_url(fn))
    return vul_url::exists(fn);
  else
    return vul_file::exists(fn) && ! vul_file::is_directory(fn);
}

//=======================================================================

static const
int base64_encoding[]=
{
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

static char out_buf[4];

static const char * encode_triplet(char data[3], unsigned n)
{
  assert (n>0 && n <4);
  out_buf[0] = base64_encoding[(data[0] & 0xFC) >> 2];
  out_buf[1] = base64_encoding[
    ((data[0] & 0x3) << 4) + ((data[1] & 0xf0)>>4)];

  if (n==1)
  {
    out_buf[2] = out_buf[3] = '=';
    return out_buf;
  }

  out_buf[2] = base64_encoding[
    ((data[1] & 0xf) << 2) + ((data[2] & 0xc0)>>6)];

  if (n==2)
  {
    out_buf[3] = '=';
    return out_buf;
  }

  out_buf[3] = base64_encoding[ (data[2] & 0x3f) ];
  return out_buf;
}

//=======================================================================

vcl_string vul_url::encode_base64(const vcl_string& in)
{
  vcl_string out;
  unsigned i = 0, line_octets = 0;
  const unsigned l = in.size();
  char data[3];
  while (i <= l)
  {
    if (i == l)
    {
      out.append("=");
      return out;
    }

    data[0] = in[i++];
    data[1] = data[2] = 0;

    if (i == l)
    {
      out.append(encode_triplet(data,1),4);
      return out;
    }

    data[1] = in[i++];

    if (i == l)
    {
      out.append(encode_triplet(data,2),4);
      return out;
    }

    data[2] = in[i++];

    out.append(encode_triplet(data,3),4);

    if (line_octets >= 68/4) // print carriage return
    {
      out.append("\r\n",2);
      line_octets = 0;
    }
    else
      ++line_octets;
  }

  return out;
}

//=======================================================================

static int get_next_char(const vcl_string &in, unsigned int *i)
{
  while (*i < in.size())
  {
    char c;
    c = in[(*i)++];

    if (c == '+')
      return 62;

    if (c == '/')
      return 63;

    if (c >= 'A' && c <= 'Z')
      return 0 + (int)c - (int)'A';

    if (c >= 'a' && c <= 'z')
      return 26 + (int)c - (int)'a';

    if (c >= '0' && c <= '9')
      return 52 + (int)c - (int)'0';

    if (c == '=')
      return 64;
  }
  return -1;
}

//=======================================================================

vcl_string vul_url::decode_base64(const vcl_string& in)
{
  int c;
  char data[3];

  unsigned i=0;
  const unsigned l = in.size();
  vcl_string out;
  while (i < l)
  {
    data[0] = data[1] = data[2] = 0;

    // -=- 0 -=-
    // Search next valid char...
    c = get_next_char(in , &i);

    // treat '=' as end of message
    if (c == 64)
      return out;
    if (c==-1)
      return "";

    data[0] = ((c & 0x3f) << 2) | (0x3 & data[0]);

    // -=- 1 -=-
    // Search next valid char...
    c = get_next_char(in , &i);

      // Error! Second character in octet can't be '='
    if (c == 64 || c==-1)
      return "";

    data[0] = ((c & 0x30) >> 4) | (0xfc & data[0]);
    data[1] = ((c & 0xf) << 4) | (0xf & data[1]);


    // -=- 2 -=-
    // Search next valid char...

    c = get_next_char(in , &i);

    if (c==-1)
      return "";
    if (c == 64)
    {
      // should really read next char and check it is '='
      out.append(data,1);  // write 1 byte to output
      return out;
    }

    data[1] = ((c & 0x3C) >> 2) | (0xf0 & data[1]);
    data[2] = ((c & 0x3) << 6) | (0x3f & data[2]);


    // -=- 3 -=-
    // Search next valid char...
    c = get_next_char(in , &i);

    if (c==-1)
      return "";

    if (c == 64)
    {
      out.append(data,2);  // write 2 bytes to output
      return out;
    }

    data[2] = (c & 0x3f) | (0xc0 & data[2]);

    out.append(data,3);  // write 3 bytes to output
  }

  return out;
}
