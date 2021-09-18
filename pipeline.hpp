#pragma once

//libs
#include <vulkan/vulkan.h>

//std
#include <vector>

class MyDevice;
class MySwapChain;

class MyPipeline
{
public:
    MyPipeline(MyDevice& device,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
        VkSampleCountFlagBits msaaSamples,
        VkRenderPass renderPass);
    ~MyPipeline();

    MyPipeline(MyPipeline& other) = delete;
    MyPipeline operator=(MyPipeline& other) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void bindDescriptorSets(VkCommandBuffer commandBuffer,
            std::vector<VkDescriptorSet> descriptorSets, 
            uint32_t firstSet=0);
    void pushConstants(VkCommandBuffer commandBuffer, 
            uint32_t size,
            const void* data);

    VkRenderPass renderPass;

private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createGraphicsPipeline(
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts,
        VkSampleCountFlagBits msaaSamples);

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    MyDevice& device;
};
