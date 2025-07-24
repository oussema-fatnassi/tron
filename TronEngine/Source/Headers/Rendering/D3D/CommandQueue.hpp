#pragma once
#include <queue>
#include <mutex>

struct RenderCommand {
    // Exemple : struct pour DrawMesh, etc.
    // Ajoute ici type, données, etc.
};

class CommandQueue {
public:
    void Push(const RenderCommand& cmd);
    bool Pop(RenderCommand& outCmd);
    void Flush();

private:
    std::queue<RenderCommand> queue;
    std::mutex mutex;
};
