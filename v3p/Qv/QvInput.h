#ifndef  _QV_INPUT_
#define  _QV_INPUT_

#include "QvDict.h"
#include "QvString.h"
#include <vcl_cstdio.h>

class QvNode;
class QvDB;

class QvInput {
 public:

    QvInput();
    ~QvInput();

    static float        isASCIIHeader(const char *string);
    void                setFilePointer(vcl_FILE *newFP);
    vcl_FILE *          getCurFile() const { return fp; }
    float               getVersion();
    QvBool              get(char &c);
    QvBool              read(char           &c);
    QvBool              read(QvString       &s);
    QvBool              read(QvName         &n, QvBool validIdent = FALSE);
    QvBool              read(int            &i);
    QvBool              read(unsigned int   &i);
    QvBool              read(short          &s);
    QvBool              read(unsigned short &s);
    QvBool              read(long           &l);
    QvBool              read(unsigned long  &l);
    QvBool              read(float          &f);
    QvBool              read(double         &d);
    QvBool              eof() const;
    void                getLocationString(QvString &string) const;
    void                putBack(char c);
    void                putBack(const char *string);
    void                addReference(const QvName &name, QvNode *node);
    QvNode *            findReference(const QvName &name) const;

  private:
    vcl_FILE            *fp;            // File pointer
    int                 lineNum;        // Number of line currently reading
    float               version;        // Version number of file
    QvBool              readHeader;     // TRUE if header was checked for A/B
    QvBool              headerOk;       // TRUE if header was read ok
    QvDict              refDict;        // Node reference dictionary
    QvString            backBuf;
    int                 backBufIndex;

    QvBool              checkHeader();

    QvBool              skipWhiteSpace();

    QvBool              readInteger(long &l);
    QvBool              readUnsignedInteger(unsigned long &l);
    QvBool              readReal(double &d);
    QvBool              readUnsignedIntegerString(char *str);
    int                 readDigits(char *string);
    int                 readHexDigits(char *string);
    int                 readChar(char *string, char charToRead);

friend class QvNode;
friend class QvDB;
};

#endif /* _QV_INPUT_ */
