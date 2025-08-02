#pragma once
#include <vector>
#include <mutex>
#include <atomic>
#include "../../Communication/RenderCommand.hpp"

// <summary>
// BufferedCommandQueue - Solves flickering by maintaining frame data
// Ensures render thread always has commands to execute, preventing black frames
// Sets foundation for future interpolation implementation
// </summary>
// <remarks>
// PHASE 1: Eliminates flickering by buffering the last frame's commands
// PHASE 2 READY: Already stores previousFrame data needed for interpolation
// 
// Thread Safety: Fully thread-safe with minimal locking overhead
// Performance: Double buffering approach - no dynamic allocations during runtime
// </remarks>
class BufferedCommandQueue {
private:
    // Double buffer system - ready for interpolation!
    std::vector<RenderCommand> currentFrameBuffer;
    std::vector<RenderCommand> previousFrameBuffer;
    std::vector<RenderCommand> renderBuffer;  // What render thread reads

    mutable std::mutex bufferMutex;
    std::atomic<bool> hasData{ false };

    // Statistics for debugging
    std::atomic<uint32_t> framesProduced{ 0 };
    std::atomic<uint32_t> framesConsumed{ 0 };
    std::atomic<uint32_t> emptyFramesPrevented{ 0 };

public:
    BufferedCommandQueue();
    ~BufferedCommandQueue();

    // PHASE 1: Game thread pushes commands (replaces old PushCommands)
    void PushCommands(const std::vector<RenderCommand>& commands);
    void PushCommand(const RenderCommand& command);

    // PHASE 1: Render thread gets buffered commands (replaces old PopAllCommands)
    std::vector<RenderCommand> GetRenderCommands();

    // PHASE 2 READY: Future interpolation method (not implemented yet)
    std::vector<RenderCommand> GetInterpolatedCommands(float alpha);

    // Utility methods
    bool HasData() const { return hasData.load(); }
    void Clear();

    // Debug information
    void PrintBufferStats() const;
    uint32_t GetFramesProduced() const { return framesProduced.load(); }
    uint32_t GetFramesConsumed() const { return framesConsumed.load(); }
    uint32_t GetEmptyFramesPrevented() const { return emptyFramesPrevented.load(); }

    // Legacy compatibility (for existing code)
    bool IsEmpty() const { return !HasData(); }
    size_t GetSize() const;

private:
    // Helper method for efficient buffer swapping
    void SwapBuffers();

    // PHASE 2 READY: Linear interpolation helper
    float Lerp(float a, float b, float t) const { return a + t * (b - a); }

    // PHASE 2 READY: Interpolate single command
    RenderCommand InterpolateCommand(const RenderCommand& prev,
        const RenderCommand& curr,
        float alpha) const;
};