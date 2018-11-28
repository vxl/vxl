// This is core/vil/vil_stream_url.cxx
//:
// \file
// \author fsm

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include "vil_stream_url.h"
#include <vil/vil_stream_core.h>

#include <cassert>
#undef sprintf
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)

# include <unistd.h>       // read(), write(), close()
# include <netdb.h>        // gethostbyname(), sockaddr_in()
# include <sys/socket.h>
# include <netinet/in.h>   // htons()
# define SOCKET int
#elif defined (_WIN32) && !defined(__CYGWIN__)
# include <winsock2.h>
#endif


static const
char base64_encoding[]=
{
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

static char out_buf[4];

static const char * encode_triplet(const char data[3], unsigned n)
{
  assert (n>0 && n <4);
  out_buf[0] = base64_encoding[(data[0] & 0xFC) >> 2];

  if (n==1)
  {
    out_buf[2] = out_buf[3] = '=';
    return out_buf;
  }

  out_buf[1] = base64_encoding[
    ((data[0] & 0x3) << 4) + ((data[1] & 0xf0)>>4)];
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

static std::string encode_base64(const std::string& in)
{
  std::string out;
  unsigned int i = 0, line_octets = 0;
  const auto l = (unsigned int)(in.size());
  char data[3];
  while (i < l)
  {
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


vil_stream_url::vil_stream_url(char const *url)
  : u_(nullptr)
{
  if (std::strncmp(url, "http://", 7) != 0)
    return; // doesn't look like a URL to me....

  char const *p = url+7;
  while (*p && *p!='/')
    ++p;

  // split URL into auth, host, path and port number.
  std::string host = std::string(url+7, p);
  std::string path = (*p) ? p+1 : "";
  std::string auth;
  int port = 80; // default

  // authentication
  for (unsigned int i=0; i<host.size(); ++i)
    if (host[i] == '@') {
      auth = std::string(host.c_str(), host.c_str()+i);
      host = std::string(host.c_str()+i+1, host.c_str() + host.size());
      break;
    }

  // port?
  if (host.size() > 0)
  for (auto i=(unsigned int)(host.size()-1); i>0; --i)
    if (host[i] == ':') {
      port = std::atoi(host.c_str() + i + 1);
      host = std::string(host.c_str(), host.c_str() + i);
      break;
    }

  // do character translation
  for (unsigned k =0; k < path.size(); ++k)
    if (path[k] == ' ')
      path.replace(k, 1, "%20");
    else if (path[k] == '%')
      path.replace(k, 1, "%25");

  // so far so good.
#ifdef DEBUG
  std::cerr << "auth = \'" << auth << "\'\n"
           << "host = \'" << host << "\'\n"
           << "path = \'" << path << "\'\n"
           << "port = " << port << std::endl;
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
  static int called_WSAStartup;
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
                             SOCK_STREAM,  // two-way, reliable, connection-based stream socket.
                             PF_UNSPEC);   // protocol number.

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (tcp_socket == INVALID_SOCKET) {
    std::cerr << __FILE__ ": failed to create socket.\n";
# ifndef NDEBUG
    std::cerr << "error code : " << WSAGetLastError() << std::endl;
# endif
    return;
  }
#else
  if (tcp_socket < 0)
    std::cerr << __FILE__ ": failed to create socket.\n";
#endif

#ifdef DEBUG
  std::cerr << __FILE__ ": tcp_sockect = " << tcp_socket << std::endl;
#endif

  // get network address of server.
  hostent *hp = gethostbyname(host.c_str());
  if (! hp) {
    std::cerr << __FILE__ ": failed to lookup host\n";
#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return;
  }

  // make socket address.
  sockaddr_in my_addr;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);  // convert port number to network byte order..
  std::memcpy(&my_addr.sin_addr, hp->h_addr_list[0], hp->h_length);

  // connect to server.
  if (connect(tcp_socket , (sockaddr *) &my_addr, sizeof my_addr) < 0) {
    std::cerr << __FILE__ ": failed to connect to host\n";
    //perror(__FILE__);
#if defined(_WIN32) && !defined(__CYGWIN__)
    closesocket(tcp_socket);
#else
    close(tcp_socket);
#endif
    return;
  }

  // buffer for data transfers over socket.

  char buffer[4096];

  // send HTTP 1.1 request.
  std::snprintf(buffer, 4090, "GET /%s / HTTP/1.1\r\n", path.c_str());
  if (auth != "")
    std::snprintf(buffer+std::strlen(buffer), 4090-std::strlen(buffer),
                 "Authorization:  Basic %s\n", encode_base64(auth).c_str());

  if (std::snprintf(buffer+std::strlen(buffer), 4090-std::strlen(buffer), "\r\n") < 0)
  {
    std::cerr << "ERROR: vil_stream_url buffer overflow.";
    std::abort();
  }

#if defined(_WIN32) && !defined(__CYGWIN__)
  if (send(tcp_socket, buffer, (int)std::strlen(buffer), 0) < 0)
  {
    std::cerr << __FILE__ ": error sending HTTP request\n";
    closesocket(tcp_socket);
    return;
  }
#else
  if (::write(tcp_socket, buffer, std::strlen(buffer)) < 0)
  {
    std::cerr << __FILE__ ": error sending HTTP request\n";
    close(tcp_socket);
    return;
  }
#endif


//  std::ofstream test2("/test2.jpg", std::ios::binary);

  // read from socket into memory.
  u_ = new vil_stream_core;
  u_->ref();
  {
    unsigned entity_marker = 0; // count end of header CR and LFs
    vil_streampos n;
#if defined(_WIN32) && !defined(__CYGWIN__)
    while ((n = recv(tcp_socket, buffer, sizeof buffer,0 )) > 0L)
#else
    while ((n = ::read(tcp_socket, buffer, sizeof buffer)) > 0L)
#endif
    {
      // search for the CRLFCRLF sequence that marks the end
      // of the http response header
      assert (entity_marker < 5);
      if (entity_marker==4)
      {
        u_->write(buffer, n);
//      test2.write(buffer, n);
      }
      else
      {
        for (vil_streampos i=0; i<n; ++i)
        {
          if ((entity_marker==2||entity_marker==0) && buffer[i]=='\r') entity_marker++;
          else if (entity_marker==1 && buffer[i]=='\n') entity_marker++;
          else if (entity_marker==3 && buffer[i]=='\n')
          {
            entity_marker++;
            u_->write(buffer+i+1, n-i-1);
//            test2.write(buffer+i+1, n-i-1);
            break;
          }
          else entity_marker=0;
        }
      }
    }
  }

#if 0 // useful for figuring out where the error is
  char btest[4096];
  std::ofstream test("/test.jpg", std::ios::binary);
  u_->seek(0L);
  while (vil_streampos bn = u_->read(btest, 4096L))
    test.write(btest, bn);
  test.close();
#endif


  // close connection to server.
#if defined(_WIN32) && !defined(__CYGWIN__)
  closesocket(tcp_socket);
#else
  close(tcp_socket);
#endif
}

vil_stream_url::~vil_stream_url()
{
  if (u_) {
    u_->unref();
    u_ = nullptr;
  }
}
