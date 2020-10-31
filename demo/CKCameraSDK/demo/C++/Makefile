
ARCH ?= x64   # x86 x64 armsf armhf armpi
LIB_DIR := ../../lib/$(ARCH)
CFLAGS := -std=c++11 -I../../include
LDFLAGS := -Wl,-rpath=$(LIB_DIR) -L$(LIB_DIR) -lCKCameraSDK -pthread

.PHONY: all clean run
all: camera frame_duration soft_trigger

camera : main.cpp
	g++ $(CFLAGS) $< $(LDFLAGS) -o $@

frame_duration:frame_duration.cpp
	g++ $(CFLAGS) $< $(LDFLAGS) -o $@

soft_trigger: soft_trigger.cpp
	g++ $(CFLAGS) $< $(LDFLAGS) -o $@

clean:
	rm -rf camera
	rm -rf frame_duration
	rm -rf soft_trigger

