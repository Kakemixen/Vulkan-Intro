CFLAGS := -std=c++17 -Itools/stb -Itools/tinyobjloader -g
LDFLAGS := `pkg-config --static --libs glfw3` -lvulkan
CC := g++
GLSLC := glslc
ODIR := build

sources = $(wildcard *.cpp)
objs = $(patsubst %.cpp, $(ODIR)/%.o, $(sources))

vertexSources = $(wildcard shaders/*.vert)
vertexObjs = $(patsubst %.vert, $(ODIR)/%.vert.spv, $(vertexSources))
fragSources = $(wildcard shaders/*.frag)
fragObjs = $(patsubst %.frag, $(ODIR)/%.frag.spv, $(fragSources))

program = $(ODIR)/Application

all: $(program) shaders

$(program): $(objs)
	$(CC) $(LDFLAGS) $(objs) -o $(program)
$(ODIR)/%.o : %.cpp | directories
	$(CC) $(CFLAGS) -c $< -o $@

shaders: $(vertexObjs) $(fragObjs)
$(ODIR)/%.vert.spv : %.vert | directories
	$(GLSLC) $< -o $@
$(ODIR)/%.frag.spv : %.frag | directories
	$(GLSLC) $< -o $@

directories:
	@mkdir -p $(ODIR) $(ODIR)/shaders

.PHONY: run gdb clean all shaders

run: all
	nixVulkanNvidia $(ODIR)/Application

gdb: all
	nixVulkanNvidia gdb $(ODIR)/Application

clean:
	rm -rf $(ODIR)
