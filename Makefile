CC = gcc
LD = gcc

OBJ = Merry-go-around.o LoadShader.o Matrix.o StringExtra.o OBJParser.o List.o LoadTexture.o
TARGET = Merry-go-around


CFLAGS = -g -Wall
LDLIBS= -lstdc++ -lm -lglut -lGLEW -lGL
INCLUDES = -Isource

SRC_DIR = src
BUILD_DIR = build
VPATH = src

# Rules
all: $(TARGET)

$(TARGET).o: $(TARGET).cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	rm -f $(BUILD_DIR)/*.o *.o $(TARGET)

.PHONY: clean

# Dependencies
$(TARGET): $(BUILD_DIR)/LoadShader.o $(BUILD_DIR)/Matrix.o $(BUILD_DIR)/StringExtra.o $(BUILD_DIR)/OBJParser.o  $(BUILD_DIR)/List.o $(BUILD_DIR)/LoadTexture.o | $(BUILD_DIR)


