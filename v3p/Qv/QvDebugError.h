#ifndef  _QV_DEBUG_ERROR
#define  _QV_DEBUG_ERROR

// mpichler, 19950713: allow redirection of error messages
typedef void (*QvDebugErrorCallback) (const char* /* method */, const char* /* errormsg */);
// suggested output format: "VRML error in %s(): %s\n", method, errormsg

class QvDebugError {
  public:
    static void post(const char *methodName, const char *formatString ...);

    static QvDebugErrorCallback callback_;
};

#endif /* _QV_DEBUG_ERROR */
