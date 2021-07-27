VULKAN_SDK_PATH = /home/user/.local/vulkan/1.2.182.0/x86_64

CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

Application: main.cpp
	g++ $(CFLAGS) -o Application main.cpp $(LDFLAGS)

.PHONY: test clean

run: Application
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib
	VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/vulkan/explicit_layer.d
	./Application

clean:
	rm -f Application

