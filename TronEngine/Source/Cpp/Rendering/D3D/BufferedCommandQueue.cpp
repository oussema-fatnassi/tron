#include "../../../Headers/Rendering/D3D/BufferedCommandQueue.hpp"
#include <iostream>
#include <algorithm>

BufferedCommandQueue::BufferedCommandQueue() {
    // Pre-allocate reasonable buffer sizes to avoid reallocations
    currentFrameBuffer.reserve(1000);
    previousFrameBuffer.reserve(1000);
    renderBuffer.reserve(1000);

    std::cout << "[BufferedCommandQueue] Initialized - Ready to eliminate flickering!\n";
    std::cout << "[BufferedCommandQueue] PHASE 2 READY: Interpolation support built-in\n";
}

BufferedCommandQueue::~BufferedCommandQueue() {
    Clear();
    std::cout << "[BufferedCommandQueue] Destroyed\n";
}

void BufferedCommandQueue::PushCommands(const std::vector<RenderCommand>& commands) {
    if (commands.empty()) {
        return; // Don't update with empty data
    }

    {
        std::lock_guard<std::mutex> lock(bufferMutex);

        // Swap previous and current for future interpolation
        previousFrameBuffer = std::move(currentFrameBuffer);
        currentFrameBuffer = commands;

        // Update what render thread will read
        renderBuffer = currentFrameBuffer;

        hasData.store(true);
        framesProduced.fetch_add(1);
    }

    // Debug every 300 frames (~2.5 seconds at 120fps)
    if (framesProduced.load() % 300 == 0) {
        std::cout << "[BufferedCommandQueue] Buffered " << commands.size()
            << " commands (Frame #" << framesProduced.load() << ")\n";
    }
}

void BufferedCommandQueue::PushCommand(const RenderCommand& command) {
    std::vector<RenderCommand> singleCommand = { command };
    PushCommands(singleCommand);
}

std::vector<RenderCommand> BufferedCommandQueue::GetRenderCommands() {
    std::vector<RenderCommand> commandsToRender;

    {
        std::lock_guard<std::mutex> lock(bufferMutex);

        if (!renderBuffer.empty()) {
            commandsToRender = renderBuffer;
            framesConsumed.fetch_add(1);
        }
        else if (!previousFrameBuffer.empty()) {
            // FLICKERING PREVENTION: Use previous frame if no new data
            commandsToRender = previousFrameBuffer;
            emptyFramesPrevented.fetch_add(1);

            if (emptyFramesPrevented.load() % 60 == 1) { // Log every second
                std::cout << "[BufferedCommandQueue] ANTI-FLICKER: Using previous frame data\n";
            }
        }
    }

    return commandsToRender;
}

std::vector<RenderCommand> BufferedCommandQueue::GetInterpolatedCommands(float alpha) {
    // PHASE 2: Professional interpolation (future implementation)
    std::vector<RenderCommand> interpolatedCommands;

    {
        std::lock_guard<std::mutex> lock(bufferMutex);

        if (currentFrameBuffer.empty() && previousFrameBuffer.empty()) {
            return interpolatedCommands; // No data to interpolate
        }

        if (previousFrameBuffer.empty()) {
            // First frame - no interpolation possible
            return currentFrameBuffer;
        }

        if (currentFrameBuffer.empty()) {
            // No new frame - return previous
            return previousFrameBuffer;
        }

        // INTERPOLATE between previous and current frames
        size_t maxCommands = std::max(previousFrameBuffer.size(), currentFrameBuffer.size());
        interpolatedCommands.reserve(maxCommands);

        for (size_t i = 0; i < maxCommands; ++i) {
            if (i < currentFrameBuffer.size() && i < previousFrameBuffer.size()) {
                // Both frames have this command - interpolate
                RenderCommand interpolated = InterpolateCommand(
                    previousFrameBuffer[i],
                    currentFrameBuffer[i],
                    alpha
                );
                interpolatedCommands.push_back(interpolated);
            }
            else if (i < currentFrameBuffer.size()) {
                // Only current frame has this command
                interpolatedCommands.push_back(currentFrameBuffer[i]);
            }
            else if (i < previousFrameBuffer.size()) {
                // Only previous frame has this command
                interpolatedCommands.push_back(previousFrameBuffer[i]);
            }
        }
    }

    return interpolatedCommands;
}

RenderCommand BufferedCommandQueue::InterpolateCommand(const RenderCommand& prev,
    const RenderCommand& curr,
    float alpha) const {
    // PHASE 2: Smooth interpolation between two frames
    RenderCommand result = curr; // Start with current command

    // Interpolate position
    result.transform.position[0] = Lerp(prev.transform.position[0], curr.transform.position[0], alpha);
    result.transform.position[1] = Lerp(prev.transform.position[1], curr.transform.position[1], alpha);
    result.transform.position[2] = Lerp(prev.transform.position[2], curr.transform.position[2], alpha);

    // Interpolate rotation
    result.transform.rotation[0] = Lerp(prev.transform.rotation[0], curr.transform.rotation[0], alpha);
    result.transform.rotation[1] = Lerp(prev.transform.rotation[1], curr.transform.rotation[1], alpha);
    result.transform.rotation[2] = Lerp(prev.transform.rotation[2], curr.transform.rotation[2], alpha);

    // Interpolate scale
    result.transform.scale[0] = Lerp(prev.transform.scale[0], curr.transform.scale[0], alpha);
    result.transform.scale[1] = Lerp(prev.transform.scale[1], curr.transform.scale[1], alpha);
    result.transform.scale[2] = Lerp(prev.transform.scale[2], curr.transform.scale[2], alpha);

    // Interpolate color
    result.color.r = Lerp(prev.color.r, curr.color.r, alpha);
    result.color.g = Lerp(prev.color.g, curr.color.g, alpha);
    result.color.b = Lerp(prev.color.b, curr.color.b, alpha);
    result.color.a = Lerp(prev.color.a, curr.color.a, alpha);

    return result;
}

void BufferedCommandQueue::Clear() {
    std::lock_guard<std::mutex> lock(bufferMutex);
    currentFrameBuffer.clear();
    previousFrameBuffer.clear();
    renderBuffer.clear();
    hasData.store(false);
}

size_t BufferedCommandQueue::GetSize() const {
    std::lock_guard<std::mutex> lock(bufferMutex);
    return renderBuffer.size();
}

void BufferedCommandQueue::SwapBuffers() {
    // Efficient buffer swapping for minimal memory allocations
    previousFrameBuffer.swap(currentFrameBuffer);
}

void BufferedCommandQueue::PrintBufferStats() const {
    uint32_t produced = framesProduced.load();
    uint32_t consumed = framesConsumed.load();
    uint32_t prevented = emptyFramesPrevented.load();

    std::cout << "\n[BufferedCommandQueue] === ANTI-FLICKER STATS ===\n";
    std::cout << "  Frames produced: " << produced << "\n";
    std::cout << "  Frames consumed: " << consumed << "\n";
    std::cout << "  Empty frames prevented: " << prevented << "\n";

    if (produced > 0) {
        float flickerPrevention = (static_cast<float>(prevented) / produced) * 100.0f;
        std::cout << "  Flicker prevention: " << flickerPrevention << "%\n";
    }

    std::cout << "  Current buffer size: " << GetSize() << " commands\n";
    std::cout << "  Has data: " << (hasData.load() ? "YES" : "NO") << "\n";
    std::cout << "================================================\n\n";
}