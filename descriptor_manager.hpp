#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <cstdint>
#include <memory>

class MyTexture;
class MyDevice;


class MyDescriptorManager
{
public:
    MyDescriptorManager(MyDevice& device);
    ~MyDescriptorManager();

    void createDescriptorSets(uint32_t numFrameBuffers,
        std::vector<std::shared_ptr<MyTexture>>& textures);
    void updateGlobalDescriptorSets(size_t i,
            VkDescriptorBufferInfo& bufferInfo);
    void updateTextureDescriptorSets(
        std::vector<std::shared_ptr<MyTexture>>& textures);
    std::vector<VkDescriptorSetLayout> getDescriptorSetLayout();
    void createGlobalDescriptorSetLayout(
            std::vector<VkDescriptorSetLayoutBinding> bindings);
    void createTextureDescriptorSetLayout(
            std::vector<VkDescriptorSetLayoutBinding> bindings);

    std::vector<VkDescriptorSet> getGlobalDescriptorSets(size_t i);

    // num frames - one each frame
    std::vector<VkDescriptorSet> globalDescriptorSets;

    // num textures - not changing between frames - bind corresponding to object
    std::vector<VkDescriptorSet> textureDescriptorSets; 

private:
    VkDescriptorSetLayout globalDescriptorSetLayout;
    VkDescriptorSetLayout textureDescriptorSetLayout;

    MyDevice& device;

    void createDescriptorSetsHelper(std::vector<VkDescriptorSet>& descriptorSets, 
            uint32_t numSets, VkDescriptorSetLayout layout);
    void createDescriptorSetLayoutHelper(
        std::vector<VkDescriptorSetLayoutBinding> bindings,
        VkDescriptorSetLayout* layout);
};

