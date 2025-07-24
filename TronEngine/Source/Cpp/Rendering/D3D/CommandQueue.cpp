#include "../../../Headers/Rendering/D3D/CommandQueue.h"

void CommandQueue::Push(const RenderCommand& cmd) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(cmd);
}

bool CommandQueue::Pop(RenderCommand& outCmd) {
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty()) return false;
    outCmd = queue.front();
    queue.pop();
    return true;
}

void CommandQueue::Flush() {
    // Placeholder — dans une vraie archi, cette méthode
    // sera remplie depuis la GameThread avec tous les Draw à faire
}
