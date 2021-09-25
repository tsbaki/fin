CC := cc

PKG_CONFIG := `pkg-config --cflags gtk4` `pkg-config --libs gtk4`
CCFLAGS := -l sqlite3 
DBFLAGS := -g -Wextra

SRC_PATH := src/**.c
BIN_PATH := bin

TARGET := $(BIN_PATH)/fin

all: $(SRC_PATH) $(BIN_PATH)
	$(CC) $(CCFLAGS) $(DBFLAGS) $(SRC_PATH) -o $(TARGET)

.PHONY: makedir
makedir: 
	@mkdir $(BIN_PATH)

