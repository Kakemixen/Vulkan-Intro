#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

class MySwapChain;
class MyWindow;
class MyDevice;
class MyDevice;

class MyRenderer
{
public:
    MyRenderer( MyWindow& window,
            MyDevice& device,
            VkSampleCountFlagBits msaaSamples);
    ~MyRenderer();

    MyRenderer(MyRenderer& other) = delete;
    MyRenderer& operator=(const MyRenderer& other) = delete;

    VkCommandBuffer beginFrame();
    void endFrame(VkCommandBuffer commandBuffer);
    void beginRenderPass(VkCommandBuffer commandBuffer);
    void endRenderPass(VkCommandBuffer commandBuffer);

    uint32_t getIndex();
    VkRenderPass getSwapChainRenderPass();
    size_t getSize();
    float getAspectRatio();

private:
    void createCommandBuffers();
    void reCreateSwapChain();

    std::unique_ptr<MySwapChain> swapchain;
    std::vector<VkCommandBuffer> commandBuffers;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    MyWindow& window;
    MyDevice& device;
    uint32_t currentImageIdx = 0;
    uint32_t currentCommandBufferIdx = 0;
    bool startedFrame = false;
};
