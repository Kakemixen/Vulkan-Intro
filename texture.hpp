#pragma once

#include <vulkan/vulkan.h>

class MyDevice;

class MyTexture
{
public:
    MyTexture(MyDevice& device, const char* texturePath);
    ~MyTexture();

    MyTexture(MyTexture& other) = delete;
    MyTexture operator=(MyTexture& other) = delete;

    VkDescriptorImageInfo getImageInfo();

private:
    void createTextureImage(const char* texturePath);
    void createTextureImageView();
    void createTextureSampler();
    void generateMipmaps(VkImage image,
            VkFormat format,
            int32_t texWidth,
            int32_t texHeight,
            uint32_t mipLevels);

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;
    MyDevice& device;
};
