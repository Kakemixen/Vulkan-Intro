#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>

class MyTexture;
class MyDevice;

class MyDescriptorManager
{
public:
    MyDescriptorManager(MyDevice& device);
    ~MyDescriptorManager();

    void createDescriptorSets(uint32_t numFrameBuffers);
    void updateDescriptorSets(size_t i,
            VkDescriptorBufferInfo bufferInfo,
            VkDescriptorImageInfo imageInfo);
    VkDescriptorSetLayout* getDescriptorSetLayout();
    void createDescriptorSetLayout(
            std::vector<VkDescriptorSetLayoutBinding> bindings);

    std::vector<VkDescriptorSet> descriptorSets;
private:
    VkDescriptorSetLayout descriptorSetLayout;

    MyDevice& device;
};
