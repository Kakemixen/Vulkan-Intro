#include "device.hpp"
#include "window.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "simple_render_system.hpp"

//libs
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//std
#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <memory>

//cstd - why is memcpy in cstring
#include <cstring> 


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
    ~HelloTriangleApplication() 
    {
        cleanup();
    }

    void run() {
        createGameObjects();
        initVulkan();
        mainLoop();
        //cleanup();
    }

private:

    void initVulkan() 
    {
        createDescriptorSetLayout();
        renderSystem = std::make_unique<SimpleRenderSystem>(device,
                renderer.getSwapChainRenderPass(),
                &descriptorSetLayout);
        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
    }

    void mainLoop() 
    {
        while (!window.shouldClose()) {
            glfwPollEvents();
            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float timeDelta = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
            startTime = std::chrono::high_resolution_clock::now();
            for (auto& gameObject : gameObjects) {
                gameObject.updateTick(timeDelta);
            }

            VkCommandBuffer commandBuffer = renderer.beginFrame();
            renderer.beginRenderPass(commandBuffer);
            renderSystem->renderGameObjects(commandBuffer, gameObjects, &descriptorSets[renderer.getIndex()]);
            renderer.endRenderPass(commandBuffer);
            renderer.endFrame(commandBuffer);
        }
        vkDeviceWaitIdle(device.device);
    }

    void cleanup() 
    {
        cleanupBuffers();

        vkDestroyDescriptorSetLayout(device.device, descriptorSetLayout, nullptr);
        glfwTerminate();
    }

    void cleanupBuffers()
    {

        for (size_t i = 0; i < renderer.getSize(); i++) {
            vkDestroyBuffer(device.device, uniformBuffers[i], nullptr);
            vkFreeMemory(device.device, uniformBuffersMemory[i], nullptr);
        }

        vkDestroyDescriptorPool(device.device, descriptorPool, nullptr);
    }


    void updateUniformBuffer(uint32_t currentImage)
    {

        UniformBufferObject ubo{};
        ubo.view = glm::lookAt(glm::vec3(2.f, 2.f, 2.f), 
                glm::vec3(0.f, 0.f, 0.f),
                glm::vec3(0.f, 0.f, 1.f));
        ubo.proj = glm::perspective(glm::radians(45.f), 
                renderer.getAspectRatio(),
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

        uniformBuffers.resize(renderer.getSize());
        uniformBuffersMemory.resize(renderer.getSize());

        for (size_t i = 0; i < renderer.getSize(); i++) {
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
        poolSizes[0].descriptorCount = static_cast<uint32_t>(renderer.getSize());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(renderer.getSize());

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(renderer.getSize());

        if (vkCreateDescriptorPool(device.device, &poolInfo, nullptr, &descriptorPool)
                != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void createDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(renderer.getSize(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(renderer.getSize());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(renderer.getSize());
        if (vkAllocateDescriptorSets(device.device, &allocInfo, descriptorSets.data())
                != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < renderer.getSize(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            //TODO no
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

public: //TODO perhaps another way, friend?
    static void resizeCallback(VkExtent2D newExtent, void* obj)
    {
        HelloTriangleApplication* app = reinterpret_cast<HelloTriangleApplication*>(obj);
        app->recreateBuffers();
    }

    void recreateBuffers()
    {
        cleanupBuffers();

        createUniformBuffers();
        createDescriptorPool();
        createDescriptorSets();
    }

    static void renderPassUpdateCallback(VkRenderPass newRenderPass, void* obj)
    {
        HelloTriangleApplication* app = reinterpret_cast<HelloTriangleApplication*>(obj);
        app->recreatePipeline(newRenderPass);
    }

    void recreatePipeline(VkRenderPass newRenderPass)
    {
        renderSystem->createNewPipeline(newRenderPass, &descriptorSetLayout);
    }

private:
    MyWindow window;
    MyDevice device{window};
    MyRenderer renderer{window, device, 
            device.getMaxUsableSampleCount(),
            static_cast<void*>(this), 
            &HelloTriangleApplication::resizeCallback,
            &HelloTriangleApplication::renderPassUpdateCallback};
    std::vector<MyGameObject> gameObjects{};
    VkDescriptorSetLayout descriptorSetLayout;
    std::unique_ptr<SimpleRenderSystem> renderSystem;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

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
