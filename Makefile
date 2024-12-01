# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -O2 -Wall -Wextra -DMG_ENABLE_PACKED_FS=1 -w -DLOG_USE_COLOR

# Include Paths
INCLUDES = -I./  # Ensure this points to mongoose.h and sqlite3.h if needed

# Source files
SRCS = main.c ./libs/sqlite3.c ./libs/mongoose.c ./libs/log.c fs.c ./modules/db.c ./modules/utils.c ./modules/jwt.c

LDFLAGS :=
TARGET :=

ifeq ($(UNAME_S),Linux)
  TCFLAGS += -D LINUX
  TARGET = C_Base
  $(info Linux build...)
else ifeq ($(UNAME_S),Darwin)
  CFLAGS += -D OSX
  TARGET = C_Base
  $(info OSX build...)
else
# Linker Flags for Windows (add ws2_32 for networking support)
  CFLAGS += -D Windows
  LDFLAGS = -lws2_32 
  TARGET = C_Base.exe
  $(info Windows build...)
endif

# Build the application
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

