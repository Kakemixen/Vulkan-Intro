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
    static const int MAX_FRAMES_IN_FLIGHT = 2;

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

    VkFramebuffer getFramebuffer(size_t i);
    VkRenderPass getRenderPass();
    VkResult acquireNextImage(uint32_t* imageIndex);
    VkResult submitCommandBuffers(VkCommandBuffer* commandBuffer, 
            size_t imageIndex);
    VkResult present(uint32_t imageIndex);
    size_t size();
    bool renderPassCompatible(const std::shared_ptr<MySwapChain> oldSwapchain);

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkExtent2D swapChainExtent;

private:
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
    VkRenderPass renderPass = nullptr;
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
