#ifndef _QV_STRING_
#define _QV_STRING_

#include "QvBasic.h"
#include <vcl_cstring.h>

class QvString {
  public:
    QvString()                   { string = staticStorage; string[0] = '\0'; }
    QvString(const char *str)    { string = staticStorage; *this = str; }
    QvString(const QvString &str) {string = staticStorage; *this = str.string;}
    ~QvString();
    u_long              hash()          { return QvString::hash(string); }
    int                 getLength() const       { return vcl_strlen(string); }
    void                makeEmpty(QvBool freeOld = TRUE);
    const char *        getString() const       { return string; }
    QvString &          operator =(const char *str);
    QvString &          operator =(const QvString &str)
        { return (*this = str.string); }
    QvString &          operator +=(const char *str);
    bool                operator !() const { return (string[0] == '\0'); }
    friend bool         operator ==(const QvString &str, const char *s);

    friend bool         operator ==(const char *s, const QvString &str)
        { return (str == s); }

    friend bool         operator ==(const QvString &str1, const QvString &str2)
        { return (str1 == str2.string); }
    friend bool         operator !=(const QvString &str, const char *s);

    friend bool         operator !=(const char *s, const QvString &str)
        { return (str != s); }
    friend bool         operator !=(const QvString &str1,
                                    const QvString &str2)
        { return (str1 != str2.string); }
    static u_long       hash(const char *s);
  private:
    char                *string;
    int                 storageSize;
#define QV_STRING_STATIC_STORAGE_SIZE           32
    char                staticStorage[QV_STRING_STATIC_STORAGE_SIZE];
    void                expand(int bySize);
};

struct QvNameChunk;  /* mpichler, 19950616 */

class QvNameEntry {
 public:
    QvBool              isEmpty() const   { return (string[0] == '\0'); }
    QvBool              isEqual(const char *s) const
        { return (string[0] == s[0] && ! vcl_strcmp(string, s)); }
 private:
    static int          nameTableSize;
    static QvNameEntry  **nameTable;
    static struct QvNameChunk *chunk;
    const char          *string;
    u_long              hashValue;
    QvNameEntry         *next;
    static void         initClass();
    QvNameEntry(const char *s, u_long h, QvNameEntry *n)
        { string = s; hashValue = h; next = n; }
    static const QvNameEntry *  insert(const char *s);

friend class QvName;
};

class QvName {
  public:
    QvName();
    QvName(const char *s)               { entry = QvNameEntry::insert(s); }
    QvName(const QvString &s)   { entry = QvNameEntry::insert(s.getString()); }

    QvName(const QvName &n)                     { entry = n.entry; }
    ~QvName()                                   {}
    const char          *getString() const      { return entry->string; }
    int                 getLength() const   { return vcl_strlen(entry->string); }
    static QvBool       isIdentStartChar(char c);
    static QvBool       isIdentChar(char c);
    static QvBool       isNodeNameStartChar(char c);
    static QvBool       isNodeNameChar(char c);
    bool                operator !() const   { return entry->isEmpty() != 0; }
    friend bool         operator ==(const QvName &n, const char *s)
        { return n.entry->isEqual(s) != 0; }
    friend bool         operator ==(const char *s, const QvName &n)
        { return n.entry->isEqual(s) != 0; }

    friend bool         operator ==(const QvName &n1, const QvName &n2)
        { return n1.entry == n2.entry; }
    friend bool         operator !=(const QvName &n, const char *s)
        { return ! n.entry->isEqual(s); }
    friend bool         operator !=(const char *s, const QvName &n)
        { return ! n.entry->isEqual(s); }

    friend bool         operator !=(const QvName &n1, const QvName &n2)
        { return n1.entry != n2.entry; }
  private:
    const QvNameEntry   *entry;
};

#endif /* _QV_STRING_ */
