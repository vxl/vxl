#ifndef  _QV_READ_ERROR
#define  _QV_READ_ERROR

// mpichler, 19950713: allow redirection of error messages
typedef void (*QvReadErrorCallback) (const char* /* error */, const char* /* location */);
// suggested output format: "VRML read error in %s\n%s\n", error, location

class QvInput;

class QvReadError {
  public:
    static void post(const QvInput *in, const char *formatString ...);

    static QvReadErrorCallback callback_;
};

#endif /* _QV_READ_ERROR */
