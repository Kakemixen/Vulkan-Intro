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
        const MySwapChain& swapchain);
    ~MyPipeline();

    void bind(VkCommandBuffer commandBuffer,
        VkDescriptorSet* pDescriptorSet);


private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createGraphicsPipeline(
        VkDescriptorSetLayout* pDescriptorSetLayout,
        VkSampleCountFlagBits msaaSamples,
        const MySwapChain& swapchain);

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    MyDevice& device;
};
