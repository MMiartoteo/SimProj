#
# OMNeT++/OMNEST Makefile for Symphony
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out
#

# Name of target to be created (-o option)
TARGET = Symphony$(EXE_SUFFIX)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = \
    -I. \
    -Irelazione \
    -Irelazione/imgs \
    -Irelazione/v2-acmlarge \
    -Iresults \
    -Iresults/norelink_concurrent-joins--stability \
    -Iresults/norelink_join-freq--stability \
    -Iresults/test_join_cost1 \
    -Iresults/test_join_cost2 \
    -Iresults/test_n_hops \
    -Iresults/test_stability2_concurrentjoins1 \
    -Iresults/test_stability_joinfreq1 \
    -Iresults/test_stability_joinfreq2 \
    -Iresults/test_stability_joinfreq3 \
    -Iresults/test_stability_joinfreq3_norelink

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc and .msg files
OBJS = $O/churner.o $O/peer.o $O/Msgs_m.o

# Message files
MSGFILES = \
    Msgs.msg

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIB_SUBDIR = $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
OMNETPP_LIBS = -L"$(OMNETPP_LIB_SUBDIR)" -L"$(OMNETPP_LIB_DIR)" -loppmain$D $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ($(MAKECMDGOALS),depend)
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $O/$(TARGET)
	$(LN) $O/$(TARGET) .

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile
	@$(MKPATH) $O
	$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

.PHONY: all clean cleanall depend msgheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE)
	@$(MKPATH) $(dir $@)
	$(CXX) -c $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(MSGC) -s _m.cc $(MSGCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

clean:
	-rm -rf $O
	-rm -f Symphony Symphony.exe libSymphony.so libSymphony.a libSymphony.dll libSymphony.dylib
	-rm -f ./*_m.cc ./*_m.h
	-rm -f relazione/*_m.cc relazione/*_m.h
	-rm -f relazione/imgs/*_m.cc relazione/imgs/*_m.h
	-rm -f relazione/v2-acmlarge/*_m.cc relazione/v2-acmlarge/*_m.h
	-rm -f results/*_m.cc results/*_m.h
	-rm -f results/norelink_concurrent-joins--stability/*_m.cc results/norelink_concurrent-joins--stability/*_m.h
	-rm -f results/norelink_join-freq--stability/*_m.cc results/norelink_join-freq--stability/*_m.h
	-rm -f results/test_join_cost1/*_m.cc results/test_join_cost1/*_m.h
	-rm -f results/test_join_cost2/*_m.cc results/test_join_cost2/*_m.h
	-rm -f results/test_n_hops/*_m.cc results/test_n_hops/*_m.h
	-rm -f results/test_stability2_concurrentjoins1/*_m.cc results/test_stability2_concurrentjoins1/*_m.h
	-rm -f results/test_stability_joinfreq1/*_m.cc results/test_stability_joinfreq1/*_m.h
	-rm -f results/test_stability_joinfreq2/*_m.cc results/test_stability_joinfreq2/*_m.h
	-rm -f results/test_stability_joinfreq3/*_m.cc results/test_stability_joinfreq3/*_m.h
	-rm -f results/test_stability_joinfreq3_norelink/*_m.cc results/test_stability_joinfreq3_norelink/*_m.h

cleanall: clean
	-rm -rf $(PROJECT_OUTPUT_DIR)

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile -P\$$O/ -- $(MSG_CC_FILES)  ./*.cc relazione/*.cc relazione/imgs/*.cc relazione/v2-acmlarge/*.cc results/*.cc results/norelink_concurrent-joins--stability/*.cc results/norelink_join-freq--stability/*.cc results/test_join_cost1/*.cc results/test_join_cost2/*.cc results/test_n_hops/*.cc results/test_stability2_concurrentjoins1/*.cc results/test_stability_joinfreq1/*.cc results/test_stability_joinfreq2/*.cc results/test_stability_joinfreq3/*.cc results/test_stability_joinfreq3_norelink/*.cc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/Msgs_m.o: Msgs_m.cc \
	Msgs_m.h
$O/churner.o: churner.cc \
	churner.h \
	peer.h
$O/peer.o: peer.cc \
	Msgs_m.h \
	churner.h \
	peer.h

