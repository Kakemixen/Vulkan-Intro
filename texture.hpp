#pragma once

#include <vulkan/vulkan.h>

class MyDevice;

class MyTexture
{
public:
    MyTexture(MyDevice& device);
    ~MyTexture();

    void createTextureImage(const char* texturePath);
    void createTextureImageView();
    void createTextureSampler();
    VkDescriptorImageInfo getImageInfo();

private:
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
