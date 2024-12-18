# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -DMG_ENABLE_PACKED_FS=1 -w -DLOG_USE_COLOR

# Include Paths
INCLUDES = -Iinclude  # Ensure this points to mongoose.h and sqlite3.h if needed

# Source files
SRCS = ./src/main.c ./src/fs.c 
SRCS += ./src/libs/sqlite3.c ./src/libs/mongoose.c ./src/libs/log.c ./src/libs/parson.c
SRCS += ./src/db/db.c ./src/utils/utils.c ./src/utils/jwt.c ./src/routes/standard-route.c ./src/routes/admin-route.c
SRCS += ./src/controllers/admin-controller.c ./src/controllers/standard-controller.c ./src/controllers/auth-controller.c

LDFLAGS :=
TARGET = C_Base

PLATFORM := $(shell uname)

ifeq ($(PLATFORM),Linux)
  TCFLAGS += -D LINUX
  $(info Linux build...)
else ifeq ($(PLATFORM),Darwin)
  CFLAGS += -D OSX
  $(info OSX build...)
else
  CFLAGS += -D Windows
  SRCS += resource.res
  LDFLAGS = -lws2_32  # Linker Flags for Windows (add ws2_32 for networking support)
  $(info Windows build...)
endif

all:  $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LDFLAGS)


debug:
	$(CC) $(CFLAGS) -g -O0 $(INCLUDES) -o $(TARGET) $(SRCS) $(LDFLAGS)

front:
	npm run build --prefix ui
	./pack ui/dist/index.html ui/dist/index.css.gz ui/dist/index.js.gz ui/dist/favicon.ico
  
# Clean up build files
clean:
	rm -f $(TARGET)

