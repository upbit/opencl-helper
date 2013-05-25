DEPTH = ../../../../..

include $(DEPTH)/make/openclsdkdefs.mk 

####
#
#  Targets
#
####

OPENCL			= 1
SAMPLE_EXE		= 0
EXE_TARGET 		= demo_iobuffer
#EXE_TARGET_INSTALL   	= 

####
#
#  C/CPP files
#
####

FILES 	= iobuffer_demo
CLFILES	= kernel.cl

include $(DEPTH)/make/openclsdkrules.mk 

