CFLAGS = -std=c++17 -Itools/stb
LDFLAGS = `pkg-config --static --libs glfw3` -lvulkan

Application: main.cpp shaders/vert.spv shaders/frag.spv
	g++ $(CFLAGS) -o Application main.cpp $(LDFLAGS)

shaders/vert.spv: shaders/shader.vert
	glslc shaders/shader.vert -o shaders/vert.spv

shaders/frag.spv: shaders/shader.frag
	glslc shaders/shader.frag -o shaders/frag.spv


.PHONY: run clean

run: Application
	nixVulkanNvidia ./Application

clean:
	rm -f Application
	rm -f shaders/*.spv

