#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <optional>
#include <memory>
#include <thread>
#include <atomic>

#include <2019/intcode/Interpreter.hpp>

static std::atomic<bool> g_done{false};

struct Packet
{
    int64_t x;
    int64_t y;
};

class Computer
{
public:
    Computer(
        int64_t _address,
        std::string_view instructions_file,
        std::vector<std::unique_ptr<Computer>>& _network
    )
        : address(_address)
        , interpreter(instructions_file)
        , network(_network)
    {

    }

    auto Run() -> void
    {
        using Status = intcode::Interpreter::ExecuteStatusEnum;

        interpreter.Input(address);

        bool done{false};
        while(!done && !g_done)
        {
            auto exec_status = interpreter.Execute();

            switch(exec_status)
            {
                case Status::COMPLETED:
                    done = true;
                break;
                case Status::OUTPUT_READY:
                {
                    Packet packet{};
                    auto addr = interpreter.Output();
                    interpreter.Execute();
                    packet.x = interpreter.Output();
                    interpreter.Execute();
                    packet.y = interpreter.Output();

                    if(addr == 255)
                    {
                        std::cout << "addr 255; packet [" << packet.x << ", " << packet.y << "]" << '\n';
                        g_done = true;
                    }
                    else if(addr >= 0 && addr < 50)
                    {
                        network[addr]->EnqueuePacket(std::move(packet));
                    }
                    else
                    {
                        std::cout << "addr " << addr << " is invalid, dropping packet\n";
                    }
                }
                break;
                case Status::INPUT_REQUESTED:
                {
                    auto p_opt = dequeuePacket();
                    if(p_opt.has_value())
                    {
                        const auto& p = p_opt.value();
                        interpreter.Input(p.x);
                        interpreter.Execute();
                        interpreter.Input(p.y);
                    }
                    else
                    {
                        interpreter.Input(-1);
                    }
                }
                break;
            }
        }
    }

    auto EnqueuePacket(
        Packet packet
    ) -> void
    {
        std::lock_guard guard{packet_lock};
        packet_queue.emplace_back(std::move(packet));
    }

private:
    int64_t address{0};
    intcode::Interpreter interpreter;
    std::vector<std::unique_ptr<Computer>>& network;
    std::mutex packet_lock{};
    std::deque<Packet> packet_queue{};

    auto dequeuePacket() -> std::optional<Packet>
    {
        std::lock_guard guard{packet_lock};
        if(packet_queue.empty())
        {
            return std::nullopt;
        }
        else
        {
            Packet p = packet_queue.front();
            packet_queue.pop_front();
            return {std::move(p)};
        }
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<Computer>> computers{};
    computers.reserve(50);
    for(std::size_t i = 0; i < 50; ++i)
    {
        computers.emplace_back(std::make_unique<Computer>(i, args[1], computers));
    }

    std::vector<std::thread> workers{};
    workers.reserve(50);
    for(auto& c : computers)
    {
        workers.emplace_back([&c]() { c->Run(); });
    }

    for(auto& w : workers)
    {
        w.join();
    }

    return 0;
}
