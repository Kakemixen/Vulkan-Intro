#include "descriptor_manager.hpp"
#include "device.hpp"
#include "texture.hpp"

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
    vkDestroyDescriptorSetLayout(device.device, globalDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(device.device, textureDescriptorSetLayout, nullptr);
}

void MyDescriptorManager::createDescriptorSetsHelper(std::vector<VkDescriptorSet>& descriptorSets, 
        uint32_t numSets, VkDescriptorSetLayout layout)
{
    std::vector<VkDescriptorSetLayout> layouts(numSets, layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = device.descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(numSets);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(numSets);
    if (vkAllocateDescriptorSets(device.device, &allocInfo, descriptorSets.data())
            != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void MyDescriptorManager::createDescriptorSets(uint32_t numFrameBuffers, 
        std::vector<std::shared_ptr<MyTexture>>& textures)
{
    createDescriptorSetsHelper(globalDescriptorSets, numFrameBuffers, globalDescriptorSetLayout);
    createDescriptorSetsHelper(textureDescriptorSets, 
            static_cast<uint32_t>(textures.size()), textureDescriptorSetLayout);
    for (size_t i = 0; i < textures.size(); i++) {
        textures[i]->setDescriptor(textureDescriptorSets[i]);
    }
}

void MyDescriptorManager::updateGlobalDescriptorSets(size_t i,
        VkDescriptorBufferInfo& bufferInfo)
{
    assert(i < globalDescriptorSets.size());

    VkWriteDescriptorSet descriptorWrite{};

    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = globalDescriptorSets[i];
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pBufferInfo = &bufferInfo;
    descriptorWrite.pImageInfo = nullptr;
    descriptorWrite.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(device.device, 
            1,
            &descriptorWrite,
            0, nullptr);
}

void MyDescriptorManager::updateTextureDescriptorSets(
        std::vector<std::shared_ptr<MyTexture>>& textures)
{
    assert(textures.size() == textureDescriptorSets.size());

    std::vector<VkWriteDescriptorSet> descriptorWrites(textures.size(),
            VkWriteDescriptorSet{});

    std::vector<VkDescriptorImageInfo> imageInfos(textures.size());
    for (size_t i = 0; i < textures.size(); i++) {
        imageInfos[i] = textures[i]->getImageInfo();
    }
    

    for (size_t i = 0; i < textures.size(); i++) {
        descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet = textureDescriptorSets[i];
        descriptorWrites[i].dstBinding = 0;
        descriptorWrites[i].dstArrayElement = 0;
        descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pBufferInfo = nullptr;
        descriptorWrites[i].pImageInfo = &imageInfos[i];
        descriptorWrites[i].pTexelBufferView = nullptr;
    }

    vkUpdateDescriptorSets(device.device, 
            static_cast<uint32_t>(descriptorWrites.size()), 
            descriptorWrites.data(),
            0, nullptr);
}

void MyDescriptorManager::createDescriptorSetLayoutHelper(
    std::vector<VkDescriptorSetLayoutBinding> bindings,
    VkDescriptorSetLayout* layout)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device.device, &layoutInfo, nullptr, layout)
            != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void MyDescriptorManager::createGlobalDescriptorSetLayout(
        std::vector<VkDescriptorSetLayoutBinding> bindings)
{
    createDescriptorSetLayoutHelper(bindings, &globalDescriptorSetLayout);
}

void MyDescriptorManager::createTextureDescriptorSetLayout(
        std::vector<VkDescriptorSetLayoutBinding> bindings)
{
    createDescriptorSetLayoutHelper(bindings, &textureDescriptorSetLayout);
}

std::vector<VkDescriptorSetLayout> MyDescriptorManager::getDescriptorSetLayout() const
{
    return {globalDescriptorSetLayout, textureDescriptorSetLayout};
}


std::vector<VkDescriptorSet> MyDescriptorManager::getGlobalDescriptorSets(size_t i) const
{
    return {globalDescriptorSets[i]};
}
