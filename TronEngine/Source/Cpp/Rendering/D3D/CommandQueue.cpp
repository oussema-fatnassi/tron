#include "../../../Headers/Rendering/D3D/CommandQueue.hpp"
#include <iostream>

CommandQueue::CommandQueue() {
    std::cout << "[CommandQueue] Thread-safe render command queue initialized\n";
}

CommandQueue::~CommandQueue() {
    Clear();
    std::cout << "[CommandQueue] Command queue destroyed\n";
}

void CommandQueue::PushCommand(const RenderCommand& command) {
    std::lock_guard<std::mutex> lock(queueMutex);
    commandQueue.push(command);
}

void CommandQueue::PushCommands(const std::vector<RenderCommand>& commands) {
    std::lock_guard<std::mutex> lock(queueMutex);
    for (const auto& cmd : commands) {
        commandQueue.push(cmd);
    }
}

bool CommandQueue::PopCommand(RenderCommand& outCommand) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (commandQueue.empty()) {
        return false;
    }

    outCommand = commandQueue.front();
    commandQueue.pop();
    return true;
}

std::vector<RenderCommand> CommandQueue::PopAllCommands() {
    std::lock_guard<std::mutex> lock(queueMutex);
    std::vector<RenderCommand> commands;

    while (!commandQueue.empty()) {
        commands.push_back(commandQueue.front());
        commandQueue.pop();
    }

    return commands;
}

bool CommandQueue::IsEmpty() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return commandQueue.empty();
}

size_t CommandQueue::GetSize() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return commandQueue.size();
}

void CommandQueue::Clear() {
    std::lock_guard<std::mutex> lock(queueMutex);
    std::queue<RenderCommand> empty;
    commandQueue.swap(empty);
}

void CommandQueue::DebugPrintQueueState() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    std::cout << "[CommandQueue] Current queue size: " << commandQueue.size() << " commands\n";

    if (!commandQueue.empty()) {
        std::cout << "[CommandQueue] Next command type: ";
        switch (commandQueue.front().type) {
        case RenderCommandType::DRAW_MESH:
            std::cout << "DRAW_MESH (" << commandQueue.front().meshName << ")\n";
            break;
        case RenderCommandType::CLEAR_SCREEN:
            std::cout << "CLEAR_SCREEN\n";
            break;
        case RenderCommandType::BEGIN_FRAME:
            std::cout << "BEGIN_FRAME\n";
            break;
        case RenderCommandType::END_FRAME:
            std::cout << "END_FRAME\n";
            break;
        default:
            std::cout << "UNKNOWN\n";
            break;
        }
    }
}

// Legacy methods for compatibility
void CommandQueue::Push(const RenderCommand& cmd) {
    PushCommand(cmd);
}

bool CommandQueue::Pop(RenderCommand& outCmd) {
    return PopCommand(outCmd);
}

void CommandQueue::Flush() {
    Clear();
}