
# Set make flags for noshared build
# don hamilton changed this to use a late binding for situations where the
# optimization values need to be revised
CGFLAGS = $(optimize) $(debug_for_optimize)

# ns-link-static is used for libs that should be statically linked on
# some builds but dynamically on others.  Normally, this means libg++ etc.
ns-link-static := $(link-static)
ns-link-dynamic := $(link-dynamic)
