#pragma once
#include <queue>
#include <mutex>
#include <vector>
#include "../../Communication/RenderCommand.hpp"

// Thread-safe command queue for communication between Game and Render threads
class CommandQueue {
private:
    std::queue<RenderCommand> commandQueue;
    mutable std::mutex queueMutex;

public:
    CommandQueue();
    ~CommandQueue();

    // Push a single render command (thread-safe)
    void PushCommand(const RenderCommand& command);

    // Push multiple render commands at once (more efficient)
    void PushCommands(const std::vector<RenderCommand>& commands);

    // Pop a single command (thread-safe)
    bool PopCommand(RenderCommand& outCommand);

    // Get all commands at once (more efficient for batch processing)
    std::vector<RenderCommand> PopAllCommands();

    // Check if queue is empty (thread-safe)
    bool IsEmpty() const;

    // Get queue size (thread-safe)
    size_t GetSize() const;

    // Clear all commands (thread-safe)
    void Clear();

    // Debug methods
    void DebugPrintQueueState() const;

    // Legacy methods for compatibility (can be removed later)
    void Push(const RenderCommand& cmd);
    bool Pop(RenderCommand& outCmd);
    void Flush();
};