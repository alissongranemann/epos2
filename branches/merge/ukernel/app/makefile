# EPOS Application Makefile

include ../makedefs

all: install

$(MACH)_loader:	$(MACH)_loader.o
		$(ALD) $(ALDFLAGS) -o $@ $<

$(MACH)_loader.o: $(MACH)_loader.cc
		$(ACC) $(ACCFLAGS) -o $@ $<

$(APPLICATION):	$(APPLICATION).o $(LIB)/*
		$(ALD) $(ALDFLAGS) -o $@ $(APPLICATION).o

$(APPLICATION).o: $(APPLICATION).cc $(SRC)
		$(ACC) $(ACCFLAGS) -o $@ $<

$(MACH)_install_library: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

$(MACH)_install_builtin: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

$(MACH)_install_kernel: $(MACH)_loader $(APPLICATION)
		$(INSTALL) $(MACH)_loader $(IMG)
		$(INSTALL) $(APPLICATION) $(IMG)

install: $(MACH)_install_$(MODE)

clean:
		$(CLEAN) *.o $(APPLICATION)