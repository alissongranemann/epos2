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

ifeq ($(NO_DOMAIN_2),false)
$(APPLICATION)_domain_2: $(APPLICATION)_domain_2.o
		$(ALD) $(ALDFLAGS) -o $@ $<
endif

ifeq ($(NO_DOMAIN_2),false)
$(APPLICATION)_domain_2.o: $(APPLICATION)_domain_2.cc
		$(ACC) $(ACCFLAGS) -o $@ $<
endif

$(MACH)_install_library: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

$(MACH)_install_builtin: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

ifeq ($(NO_DOMAIN_2),false)
$(MACH)_install_kernel: $(MACH)_loader $(APPLICATION) $(APPLICATION)_domain_2
		$(INSTALL) $(MACH)_loader $(IMG)
		$(INSTALL) $(APPLICATION) $(IMG)
		$(INSTALL) $(APPLICATION)_domain_2 $(IMG)
else
$(MACH)_install_kernel: $(MACH)_loader $(APPLICATION)
		$(INSTALL) $(MACH)_loader $(IMG)
		$(INSTALL) $(APPLICATION) $(IMG)
endif

clean:
		$(CLEAN) *.o $(APPLICATION)
