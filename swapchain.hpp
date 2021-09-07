#pragma once

//libs
#include <vulkan/vulkan.h>

//std
#include <vector>

class MyDevice;

class MySwapChain
{
public:
    MySwapChain(MyDevice& device, 
            const VkExtent2D& windowExtent,
            VkSampleCountFlagBits msaaSamples);
    ~MySwapChain();

    void beginRenderPass(VkCommandBuffer commandBuffer, size_t i);
    void endRenderPass(VkCommandBuffer commandBuffer);
    size_t size();

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass = nullptr;

private:
    void createSwapChain(const VkExtent2D& windowExtent);
    void createImageViews();
    void createFramebuffers();
    void createDepthResources();
    void createColorResources();
    void createRenderPass();

    MyDevice& device;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
};
