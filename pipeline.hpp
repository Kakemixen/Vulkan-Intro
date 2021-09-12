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


private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createGraphicsPipeline(
        VkDescriptorSetLayout* pDescriptorSetLayout,
        VkSampleCountFlagBits msaaSamples,
        VkRenderPass renderPass);

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    MyDevice& device;
};
