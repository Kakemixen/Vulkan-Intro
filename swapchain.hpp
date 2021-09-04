
//libs
#include <vulkan/vulkan.h>

//std
#include <vector>

class MyDevice;

class MySwapChain
{
public:
    MySwapChain(MyDevice& device);
    ~MySwapChain();

    void createSwapChain(const VkExtent2D& windowExtent);
    void cleanup();
    void createImageViews();
    void createFramebuffers();
    void createDepthResources();
    void createColorResources();
    void createRenderPass();
    void beginRenderPass(VkCommandBuffer commandBuffer, size_t i);
    void endRenderPass(VkCommandBuffer commandBuffer);
    size_t size();

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkRenderPass renderPass = nullptr;

private:

    MyDevice& device;
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
