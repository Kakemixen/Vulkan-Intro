#include <vulkan/vulkan.h>
//
//std
#include <optional>
#include <vector>

class MyWindow;

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> transferFamily;

    bool isComplete() {
        return graphicsFamily.has_value() &&
            presentFamily.has_value() && 
            transferFamily.has_value();
    }
};

class MyDevice 
{
public:
    MyDevice();
    ~MyDevice();

    void setupDevice(MyWindow* window);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, 
            VkBuffer dstBuffer, 
            VkDeviceSize size);
    void createCommandPool();
    void createTransferCommandPool();
    VkCommandBuffer beginSingleCommands(VkCommandPool& pool);
    void endSingleCommands(VkCommandBuffer commandBuffer, 
            VkCommandPool& pool, 
            VkQueue queue);
    VkImageView createImageView(
        VkImage image, 
        VkFormat format, 
        VkImageAspectFlags aspectFlags,
        uint32_t mipLevels);
    VkFormat findDepthFormat();
    void createImage(uint32_t width, uint32_t height, 
        uint32_t mipLevels,
        VkSampleCountFlagBits numSamples,
        VkFormat format, 
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory & imageMemory);
    void transitionImageLayout(VkImage image, 
        VkFormat format, 
        VkImageLayout oldLayout, 
        VkImageLayout newLayout,
        uint32_t mipLevels);
    void copyBufferToImage(VkBuffer buffer, 
            VkImage image, 
            uint32_t width, uint32_t height);

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkSurfaceKHR surface;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
    VkCommandPool commandPool;
    VkCommandPool transferCommandPool;

private:
    void createInstance();
    void setupDebugmessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    int rateDeviceSuitability(VkPhysicalDevice device);
    std::vector<const char*> getRequiredExtensions();
    void createSurface();
    VkFormat findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);

    MyWindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};


bool checkDeviceExtensionSupport(VkPhysicalDevice device);

bool checkValidationLayerSupport();
void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo);
