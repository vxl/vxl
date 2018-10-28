#
# \author Hans J. Johnson
#
# Python script to remove unnecessary <vcl_XXX.h> headers
# that can simply be the C++11 <XXX> headers.
#
# Usage:  python ./scripts/UseStandardHeaders.py $( git grep -l "#include.*vcl_" -- *.h  )
# Usage:  python ./scripts/UseStandardHeaders.py $( git grep -l "#include.*vcl_" -- *.hxx  )
# Usage:  python ./scripts/UseStandardHeaders.py $( git grep -l "#include.*vcl_" -- *.cxx  )


import re
import sys

vcl_headers_to_replace = [
    "vcl_algorithm.h",
    "vcl_bitset.h",
    "vcl_cassert.h",
    "vcl_cctype.h",
    "vcl_cerrno.h",
    "vcl_cfloat.h",
    "vcl_ciso646.h",
    "vcl_climits.h",
    "vcl_clocale.h",
    "vcl_cmath.h",
    "vcl_complex.h",
    "vcl_csetjmp.h",
    "vcl_csignal.h",
    "vcl_cstdarg.h",
    "vcl_cstddef.h",
    "vcl_cstdio.h",
    "vcl_cstdlib.h",
    "vcl_cstring.h",
    "vcl_ctime.h",
    "vcl_cwchar.h",
    "vcl_cwctype.h",
    "vcl_deque.h",
    "vcl_exception.h",
    "vcl_fstream.h",
    "vcl_functional.h",
    "vcl_iomanip.h",
    "vcl_ios.h",
    "vcl_iosfwd.h",
    "vcl_iostream.h",
    "vcl_istream.h",
    "vcl_iterator.h",
    "vcl_limits.h",
    "vcl_list.h",
    "vcl_locale.h",
    "vcl_map.h",
    "vcl_memory.h",
    "vcl_numeric.h",
    "vcl_ostream.h",
    "vcl_queue.h",
    "vcl_set.h",
    "vcl_sstream.h",
    "vcl_stack.h",
    "vcl_stdexcept.h",
    "vcl_streambuf.h",
    "vcl_string.h",
    "vcl_typeinfo.h",
    "vcl_utility.h",
    "vcl_valarray.h",
    "vcl_vector.h"
]


def fix_one_file(filename):
    try:
        with open(filename, 'r', encoding="utf-8") as fid:
            file_as_string = fid.read()
            new_file_as_string = file_as_string

        for curr_vcl_filename in vcl_headers_to_replace:
            quoted_replace = re.compile(r'# *include *"{0}" *'.format(curr_vcl_filename))
            braced_replace = re.compile(r'# *include *< *{0} *> *'.format(curr_vcl_filename))
            new_name = curr_vcl_filename.replace("vcl_", "").replace(".h", "")

            std_compiler_include = ""
            if "<{0}>".format(new_name) not in new_file_as_string:
                std_compiler_include = "#include <{0}>".format(new_name)
            vcl_compiler_include = ""
            if "vcl_compiler.h" not in new_file_as_string:
                vcl_compiler_include = "\n#include <vcl_compiler.h>"

            new_include = "{0}{1}".format(std_compiler_include, vcl_compiler_include)

            new_file_as_string = quoted_replace.sub(new_include, new_file_as_string)
            new_file_as_string = braced_replace.sub(new_include, new_file_as_string)
        if new_file_as_string != file_as_string:
            with open(filename, 'w', encoding="utf-8") as wfid:
                wfid.write(new_file_as_string)
    except:
        print("ERROR: Failed to convert {0}".format(filename))
        pass


for filename in sys.argv[1:]:
    fix_one_file(filename)

#for curr_vcl_filename in vcl_headers_to_replace:
#    print("git rm vcl/{0}".format(curr_vcl_filename))
