// SimpleTest.cpp : Defines the entry point for the console application.
//
#include "StdAfx.h"
#include <vcl_cstdio.h>
#include <expatpplib.h> // resides in b3p/expatpp
#include <vcl_cstring.h>

class myParser : public expatpp
{
 public:  myParser() : mDepth(0) {}
  virtual void startElement(const XML_Char *name, const XML_Char **atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char *s, int len);

 private:
  void WriteIndent();
  int mDepth;
};


void myParser::WriteIndent()
{
 for (int i=0; i<mDepth; i++)
   vcl_putchar('\t');
}


void myParser::startElement(const char* name, const char** atts)
{
  WriteIndent();
  vcl_puts(name);
  if (atts) {  /* write list of attributes indented below element */
    int i;
    for (i=0; atts[i]; i++) {
      WriteIndent();
      vcl_putchar('-'); vcl_putchar(' ');
      vcl_puts(atts[i]);
    }
  }
  mDepth++;
}


void myParser::endElement(const char*)
{
   mDepth--;
}


void myParser::charData(const XML_Char *s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len==leadingSpace)
    return;  // called with whitespace between elements
  WriteIndent();

  /* write out the user data bracketed by ()*/
  vcl_putchar('(');
  vcl_fwrite(s, len, 1, stdout);
  vcl_puts(")");
}


int main(int argc, char* argv[])
{
  myParser parser;
  char filename[80];
  FILE* xmlFile;
  for (;;) {
    int depth = 0;
    vcl_puts("\n\nXML test: enter filename");
    vcl_gets(filename);
    if (vcl_strlen(filename)==0)
      break;
    xmlFile = vcl_fopen(filename, "r");
    if (!xmlFile)
      break;
    if (!parser.parseFile(xmlFile)) {
      vcl_fprintf(stderr,
                  "%s at line %d\n",
                  XML_ErrorString(parser.XML_GetErrorCode()),
                  parser.XML_GetCurrentLineNumber()
                 );
      return 1;
    }
  }  // loop asking for and parsing files
  vcl_puts("\nfinished!");
  return 0;
}
