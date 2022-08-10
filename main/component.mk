#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

CFLAGS+= -DLV_CONF_INCLUDE_SIMPLE

COMPONENT_SRCDIRS := . src

COMPONENT_ADD_INCLUDEDIRS := . inc