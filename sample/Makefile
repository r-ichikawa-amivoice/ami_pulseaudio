PROJECT	  = ami_pulseaudio
COMPILER  = g++
CFLAGS    = -fPIC
LDFLAGS   = -L../lib/ -lami_pulseaudio
LIBS      = 
INCLUDE   = -I../src/
TARGETDIR = ../bin/
ifeq "$(strip $(TARGETDIR))" ""
  TARGETDIR  = .
endif
TARGET    = $(PROJECT)
SRCDIR    = ../sample/
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ../obj/
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGETDIR)$(TARGET): $(OBJECTS) $(LIBS)
	-mkdir -p $(TARGETDIR)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -rf $(OBJDIR) $(TARGETDIR)

-include $(DEPENDS)


