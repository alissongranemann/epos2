# EPOS Application Makefile

include ../makedefs

install: $(MACH)_install_$(MODE)

$(MACH)_loader:	$(MACH)_loader.o
		$(ALD0) $(ALDFLAGS) -o $@ $<

$(MACH)_loader.o: $(MACH)_loader.cc
		$(ACC) $(ACCFLAGS) -o $@ $<

$(APPLICATION):	$(APPLICATION).o $(LIB)/*
		$(ALD) $(ALDFLAGS) -o $@ $(APPLICATION).o

$(APPLICATION).o: $(APPLICATION).cc $(SRC)
		$(ACC) $(ACCFLAGS) -o $@ $<

domain_2: domain_2.o
		$(ALD) $(ALDFLAGS) -o $@ $<

domain_2.o: domain_2.cc
		$(ACC) $(ACCFLAGS) -o $@ $<

$(MACH)_install_library: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

$(MACH)_install_builtin: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

$(MACH)_install_kernel: $(MACH)_loader $(APPLICATION) domain_2
		$(INSTALL) $(MACH)_loader $(IMG)
		$(INSTALL) $(APPLICATION) $(IMG)
		$(INSTALL) domain_2 $(IMG)

clean:
		$(CLEAN) *.o $(APPLICATION)
