#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

class MyPipeline;
class MyGameObject;
class MyDevice;

class SimpleRenderSystem
{
public:
    SimpleRenderSystem(MyDevice& device, 
            VkRenderPass renderPass,
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem& other) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem& other) = delete;

    void createNewPipeline(VkRenderPass newRenderPass,
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
    void renderGameObjects(VkCommandBuffer commandBuffer, 
            std::vector<MyGameObject>& gameObjects,
            const std::vector<VkDescriptorSet>& globalDescriptorSet) const;

private:

    MyDevice& device;
    std::unique_ptr<MyPipeline> pipeline;
};
