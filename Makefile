#
# TODO: Move `libmongoclient.a` to /usr/local/lib so this can work on production servers
#
 
CC := g++# This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
TARGET := bin/calibrator
 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -O2 -g -Wall -std=c++11 -fmessage-length=0 
LIB := -L ~/boost/lib  -lboost_program_options -lboost_system
INC := -I ~/boost

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(OBJECTS)"
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Spikes
ticket:
	$(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

#CXX = g++
#CXXFLAGS = -O2 -g -Wall -std=c++11 -fmessage-length=0
#INCLUDES := -I ~/boost
#LD := g++
#LDFLAGS := -L ~/boost/lib -lboost_program_options
#SOURCES := $(shell find src/ -name '*.cpp' -print | sort)
#OBJECTS := $(SOURCES:.cpp=.o)
#TARGETS = main cal_curve uncal_date cal_date

##%.o:%.cpp
##	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

##all: $(TARGETS)

##main: main.o $(OBJECTS)
##	$(LD) $(LDFLAGS) -o $@ $<
##	chmod 755 $@

##clean:
##	rm -f $(TARGETS) $(OBJECTS)



#all: main

#bin/main: main.o cal_curve.o uncal_date.o cal_date.o uncal_date_list.o cal_date_list.o sigma_range.o
#	$(LD) $(LDFLAGS) -o $@ $< cal_curve.o uncal_date.o cal_date.o uncal_date_list.o cal_date_list.o sigma_range.o
#	chmod 755 $@

##main: main.o cal_curve.o uncal_date.o cal_date.o
##	g++ main.o cal_curve.o uncal_date.o cal_date.o -o main

#build/main.o: src/main.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/main.cpp

##json.o: lib/json.hpp
##	$(CXX) $(CXXFLAGS) $(INCLUDES) -c lib/json.hpp

#build/cal_curve.o: src/cal_curve.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/cal_curve.cpp

#build/uncal_date.o: src/uncal_date.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/uncal_date.cpp

#build/uncal_date_list.o: src/uncal_date_list.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/uncal_date_list.cpp

#build/cal_date.o: src/cal_date.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/cal_date.cpp

#build/cal_date_list.o: src/cal_date_list.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/cal_date_list.cpp

#build/sigma_range.o: src/sigma_range.cpp
#	$(CXX) $(CXXFLAGS) $(INCLUDES) -c src/sigma_range.cpp

#clean:
#	rm -rf build/*o main
