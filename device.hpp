#pragma once

//libs
#include <vulkan/vulkan.h>

//std
#include <optional>
#include <vector>
#include <map>

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

enum class DeviceQueue
{
    Graphics,
    Present,
    Transfer
};

enum class CommandPool
{
    Command,
    Transfer
};

class MyDevice 
{
public:
    MyDevice(MyWindow& window);
    ~MyDevice();

    MyDevice(MyDevice& other) = delete;
    MyDevice operator=(MyDevice& other) = delete;

    void setupDevice();
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
    VkCommandBuffer beginSingleCommands(CommandPool poolEnum);
    void endSingleCommands(VkCommandBuffer commandBuffer, 
            CommandPool poolEnum,
            DeviceQueue queue);
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
    VkResult queueSubmit(
            DeviceQueue queue,
            uint32_t submitCount,
            const VkSubmitInfo* pSubmits,
            VkFence fence);
    VkResult present(const VkPresentInfoKHR* pPresentInfo);
    void allocateCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers);
    void freeCommandBuffers(std::vector<VkCommandBuffer>* commandBuffers);

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkSurfaceKHR surface;

    void createDescriptorPool(std::vector<VkDescriptorPoolSize>& poolSizes,
            uint32_t maxSets);
    VkDescriptorPool descriptorPool;

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

    MyWindow& window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    std::map<CommandPool, VkCommandPool> poolMap;
    std::map<DeviceQueue, VkQueue> queueMap;

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
