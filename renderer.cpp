#include "renderer.hpp"
#include "window.hpp"
#include "device.hpp"
#include "swapchain.hpp"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>
#include <functional>
#include <iostream>
#include <cassert>

MyRenderer::MyRenderer(MyWindow& window,
            MyDevice& device,
            VkSampleCountFlagBits msaaSamples,
            void* callbackObject,
            std::function<void(VkExtent2D, void*)> resizeCallback,
            std::function<void(VkRenderPass, void*)> renderPassUpdateCallback)
    : window(window),
      device(device),
      msaaSamples(msaaSamples),
      callbackObject(callbackObject),
      resizeCallback(resizeCallback),
      renderPassUpdateCallback(renderPassUpdateCallback)
{
    swapchain = std::make_unique<MySwapChain>(device,
            window.getExtent(), msaaSamples);
    createCommandBuffers();
}

void MyRenderer::createCommandBuffers()
{
    commandBuffers.resize(getSize());
    device.allocateCommandBuffers(&commandBuffers);
}

MyRenderer::~MyRenderer()
{ 
    device.freeCommandBuffers(&commandBuffers);
}


VkCommandBuffer MyRenderer::beginFrame()
{
    assert(!startedFrame && "only one frame at a time pls!");
    VkResult result = swapchain->acquireNextImage(&currentImageIdx);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        std::cout << "recreating swap chain out of date\n";
        reCreateSwapChain();
        return nullptr;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(commandBuffers[currentCommandBufferIdx], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    startedFrame = true;
    return commandBuffers[currentCommandBufferIdx];
}

void MyRenderer::endFrame(VkCommandBuffer commandBuffer)
{
    assert(startedFrame && "Cannot end unstarted frame!");
    assert(commandBuffer == commandBuffers[currentCommandBufferIdx] && "can't work on old commandBuffer");

    if (vkEndCommandBuffer(commandBuffers[currentCommandBufferIdx]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    VkResult result = swapchain->submitCommandBuffers(&commandBuffer, currentImageIdx);
    if (result != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    result = swapchain->present(currentImageIdx);
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
    startedFrame = false;
    currentCommandBufferIdx = (currentCommandBufferIdx + 1) % getSize();
}

void MyRenderer::beginRenderPass(VkCommandBuffer commandBuffer)
{
    assert(startedFrame && "Cannot end unstarted frame!");
    assert(commandBuffer == commandBuffers[currentCommandBufferIdx] && "can't work on old commandBuffer");

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

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapchain->getRenderPass();
    renderPassInfo.framebuffer = swapchain->getFramebuffer(currentImageIdx);
    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = swapchain->swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0] = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1] = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void MyRenderer::endRenderPass(VkCommandBuffer commandBuffer)
{
    assert(startedFrame && "Cannot end unstarted frame!");
    assert(commandBuffer == commandBuffers[currentCommandBufferIdx] && "can't work on old commandBuffer");

    vkCmdEndRenderPass(commandBuffer);
}

void MyRenderer::reCreateSwapChain()
{
    VkExtent2D extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.device);

    std::shared_ptr<MySwapChain> oldSwapchain = std::move(swapchain);
    swapchain = std::make_unique<MySwapChain>(device,
            extent, msaaSamples, oldSwapchain);

    if (window.wasResized()) {
        if (resizeCallback) 
            resizeCallback(extent, callbackObject);
        else 
            std::cout << "Warning: Resized without callback\n";
    }

    if (!swapchain->renderPassCompatible(oldSwapchain)) {
        if (renderPassUpdateCallback) 
            renderPassUpdateCallback(swapchain->getRenderPass(), callbackObject);
        else 
            throw std::runtime_error("Error: RenderPass incompatible, and no callback provided");
    }

}

uint32_t MyRenderer::getIndex() const
{
    return currentCommandBufferIdx;    
}

VkRenderPass MyRenderer::getSwapChainRenderPass() const
{
    return swapchain->getRenderPass();
}

size_t MyRenderer::getSize() const
{
    return swapchain->size();
}

VkExtent2D MyRenderer::getSwapChainExtent() const
{
    return swapchain->swapChainExtent;
}
