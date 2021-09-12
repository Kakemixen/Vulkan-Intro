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
        VkDescriptorSetLayout* pDescriptorSetLayout,
        VkSampleCountFlagBits msaaSamples,
        VkRenderPass renderPass);
    ~MyPipeline();

    MyPipeline(MyPipeline& other) = delete;
    MyPipeline operator=(MyPipeline& other) = delete;

    void bind(VkCommandBuffer commandBuffer,
        VkDescriptorSet* pDescriptorSet);
    void pushConstants(VkCommandBuffer commandBuffer, 
            uint32_t size,
            const void* data);

    VkRenderPass renderPass;

private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createGraphicsPipeline(
        VkDescriptorSetLayout* pDescriptorSetLayout,
        VkSampleCountFlagBits msaaSamples);

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    MyDevice& device;
};
