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
