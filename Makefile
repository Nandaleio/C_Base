# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -O2 -Wall -Wextra -DMG_ENABLE_PACKED_FS=1 -w -DLOG_USE_COLOR -g

# Include Paths
INCLUDES = -Iinclude  # Ensure this points to mongoose.h and sqlite3.h if needed

# Source files
SRCS = ./src/main.c ./libs/sqlite3.c ./libs/mongoose.c ./libs/log.c ./libs/parson.c ./src/fs.c ./src/db.c ./src/utils.c ./src/jwt.c

LDFLAGS :=
TARGET :=

PLATFORM := $(shell uname)
$(info PLATFORM is $(PLATFORM))

ifeq ($(PLATFORM),Linux)
  TCFLAGS += -D LINUX
  TARGET = C_Base
  $(info Linux build...)
else ifeq ($(PLATFORM),Darwin)
  CFLAGS += -D OSX
  TARGET = C_Base
  $(info OSX build...)
else
  CFLAGS += -D Windows
  LDFLAGS = -lws2_32  # Linker Flags for Windows (add ws2_32 for networking support)
  TARGET = C_Base.exe
  $(info Windows build...)
endif

# Run custom command before compilation
pre-build:
	npm run build --prefix ui
	./pack.exe ui/dist/index.html ui/dist/index.css.gz ui/dist/index.js.gz ui/dist/favicon.ico

all: pre-build $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

