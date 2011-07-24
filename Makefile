
# The extension we're using for our source files
cxx_ext = .cpp
# The master list of our source files
sources = main.cpp simulator.cpp event.cpp sim_time.cpp \
	timer.cpp node.cpp rand_num_generator.cpp \
	log_stream_manager.cpp packet.cpp communication_layer.cpp \
	wireless_channel_manager.cpp channel.cpp physical_layer.cpp \
	signal.cpp wireless_comm_signal.cpp location.cpp \
	rfid_reader_phy.cpp rfid_tag_phy.cpp \
	path_loss.cpp fading.cpp wireless_channel.cpp \
	application_layer.cpp rfid_tag_app.cpp rfid_reader_app.cpp \
	link_layer.cpp rfid_reader_mac.cpp rfid_tag_mac.cpp \
	mac_protocol.cpp
# The following three variables are used for make dist
# Master list of header files we've created
headers = simulator.hpp event.hpp sim_time.hpp \
	utility.hpp timer.hpp node.hpp rand_num_generator.hpp \
	log_stream_manager.hpp packet.hpp communication_layer.hpp \
	wireless_channel_manager.hpp channel.hpp physical_layer.hpp \
	signal.hpp wireless_comm_signal.hpp location.hpp \
	rfid_reader_phy.hpp rfid_tag_phy.hpp \
	path_loss.hpp fading.hpp wireless_channel.hpp \
	application_layer.hpp rfid_tag_app.hpp rfid_reader_app.hpp \
	link_layer.hpp rfid_reader_mac.hpp rfid_tag_mac.hpp \
	mac_protocol.hpp simulation_end_listener.hpp
# File whose first line contains the current version number
# for the project
version_file = VERSION
# Non-source and header files that should be included in 
# make dist
aux_files = Makefile $(version_file) Doxyfile Jamfile \
	boost-build.jam Jamrules project-root.jam python_wrapper.cpp
aux_dirs = doc/
# The name of the executable we produce
exefile = rfidsim


# Define which compiler we are using
CXX = g++
# Give the command line options for the compiler:
# -g = add debugging info
# -Wall = turn on all warnings
CXXFLAGS = -g -Wall
# Any macros to define:
# -DNDEBUG = turn off assert's
CXXDEFINES =
# Add any additional directories to search for header files
# -I/new/directory
CXXINCLUDES =

# Define which linker we are using
LD = $(CXX)
# Command line options for the linker
LDFLAGS =
# Libraries we need to link
LDLIBS =

# Uncomment this line to test what make clean would remove
#RM = @echo would rm

# Compile the sources.  This expression comes from the GNU Make 
# manual section "Generating Prerequisites Automatically".
# Basically, the source files are automatically preprocessed
# for dependencies (via -MM switch) and the include at the 
# bottom of the file will then run the Makefiles produced
# by this step.
#
# -e causes make do exit immediately if any of the commands fail.
#
# -MM option creates dependency files but excludes system
# headers.
#
# sed command translates the form:
# main.o : main.c defs.h
# into:
# main.o main.d : main.c defs.h
# which makes the .d file depend on all the source and header 
# files that the .o file depends on.
#
# $@ = name of the target
# $< = name of the first prerequisite
# $$$$ = creates temp files of the form name.$$
%.d: %$(cxx_ext)
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# Link all the object files together with any other libraries
# that we may need.
# $^ = name of all the prerequisites with spaces between
$(exefile): $(sources:$(cxx_ext)=.o)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

# This runs each of the .d makefiles by using subsitution
# to translate each source files name into a corresponding 
# .d file name.
include $(sources:$(cxx_ext)=.d)

# Create a tar.gz file by reading the first line of
# the $version_file, creating a directory with the version
# number appended to the file name.  Create hard links
# to all desired files in the directory, create the tar
# file, and delete the directory
.PHONY: dist
dist: $(sources) $(headers) $(aux_files)
	echo $(exefile)-`sed -e q $(version_file)` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	ln $(sources) $(headers) $(aux_files) `cat .fname`
	cp -r $(aux_dirs) `cat .fname`
	tar -czvf `cat .fname`.tar.gz `cat .fname`
	rm -rf `cat .fname` .fname

# Remove all the .o and .d files corresponding to the
# source file names along with some other files we don't
# need.
.PHONY: clean
clean:
	$(RM) $(sources:$(cxx_ext)=.o) $(sources:$(cxx_ext)=.d) \
	$(exefile) core.*

