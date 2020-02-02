CC          := gcc
CXX         := g++
CXXFLAGS    := -g --std=c++17
BUILDDIR	:= build
OBJSDIR		:= $(BUILDDIR)/obj

SRCS=tableau.cpp sat.cpp
OBJS := $(SRCS:%.cpp=$(OBJSDIR)/%.o)

all: tableaux-sat

tableaux-sat: $(OBJS) $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(BUILDDIR)/tableaux-sat

clean:
	@rm -rf $(OBJSDIR)

distclean: clean
	@rm -rf $(BUILDDIR)

$(OBJSDIR)/%.o: %.cpp $(OBJSDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJSDIR):
	@mkdir -p $(OBJSDIR)

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)