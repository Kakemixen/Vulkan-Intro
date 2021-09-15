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
            VkDescriptorSetLayout* pDescriptorSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem& other) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem& other) = delete;

    void createNewPipeline(VkRenderPass newRenderPass,
            VkDescriptorSetLayout* pDescriptorSetLayout);
    void renderGameObjects(VkCommandBuffer commandBuffer, 
            std::vector<MyGameObject> gameObjects,
            VkDescriptorSet* pDescriptorSet);

private:

    MyDevice& device;
    std::unique_ptr<MyPipeline> pipeline;
};
