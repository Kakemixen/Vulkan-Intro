#include "device.hpp"
#include "window.hpp"
#include "vertex.hpp"
#include "model.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "texture.hpp"
#include "game_object.hpp"
#include "utils.hpp"

//libs
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

//std
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <map>
#include <optional>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>
#include <memory>

//cstd
#include <unistd.h>
#include <cstring>
#include <cstdint>
#include <cassert>


const std::string MODEL_PATH = "models/companion_cube.obj";
const std::string TEXTURE_PATH = "textures/companion_cube.png";


struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct PushConstantData {
    alignas(16) glm::mat4 transform;
};


class HelloTriangleApplication
{
public:
    void run() {
        createGameObjects();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:

    void initVulkan() 
    {
        msaaSamples = device.getMaxUsableSampleCount();
        swapchain = std::make_unique<MySwapChain>(device,
                window.getExtent(), msaaSamples);
        createDescriptorSetLayout();
        pipeline = std::make_unique<MyPipeline>(device,
                &descriptorSetLayout, msaaSamples, 
                swapchain->renderPass);
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
    }

    void mainLoop() 
    {
        while (!window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }
        vkDeviceWaitIdle(device.device);
    }

    void cleanup() 
    {
        cleanupSwapChain();

        vkDestroyDescriptorSetLayout(device.device, descriptorSetLayout, nullptr);
        glfwTerminate();
    }

    void cleanupSwapChain()
    {
        device.freeCommandBuffers(&commandBuffers);

        for (size_t i = 0; i < swapchain->size(); i++) {
            vkDestroyBuffer(device.device, uniformBuffers[i], nullptr);
            vkFreeMemory(device.device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device.device, descriptorPool, nullptr);
    }


    void drawFrame()
    {
        uint32_t imageIndex;
        VkResult result = swapchain->acquireNextImage(&imageIndex);

        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float timeDelta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        startTime = std::chrono::high_resolution_clock::now();

        recordCommandBuffer(imageIndex, timeDelta);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            std::cout << "recreating swap chain out of date\n";
            reCreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        result = swapchain->submitCommandBuffers(&commandBuffers[imageIndex], imageIndex);
        if (result != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        result = swapchain->present(imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR 
                || result == VK_SUBOPTIMAL_KHR
                || window.wasResized()) 
        {
            std::cout << "recreating swap chain out of date or suboptimal\n";
            reCreateSwapChain();
            window.resetResizedFlag();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
    }

    void recordCommandBuffer(int imageIndex, float timeDelta)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapchain->swapChainExtent.width;
        viewport.height = (float)swapchain->swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapchain->swapChainExtent;

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        swapchain->beginRenderPass(commandBuffers[imageIndex], imageIndex);
        pipeline->bind(commandBuffers[imageIndex], &descriptorSets[imageIndex]);
        for (auto& gameObject : gameObjects) {
            gameObject.updateTick(timeDelta);
            pipeline->pushConstants(commandBuffers[imageIndex], sizeof(gameObject.transform.matrix), &gameObject.transform.matrix);
            gameObject.model->bind(commandBuffers[imageIndex]);
            gameObject.model->draw(commandBuffers[imageIndex]);
        }
        swapchain->endRenderPass(commandBuffers[imageIndex]);


        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void updateUniformBuffer(uint32_t currentImage)
    {

        UniformBufferObject ubo{};
        ubo.view = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), 
                glm::vec3(0.f, 0.f, 0.f),
                glm::vec3(0.f, 0.f, 1.f));
        ubo.proj = glm::perspective(glm::radians(45.f), 
                swapchain->swapChainExtent.width / (float) swapchain->swapChainExtent.height, 
                0.1f, 10.f);
        ubo.proj[1][1] *= -1;

        void* data;
        vkMapMemory(device.device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device.device, uniformBuffersMemory[currentImage]);
    }

    void createGameObjects()
    {
        auto model   = std::make_shared<MyModel>(device, MODEL_PATH.c_str());
        auto texture = std::make_shared<MyTexture>(device, TEXTURE_PATH.c_str());

        MyGameObject gameObject = MyGameObject::createGameObject(model, texture);
        gameObjects.push_back(std::move(gameObject));

        gameObject = MyGameObject::createGameObject(model, texture);
        gameObject.transform.matrix = glm::translate(gameObject.transform.matrix,
                glm::vec3(-1.f, -1.f, 2.f));
        gameObject.transform.matrix = glm::scale(gameObject.transform.matrix,
                glm::vec3(0.5f));
        gameObjects.push_back(std::move(gameObject));
    }

    void reCreateSwapChain()
    {
        VkExtent2D extent = window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.device);

        cleanupSwapChain();

        swapchain = std::make_unique<MySwapChain>(device,
                window.getExtent(), msaaSamples, std::move(swapchain));
        if (!swapchain->renderPassCompatible(pipeline->renderPass))
            pipeline = std::make_unique<MyPipeline>(device,
                    &descriptorSetLayout, msaaSamples, 
                    swapchain->renderPass);
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
        createCommandBuffers();
    }



    void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(device.device, &layoutInfo, nullptr, &descriptorSetLayout)
                != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void createUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(swapchain->size());
        uniformBuffersMemory.resize(swapchain->size());

        for (size_t i = 0; i < swapchain->size(); i++) {
            device.createBuffer(bufferSize,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    uniformBuffers[i],
                    uniformBuffersMemory[i]);
            updateUniformBuffer(i); //could be called every frame
        }
    }

    void createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapchain->size());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(swapchain->size());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(swapchain->size());

        if (vkCreateDescriptorPool(device.device, &poolInfo, nullptr, &descriptorPool)
                != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(swapchain->size(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapchain->size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapchain->size());
        if (vkAllocateDescriptorSets(device.device, &allocInfo, descriptorSets.data())
                != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < swapchain->size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo = gameObjects[0].texture->getImageInfo();

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr;
            descriptorWrites[0].pTexelBufferView = nullptr;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = nullptr;
            descriptorWrites[1].pImageInfo = &imageInfo;
            descriptorWrites[1].pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(device.device, 
                    static_cast<uint32_t>(descriptorWrites.size()), 
                    descriptorWrites.data(),
                    0, nullptr);
        }
    }


    void createCommandBuffers()
    {
        commandBuffers.resize(swapchain->size());
        device.allocateCommandBuffers(&commandBuffers);
    }

private:
    MyWindow window;
    MyDevice device{window};
    std::vector<MyGameObject> gameObjects{};
    std::unique_ptr<MySwapChain> swapchain;
    std::unique_ptr<MyPipeline> pipeline;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

public:
};
int main()
{
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "OK\n";
    return EXIT_SUCCESS;
}
