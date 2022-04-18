# Tell Make that these are NOT files, just targets
.PHONY: all install test uninstall clean sst-info sst-help help

# shortcut for running anything inside the singularity container
CONTAINER=singularity exec 2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif

# SST environment variables (gathered from the singularity container)
CXX=g++
CXXFLAGS=-std=c++1y -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I/opt/SST/11.1.0/include
LDFLAGS =-shared -fno-common -Wl,-undefined -Wl,dynamic_lookup

# Grab all the .cpp files, put objs and depends in the .build folder
SRC=$(wildcard *.cpp)
OBJ=$(SRC:%.cpp=.build/%.o)
DEP=$(OBJ:%.o=%.d)

# Name of the library we will be working with
PACKAGE=deadlock

# Simply typing "make" calls this by default, so everything gets built and installed
all: test

# initialize the singularity container and make sure the sstsimulator.conf exists
# You shouldn't call this target directly, it gets called by other targets
2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif:
	git submodule init
	git submodule update
	mkdir -p ~/.sst
	touch ~/.sst/sstsimulator.conf

# Use the dependencies when compiling to check for header file changes
# You shouldn't call this target directly, it gets called by other targets
-include $(DEP)
.build/%.o: %.cpp
	mkdir -p $(@D)
	$(CONTAINER) $(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Link all the objects to create the library
# You shouldn't call this target directly, it gets called by other targets
lib$(PACKAGE).so: $(OBJ)
	$(CONTAINER) $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^



# User callable targets below:

# Register the model with SST
install: 2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif lib$(PACKAGE).so
	$(CONTAINER) sst-register $(PACKAGE) $(PACKAGE)_LIBDIR=$(CURDIR)

# Run the tests for the model
test: 2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif install
	$(CONTAINER) echo "Run tests here"

# Unregister the model with SST
uninstall: 2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif
	$(CONTAINER) sst-register -u $(PACKAGE)

# Remove the build files and the library
clean: uninstall
	rm -rf .build *.so

sst-info: 2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif
	$(CONTAINER) sst-info $(arg)

sst-help: 2022HPCSummer-SST/sstpackage-11.1.0-ubuntu-20.04.sif
	$(CONTAINER) sst --help

help:
	@echo "Target     | Description"
	@echo "-----------+-------------------------------------------------------"
	@echo "install    | Builds all .cpp files into a library lib$(PACKAGE).so,"
	@echo "           |  then registers the package with SST"
	@echo "           |"
	@echo "test       | Runs tests"
	@echo "           |"
	@echo "uninstall  | Un-registers the package with SST"
	@echo "           |"
	@echo "clean      | Cleans up the .build folder (.o and .d files) and"
	@echo "           |  removes the library .so file"
	@echo "           |"
	@echo "sst-info   | Runs sst-info to show what packages SST knows of."
	@echo "           |  You can also pass an argument to this target to only"
	@echo "           |  ask sst-info for a specific package or model."
	@echo "           |  For example: make sst-info arg=$(PACKAGE)"
	@echo "           |  For example: make sst-info arg=merlin.hr_router"
	@echo "           |"
	@echo "sst-help   | Runs sst --help to show available options when running"
	@echo "           |  sst (run modes, output graphs, threads, etc.)"
