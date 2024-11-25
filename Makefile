# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -O2 -Wall -Wextra -DMG_ENABLE_PACKED_FS=1 -w

# Include Paths
INCLUDES = -I./  # Ensure this points to mongoose.h and sqlite3.h if needed

# Source files
SRCS = main.c ./libs/sqlite3.c ./libs/mongoose.c fs.c ./modules/db.c ./modules/utils.c

# Linker Flags for Windows (add ws2_32 for networking support)
LDFLAGS = -lws2_32 

# Output binary
TARGET = C_Base.exe

# Build the application
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGET)

