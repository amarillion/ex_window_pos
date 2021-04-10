#by Martijn van Iersel (amarillion@yahoo.com)

# BUILD=DEBUG
BUILD=RELEASE

LIBS = 
CFLAGS = -Iinclude -W -Wall
GPP = g++
BINSUF =
LFLAGS = 

ifdef WINDOWS
	CFLAGS += -D__GTHREAD_HIDE_WIN32API
	LFLAGS += -Wl,--subsystem,windows
	ifeq ($(BUILD),RELEASE)
		LIBS += -lallegro_monolith
	endif
	ifeq ($(BUILD),DEBUG)
		LIBS += -lallegro_monolith-debug
	endif
	BINSUF = .exe
else
	ALLEGRO_MODULES=allegro allegro_font allegro_color
	ifeq ($(BUILD),RELEASE)
		CFLAGS += -O3 -s
		ALLEGRO_LIBS = $(addsuffix -5, $(ALLEGRO_MODULES))
	endif
	ifeq ($(BUILD),DEBUG)
		CFLAGS += -g -DDEBUG
		ALLEGRO_LIBS = $(addsuffix -debug-5, $(ALLEGRO_MODULES))
	endif
	LIBS += `pkg-config --libs $(ALLEGRO_LIBS)`
endif

OBJDIR=obj

vpath %.cpp src

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRC)))
DEP = $(patsubst %.cpp, $(OBJDIR)/%.d, $(notdir $(SRC)))
BIN = ex_window_pos$(BINSUF)

$(shell mkdir -p $(OBJDIR) >/dev/null)

$(BIN) : $(OBJ) $(LIB)
	$(GPP) $^ -o $@ $(LIBS) $(LFLAGS)

$(OBJ) : $(OBJDIR)/%.o : %.cpp
	$(GPP) $(CFLAGS) -MMD -c $< -o $@