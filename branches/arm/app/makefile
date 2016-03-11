# EPOS Application Makefile

include ../makedefs

APP_PATH := $(subst _app,,$(APPLICATION))
ISDIR := $(shell test -d $(APP_PATH) && echo 0)

ifeq ($(ISDIR), 0)
	SRC := $(shell find ./$(APP_PATH)/src/ -mindepth 1 -name "*.cc" -type f)
	OBJS := $(subst .cc,.do,$(SRC))
else
	OBJS :=
endif


all: install

.SUFFIXES: .do .cc

.cc.do:	$(SRC)
		$(CXX) $(CXXFLAGS) -o $@ $<

$(APPLICATION):	$(APPLICATION).o $(LIB)/* $(OBJS)
		$(ALD) $(ALDFLAGS) -o $@ $(APPLICATION).o $(OBJS)

$(APPLICATION).o: $(APPLICATION).cc $(SRC)
		$(ACC) $(ACCFLAGS) -o $@ $<

install: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

clean:
		$(CLEAN) *.o $(APPLICATION) $(OBJS)
