CXX= g++
CXXFLAGS= -std=c++11

INCLUDE=
LIB= -lSDL2 -lSDL2main -lSDL2_ttf

OBJDIR= obj
SRCDIR= src

OBJS= $(addprefix $(OBJDIR)/, filebrowser.o)
EXEC= filebrowser


# BUILD EVERYTHING
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)


# REMOVE OLD FILES
clean:
	rm -f $(OBJS) $(EXEC)
