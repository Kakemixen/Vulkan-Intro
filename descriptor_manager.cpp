#include "descriptor_manager.hpp"
#include "device.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cassert>

MyDescriptorManager::MyDescriptorManager(MyDevice& device)
    : device(device)
{ }

MyDescriptorManager::~MyDescriptorManager()
{
    vkDestroyDescriptorSetLayout(device.device, descriptorSetLayout, nullptr);
}

void MyDescriptorManager::createDescriptorSets(uint32_t numFrameBuffers)
{
    std::vector<VkDescriptorSetLayout> layouts(numFrameBuffers, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = device.descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(numFrameBuffers);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(numFrameBuffers);
    if (vkAllocateDescriptorSets(device.device, &allocInfo, descriptorSets.data())
            != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

}

void MyDescriptorManager::updateDescriptorSets(size_t i,
        VkDescriptorBufferInfo bufferInfo,
        VkDescriptorImageInfo imageInfo)
{
    assert(i < descriptorSets.size());

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;
    descriptorWrites[0].pImageInfo = nullptr;
    descriptorWrites[0].pTexelBufferView = nullptr;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = nullptr;
    descriptorWrites[1].pImageInfo = &imageInfo;
    descriptorWrites[1].pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device.device, 
            static_cast<uint32_t>(descriptorWrites.size()), 
            descriptorWrites.data(),
            0, nullptr);
}

void MyDescriptorManager::createDescriptorSetLayout(
        std::vector<VkDescriptorSetLayoutBinding> bindings)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device.device, &layoutInfo, nullptr, &descriptorSetLayout)
            != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

VkDescriptorSetLayout* MyDescriptorManager::getDescriptorSetLayout()
{
    return &descriptorSetLayout;
}


