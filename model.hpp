#pragma once

//libs
#include <vulkan/vulkan.h>

//std
#include <vector>
#include <cstdint>

class Vertex;
class MyDevice;

class MyModel {
public:
    MyModel(MyDevice& device, const char* modelPath);
    ~MyModel();

    void loadModel(const char* modelPath);
    void createVertexBuffer();
    void createIndexBuffer();
    void bind(VkCommandBuffer& commandBuffer);
    void draw(VkCommandBuffer& commandBuffer);

private:
    MyDevice& device;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;
};
