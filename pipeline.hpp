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
    MyPipeline(MyDevice& device);
    ~MyPipeline();

    void createGraphicsPipeline(
        VkDescriptorSetLayout* pDescriptorSetLayout,
        VkSampleCountFlagBits msaaSamples,
        const MySwapChain& swapchain);
    void destroyGraphicsPipeline();
    void bind(VkCommandBuffer commandBuffer,
        VkDescriptorSet* pDescriptorSet);


private:
    VkShaderModule createShaderModule(const std::vector<char>& code);

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    MyDevice& device;
};
