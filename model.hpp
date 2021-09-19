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

    MyModel(MyModel& other) = delete;
    MyModel operator=(MyModel& other) = delete;

    void loadModel(const char* modelPath);
    void createVertexBuffer();
    void createIndexBuffer();
    void bind(VkCommandBuffer& commandBuffer) const;
    void draw(VkCommandBuffer& commandBuffer) const;

private:
    MyDevice& device;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceMemory indexBufferMemory;
};
