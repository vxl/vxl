# Module: identify-os.mk
# Purpose: gnumake include to set OS variable.
# NOTE these have to match the host in configure.in

# Internal: OS
# The OS variable determines the operating system under
# which make is running.  Currently known values are:
# 
#   win32
#   SunOS5 SunOS4
#   IRIX5 IRIX6
#   Linux2
#   freebsd
#   freebsd4
#   HP-UX HP-UX10.20
#   OSF1V3.0 OSF1V3.2 OSF1V4 ULTRIX

# check for win32
# gnumake on win32 uses sh.exe as the shell
# we use bash.exe which will be in the targetjr win32bin directory

ifeq "Windows_NT" "$(OS)"
  uname_output := win32
  OS := win32
  SHELL := bash.exe
else
ifeq "sh.exe" "$(SHELL)"
  uname_output := win32
  OS := win32
  SHELL := bash.exe
else
ifeq "win32" "$(OSTYPE)"
  uname_output := win32
  OS := win32
  SHELL := bash.exe
  COMPILER := VC60
else
ifeq "MSDOS" "$(OSTYPE)"
  uname_output := win32
  OS := win32
  SHELL := bash.exe
  COMPILER := VC60
else
  # Run uname to distinguish unix systems
  uname_output := $(strip $(shell uname -sr))
endif
ifneq (,$(findstring CYGWIN,$(uname_output)))
  uname_output := win32
  OS := win32
  SHELL := bash.exe
  COMPILER := VC60
endif
endif
endif 
endif

ifeq (win32,$(OS))
ifneq "$(BUILDREL)" ""
  COMPILER := VC50.Release
endif
ifeq ($(strip $(IU_COMPILER)),VC60)
  COMPILER := VC60
endif
endif

ifneq (,$(findstring FreeBSD 4,$(uname_output)))
  OS := freebsd4
else
  ifneq (,$(findstring FreeBSD,$(uname_output)))
    OS := freebsd
  endif
endif

ifneq (,$(findstring SunOS 4,$(uname_output)))
  OS := SunOS4
endif

ifneq (,$(findstring SunOS 5,$(uname_output)))
  OS := SunOS5
  OS_MINOR := $(subst SunOS 5.,,$(uname_output))
  # special-case hack for 5.5.1...
  OS_MINOR := $(patsubst 5.1,5,$(OS_MINOR))
endif

ifneq (,$(findstring IRIX 5,$(uname_output)))
  OS := IRIX5
endif

ifneq (,$(findstring IRIX64 6,$(uname_output)))
  OS := IRIX6
  OS_MINOR := $(subst IRIX64 6.,,$(uname_output))
endif

ifneq (,$(findstring IRIX 6,$(uname_output)))
  OS := IRIX6
  OS_MINOR := $(subst IRIX 6.,,$(uname_output))
endif

ifneq (,$(findstring Linux,$(uname_output)))
  OS := Linux2
endif

# I am not sure about this as we only have B.11.00 and
# B.10.20, but it should separate hpux from hpux10.20
# from hpux11.00
ifneq (,$(findstring HP,$(uname_output)))
  ifneq (,$(findstring 10.20,$(uname_output)))
    OS := HP-UX10.20
  else
    ifneq (,$(findstring 11.00,$(uname_output)))
      OS := HP-UX11.00
    else
      OS := HP-UX
    endif
  endif
endif

ifneq (,$(findstring OSF1 V3.0,$(uname_output)))
  OS := OSF1V3.0
endif

ifneq (,$(findstring OSF1 V3.2,$(uname_output)))
  OS := OSF1V3.2
endif

ifneq (,$(findstring OSF1 V4,$(uname_output)))
  OS := OSF1V4
endif

ifneq (,$(findstring ULTRIX,$(uname_output)))
  OS := ULTRIX
endif

