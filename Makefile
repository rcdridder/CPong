.PHONY: all clean

# Define required raylib variables
PROJECT_NAME       = pong
RAYLIB_VERSION     = 4.2.0
RAYLIB_PATH        = C:/raylib/raylib/
EXT                = .exe
COMPILER_PATH      = C:/msys64/ucrt64/bin/gcc.exe
BUILD_MODE         = DEBUG

export PATH := $(COMPILER_PATH):$(PATH)

CC = gcc
MAKE = mingw32-make
CFLAGS += -Wall -pedantic -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -s -O1
endif

CFLAGS += $(RAYLIB_PATH)/src/raylib.rc.data -Wl,--subsystem,windows
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external
LDFLAGS = -L. -L$(RAYLIB_PATH)/src 
LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all:
	$(MAKE) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) -o $(PROJECT_NAME)$(EXT) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

clean:
	del *.o *.exe /s
	@echo Cleaning done

