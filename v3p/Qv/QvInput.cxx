#include "QvInput.h"
#include <vcl_cctype.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include "QvDebugError.h"
#include "QvReadError.h"
#include "QvNode.h"

#define CURVERSION 1.0  // Current version of file format
#define COMMENT_CHAR '#'

static const int numValidASCIIHeaders = 1;
struct headerStorage
{
    const char *string;
    float version;
};

static const headerStorage ASCIIHeader[1] = {
    { "#VRML V1.0 ascii", /* 20 chars */ 1.0 },
};

float
QvInput::isASCIIHeader(const char *string)
{
// gorasche: allow trailing garbage in header (conforming to VRML spec)
    for (int i = 0; i < numValidASCIIHeaders; i++) {
//      if (strcmp(ASCIIHeader[i].string, string) == 0)
        if (!vcl_strncmp(ASCIIHeader[i].string, string,vcl_strlen(ASCIIHeader[i].string)))
            return ASCIIHeader[i].version;
    }
    vcl_fprintf(stderr, "Qv: NOT ASCII\n");
    return FALSE;
}

QvInput::QvInput()
{
    setFilePointer(stdin);
}

QvInput::~QvInput()
{
}

void
QvInput::setFilePointer(vcl_FILE *newFP)
{
    fp = newFP;
    lineNum = 1;
    version = CURVERSION;
    readHeader = FALSE;
    headerOk = TRUE;
    backBufIndex = -1;
}

float
QvInput::getVersion()
{
    if (! readHeader)
        (void) checkHeader();

    return version;
}

QvBool
QvInput::get(char &c)
{
    QvBool ret;

    if (backBufIndex >= 0) {
        c = backBuf.getString()[backBufIndex++];

        if (c != '\0')
            return TRUE;

        backBuf.makeEmpty();
        backBufIndex = -1;
    }

    if (! readHeader && ! checkHeader())
        return FALSE;

    if (eof()) {
        c = (char)EOF;
        ret = FALSE;
    }

    else {
        int i = getc(fp);

        if (i == EOF) {
            c = (char)EOF;
            ret = FALSE;
        }
        else {
            c = (char) i;
            ret = TRUE;
        }
    }

    return ret;
}

QvBool
QvInput::read(char &c)
{
    return skipWhiteSpace() && get(c);
}

QvBool
QvInput::read(QvString &s)
{
    if (! skipWhiteSpace())
        return FALSE;

    QvBool      quoted;
    char        c;
    char        bufStore[256];
    char        *buf;
    int         bytesLeft;

    s.makeEmpty();

    if (! get(c))
        return FALSE;

    quoted = (c == '\"');
    if (! quoted)
        putBack(c);

    do {
        buf       = bufStore;
        bytesLeft = sizeof(bufStore) - 1;

        while (bytesLeft > 0) {

            if (! get(*buf))
                break;

            if (quoted) {
                if (*buf == '\"')
                    break;

                if (*buf == '\\') {
                    if ((get(c)) && c == '\"')
                        *buf = '\"';
                    else
                        putBack(c);
                }

                if (*buf == '\n')
                    lineNum++;
            }

            else if (vcl_isspace(*buf)) {
                putBack(*buf);
                break;
            }

            buf++;
            bytesLeft--;
        }
        *buf = '\0';

        s += bufStore;

    } while (bytesLeft == 0);

    vcl_fprintf(stderr, "QV read[%s]\n", s.getString());

    return TRUE;
}

QvBool
QvInput::read(QvName &n, QvBool validIdent)
{
    QvBool gotChar;

    if (! skipWhiteSpace())
        return FALSE;

    if (! validIdent) {
        QvString s;

        if (! read(s))
            return FALSE;

        n = s;
    }

    else {
        char buf[256];
        char *b = buf;
        char c;

        if ((gotChar = get(c)) && QvName::isIdentStartChar(c)) {
            *b++ = c;

            while ((gotChar = get(c)) && QvName::isIdentChar(c)) {
                if (b - buf < 255)
                    *b++ = c;
            }
        }
        *b = '\0';

        if (gotChar)
            putBack(c);

        n = buf;
    }

    return TRUE;
}

#define READ_NUM(reader, readType, num, type)               \
    QvBool ok;                                              \
    if (! skipWhiteSpace())                                 \
    ok = FALSE;                                             \
    else {                                                  \
        readType _tmp;                                      \
        ok = reader(_tmp);                                  \
        if (ok)                                             \
            num = (type) _tmp;                              \
    }                                                       \
    return ok

#define READ_INTEGER(num, type)                             \
    READ_NUM(readInteger, long, num, type)

#define READ_UNSIGNED_INTEGER(num, type)                    \
    READ_NUM(readUnsignedInteger, unsigned long, num, type)

#define READ_REAL(num, type)                                \
    READ_NUM(readReal, double, num, type)

QvBool
QvInput::read(int &i)
{
    READ_INTEGER(i, int);
}

QvBool
QvInput::read(unsigned int &i)
{
    READ_UNSIGNED_INTEGER(i, unsigned int);
}

QvBool
QvInput::read(short &s)
{
    READ_INTEGER(s, short);
}

QvBool
QvInput::read(unsigned short &s)
{
    READ_UNSIGNED_INTEGER(s, unsigned short);
}

QvBool
QvInput::read(long &l)
{
    READ_INTEGER(l, long);
}

QvBool
QvInput::read(unsigned long &l)
{
    READ_UNSIGNED_INTEGER(l, unsigned long);
}

QvBool
QvInput::read(float &f)
{
    READ_REAL(f, float);
}

QvBool
QvInput::read(double &d)
{
    READ_REAL(d, double);
}

QvBool
QvInput::eof() const
{
    return feof(fp);
}

void
QvInput::getLocationString(QvString &string) const
{
    char buf[128];
    vcl_sprintf(buf, "        Occurred at line %3d", lineNum);
    string = buf;
}

void
QvInput::putBack(char c)
{
    if (c == (char) EOF)
        return;

    if (backBufIndex >= 0)
        --backBufIndex;
    else
        ungetc(c, fp);
}

void
QvInput::putBack(const char *string)
{
    backBuf = string;
    backBufIndex = 0;
}

QvBool
QvInput::checkHeader()
{
    char c;

    readHeader = TRUE;

    if (get(c)) {
        if (c == COMMENT_CHAR) {
            char buf[256];
            int  i = 0;

            buf[i++] = c;
            while (get(c) && c != '\n')
              if (c != '\r' && i < 255)  // care for dos files. mpichler, 19950627
                buf[i++] = c;
            buf[i] = '\0';
            if (c == '\n')
                lineNum++;

            if ((version = isASCIIHeader(buf)))
                return TRUE;
        }

        else
            putBack(c);
    }
// mpichler: give no error massage on wrong headers (used for header identification)
    QvReadError::post(this, "File does not have a valid header string");
    headerOk = FALSE;
    return FALSE;
}

QvBool
QvInput::skipWhiteSpace()
{
    char   c;
    QvBool gotChar;

    if (! readHeader && ! checkHeader())
        return FALSE;

    while (TRUE) {

        while ((gotChar = get(c)) && vcl_isspace(c))
            if (c == '\n')
                lineNum++;

        if (! gotChar)
            break;

        if (c == COMMENT_CHAR) {
            while (get(c) && c != '\n')
                ;

            if (eof())
                QvReadError::post(this, "EOF reached before end of comment");
            else
                lineNum++;
        }
        else {
            putBack(c);
            break;    }
    }

    return TRUE;
}

QvBool
QvInput::readInteger(long &l)
{
    char str[32];
    char *s = str;

    if (readChar(s, '-') || readChar(s, '+'))
        s++;

    if (! readUnsignedIntegerString(s))
        return FALSE;

    l = vcl_strtol(str, NULL, 0);

    return TRUE;
}

QvBool
QvInput::readUnsignedInteger(unsigned long &l)
{
    char str[32];
    if (! readUnsignedIntegerString(str))
        return FALSE;

    l = vcl_strtoul(str, NULL, 0);

    return TRUE;
}

QvBool
QvInput::readUnsignedIntegerString(char *str)
{
    int  minSize = 1;
    char *s = str;

    if (readChar(s, '0')) {

        if (readChar(s + 1, 'x')) {
            s += 2 + readHexDigits(s + 2);
            minSize = 3;
        }

        else
            s += 1 + readDigits(s + 1);
    }

    else
        s += readDigits(s);

    if (s - str < minSize)
        return FALSE;

    *s = '\0';

    return TRUE;
}

QvBool
QvInput::readReal(double& d)
{
    char        str[32];
    int         n;
    char        *s = str;
    QvBool      gotNum = FALSE;

/* begin hard hat area */

#if 0
    // may try something like this to speed up things:
    if (backBufIndex < 0)  // backbuf empty
        return vcl_fscanf(fp, "%lf", &d) == 1;  // EOF considered as failure
    else
        vcl_fprintf (vcl_stderr, "reading real from backbuffer ");
#endif

/* end hard hat area */


    n = readChar(s, '-');
    if (n == 0)
        n = readChar(s, '+');
    s += n;

    if ((n = readDigits(s)) > 0) {
        gotNum = TRUE;
        s += n;
    }

    if (readChar(s, '.') > 0) {
        s++;

        if ((n = readDigits(s)) > 0) {
            gotNum = TRUE;
            s += n;
        }
    }

    if (! gotNum)
        return FALSE;

    n = readChar(s, 'e');
    if (n == 0)
        n = readChar(s, 'E');

    if (n > 0) {
        s += n;

        n = readChar(s, '-');
        if (n == 0)
            n = readChar(s, '+');
        s += n;

        if ((n = readDigits(s)) > 0)
            s += n;

        else
            return FALSE;    }

    *s = '\0';

    d = vcl_atof(str);

    return TRUE;
}

int
QvInput::readDigits(char *string)
{
    char c, *s = string;

    while (get(c)) {

        if (vcl_isdigit(c))
            *s++ = c;

        else {
            putBack(c);
            break;
        }
    }

    return int(s - string);
}

int
QvInput::readHexDigits(char *string)
{
    char c, *s = string;

    while (get(c)) {

        if (vcl_isxdigit(c))
            *s++ = c;

        else {
            putBack(c);
            break;
        }
    }

    return int(s - string);
}

int
QvInput::readChar(char *string, char charToRead)
{
    char        c;
    int         ret;

    if (! get(c))
        ret = 0;

    else if (c == charToRead) {
        *string = c;
        ret = 1;
    }

    else {
        putBack(c);
        ret = 0;
    }

    return ret;
}

void
QvInput::addReference(const QvName &name, QvNode *node)
{
    refDict.enter( name.getString(), (void *) node);

    node->setName(name);
}

QvNode *
QvInput::findReference(const QvName &name) const
{
    void *node;

    if (refDict.find( name.getString(), node))
        return (QvNode*)node;

    return NULL;
}
