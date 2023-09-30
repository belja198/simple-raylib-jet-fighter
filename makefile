# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -g

# Directories
SRCDIR = src
INCDIR = inc
LIBDIR = lib
BINDIR = bin
RESDIR = res

# Files
SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SRC))

# Libraries
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Targets
all: my_program

my_program: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean

cleanbin:
	del $(BINDIR)\*.o

cleanexe:
	del my_program