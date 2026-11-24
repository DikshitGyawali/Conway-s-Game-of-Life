CFLAGS = -Wall -Wextra -g -fsanitize=address,undefined `pkg-config --cflags --libs sdl2 SDL2_ttf`
BUILD_DIR = Build

all: $(BUILD_DIR)/life

C_SRCS := $(shell find . -name "*.c")
C_HDRS := $(shell find . -name "*.h")

check:
	gcc $(CFLAGS) -c $(FILE) -o /dev/null

$(BUILD_DIR)/life: $(C_SRCS) $(C_HDRS)
	gcc $(CFLAGS) $(C_SRCS) -o $(BUILD_DIR)/life

run:
	SDL_VIDEODRIVER=x11 ./${BUILD_DIR}/life
