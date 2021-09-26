#include "simple_render_system.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "model.hpp"
#include "texture.hpp"

#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

SimpleRenderSystem::SimpleRenderSystem(MyDevice& device, 
        VkRenderPass renderPass,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
    : device(device)
{
    createNewPipeline(renderPass, descriptorSetLayouts);
}

SimpleRenderSystem::~SimpleRenderSystem()
{ }

void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, 
        std::vector<MyGameObject>& gameObjects,
        const std::vector<VkDescriptorSet>& globalDescriptorSets) const
{
    pipeline->bind(commandBuffer);
    pipeline->bindDescriptorSets(commandBuffer, globalDescriptorSets);
    for (auto& gameObject : gameObjects) {
        glm::mat4 objMat = gameObject.transform.getMatrix();
        pipeline->pushConstants(commandBuffer, sizeof(objMat), &objMat);
        pipeline->bindDescriptorSets(commandBuffer, {gameObject.texture->getDescriptor()}, 1);
        gameObject.model->bind(commandBuffer);
        gameObject.model->draw(commandBuffer);
    }
}

void SimpleRenderSystem::createNewPipeline(VkRenderPass newRenderPass,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
{
    pipeline = std::make_unique<MyPipeline>(device,
            descriptorSetLayouts, device.getMaxUsableSampleCount(), 
            newRenderPass);
}

