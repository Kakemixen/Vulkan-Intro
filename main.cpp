#include "device.hpp"
#include "window.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "simple_render_system.hpp"
#include "camera.hpp"
#include "descriptor_manager.hpp"

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

struct UniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class HelloTriangleApplication
{
public:
    ~HelloTriangleApplication() 
    {
        cleanupBuffers();
        glfwTerminate();
    }

    void run() {
        createGameObjects();
        initVulkan();
        mainLoop();
    }

private:

    void createGameObjects()
    {
        auto model    = std::make_shared<MyModel>(device, "models/companion_cube.obj");
        auto texture  = std::make_shared<MyTexture>(device, "textures/companion_cube.png");
        auto texture2 = std::make_shared<MyTexture>(device, "textures/companion_cube_blue.png");

        MyGameObject gameObject = MyGameObject::createGameObject(model, texture);
        gameObjects.push_back(std::move(gameObject));

        gameObject = MyGameObject::createGameObject(model, texture2);
        gameObject.transform.matrix = glm::translate(gameObject.transform.matrix,
                glm::vec3(-1.f, -1.f, 2.f));
        gameObject.transform.matrix = glm::scale(gameObject.transform.matrix,
                glm::vec3(0.5f));
        gameObjects.push_back(std::move(gameObject));
        models.push_back(std::move(model));
        textures.push_back(std::move(texture));
        textures.push_back(std::move(texture2));
    }

    void initVulkan() 
    {
        createDescriptorSetLayout();
        createUniformBuffers();
        createDescriptorPool();
        descriptorManager.createDescriptorSets(renderer.getSize(), textures);
        updateDescriptorSets();
        renderSystem = std::make_unique<SimpleRenderSystem>(device,
                renderer.getSwapChainRenderPass(),
                descriptorManager.getDescriptorSetLayout());
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
            renderSystem->renderGameObjects(commandBuffer, gameObjects, 
                    descriptorManager.getGlobalDescriptorSets(renderer.getIndex()));
            renderer.endRenderPass(commandBuffer);
            renderer.endFrame(commandBuffer);
        }
        vkDeviceWaitIdle(device.device);
    }

    void cleanupBuffers()
    {
        for (size_t i = 0; i < renderer.getSize(); i++) {
            vkDestroyBuffer(device.device, uniformBuffers[i], nullptr);
            vkFreeMemory(device.device, uniformBuffersMemory[i], nullptr);
        }
    }


    void updateUniformBuffer(uint32_t currentImage)
    {
        UniformBufferObject ubo{};
        ubo.view = camera.getView();
        ubo.proj = camera.getProjection();

        void* data;
        vkMapMemory(device.device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device.device, uniformBuffersMemory[currentImage]);
    }


    void createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;

        std::vector<VkDescriptorSetLayoutBinding> globalBindings = {uboLayoutBinding};
        descriptorManager.createGlobalDescriptorSetLayout(globalBindings);

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 0;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::vector<VkDescriptorSetLayoutBinding> textureBindings = {samplerLayoutBinding};
        descriptorManager.createTextureDescriptorSetLayout(textureBindings);
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
        std::vector<VkDescriptorPoolSize> poolSizes(2, VkDescriptorPoolSize{});
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(renderer.getSize());
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(textures.size());

        device.createDescriptorPool(poolSizes, 
                renderer.getSize() * 2);
    }

    void updateDescriptorSets()
    {
        descriptorManager.updateTextureDescriptorSets(textures);

        std::vector<VkDescriptorBufferInfo> bufferInfos(renderer.getSize(),
                VkDescriptorBufferInfo{});

        for (size_t i = 0; i < renderer.getSize(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            descriptorManager.updateGlobalDescriptorSets(i,
                    bufferInfo);
        }
    }

public: //TODO perhaps another way, friend?
    static void resizeCallback(VkExtent2D newExtent, void* obj)
    {
        HelloTriangleApplication* app = 
            reinterpret_cast<HelloTriangleApplication*>(obj);
        app->camera.updateAr(
                MyCamera::calculateAspectRatio(
                    static_cast<uint32_t>(newExtent.width),
                    static_cast<uint32_t>(newExtent.height))
                );
        app->updateBuffers();
    }

    void updateBuffers()
    {
        for (size_t i = 0; i < renderer.getSize(); i++) {
            updateUniformBuffer(i); 
        }
    }

    static void renderPassUpdateCallback(VkRenderPass newRenderPass, void* obj)
    {
        HelloTriangleApplication* app = 
            reinterpret_cast<HelloTriangleApplication*>(obj);
        app->recreatePipeline(newRenderPass);
    }

    void recreatePipeline(VkRenderPass newRenderPass)
    {
        renderSystem->createNewPipeline(newRenderPass, 
                descriptorManager.getDescriptorSetLayout());
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
    std::vector<std::shared_ptr<MyTexture>> textures{};
    std::vector<std::shared_ptr<MyModel>> models{};
    std::unique_ptr<SimpleRenderSystem> renderSystem;
    MyCamera camera{{2.0f, 2.0f, 2.0f},
            {-2.f, -2.f, -2.f}, 
            {0.f, 0.f, 1.f},
            MyCamera::calculateAspectRatio(
                    static_cast<uint32_t>(renderer.getSwapChainExtent().width),
                    static_cast<uint32_t>(renderer.getSwapChainExtent().height))
            };
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    MyDescriptorManager descriptorManager{device};

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
