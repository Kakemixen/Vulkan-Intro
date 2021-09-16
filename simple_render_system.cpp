#include "simple_render_system.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "model.hpp"

#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

SimpleRenderSystem::SimpleRenderSystem(MyDevice& device, 
        VkRenderPass renderPass,
        VkDescriptorSetLayout* pDescriptorSetLayout)
    : device(device)
{
    createNewPipeline(renderPass, pDescriptorSetLayout);
}

SimpleRenderSystem::~SimpleRenderSystem()
{ }

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, 
        std::vector<MyGameObject> gameObjects,
        VkDescriptorSet* pDescriptorSet)
{
    pipeline->bind(commandBuffer, pDescriptorSet);
    for (auto& gameObject : gameObjects) {
        pipeline->pushConstants(commandBuffer, sizeof(gameObject.transform.matrix), &gameObject.transform.matrix);
        gameObject.model->bind(commandBuffer);
        gameObject.model->draw(commandBuffer);
    }
}

void SimpleRenderSystem::createNewPipeline(VkRenderPass newRenderPass,
        VkDescriptorSetLayout* pDescriptorSetLayout)
{
    pipeline = std::make_unique<MyPipeline>(device,
            pDescriptorSetLayout, device.getMaxUsableSampleCount(), 
            newRenderPass);
}

