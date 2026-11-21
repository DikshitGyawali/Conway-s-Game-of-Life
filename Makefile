CFLAGS = -Wall -Wextra -g `pkg-config --cflags --libs sdl2`
BUILD_DIR = Build

all: $(BUILD_DIR)/life

C_SRCS := $(shell find . -name "*.c")

check:
	gcc $(CFLAGS) -c $(FILE) -o /dev/null

$(BUILD_DIR)/life: $(C_SRCS)
	gcc $(CFLAGS) main.c -o $(BUILD_DIR)/life

run:
	SDL_VIDEODRIVER=x11 ./${BUILD_DIR}/life
