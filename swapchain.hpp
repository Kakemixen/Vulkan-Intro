#pragma once

//libs
#include <vulkan/vulkan.h>

//std
#include <vector>
#include <memory>

class MyDevice;

class MySwapChain
{
public:
    MySwapChain(MyDevice& device, 
            const VkExtent2D& windowExtent,
            VkSampleCountFlagBits msaaSamples);
    MySwapChain(MyDevice& device, 
            const VkExtent2D& windowExtent,
            VkSampleCountFlagBits msaaSamples,
            std::shared_ptr<MySwapChain> prevSwapChain);
    ~MySwapChain();

    MySwapChain(MySwapChain& other) = delete;
    MySwapChain operator=(MySwapChain& other) = delete;

    void beginRenderPass(VkCommandBuffer commandBuffer, size_t i);
    void endRenderPass(VkCommandBuffer commandBuffer);
    VkResult acquireNextImage(uint32_t* imageIndex);
    VkResult submitCommandBuffers(VkCommandBuffer* commandBuffer, 
            size_t imageIndex);
    VkResult present(uint32_t imageIndex);
    size_t size();
    bool renderPassCompatible(const VkRenderPass& renderPass);

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkRenderPass renderPass = nullptr;

private:
    const int MAX_FRAMES_IN_FLIGHT = 2;

    void init(const VkExtent2D& windowExtent,
        std::shared_ptr<MySwapChain> prevSwapChain);
    void createSwapChain(const VkExtent2D& windowExtent,
        std::shared_ptr<MySwapChain> prevSwapChain);
    void createImageViews();
    void createFramebuffers();
    void createDepthResources();
    void createColorResources();
    void createRenderPass();
    void createSyncObjects();

    MyDevice& device;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
};
