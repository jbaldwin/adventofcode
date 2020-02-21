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

class NAT;

struct Packet
{
    int64_t x;
    int64_t y;

    friend auto operator<<(std::ostream& os, const Packet& p) -> std::ostream&
    {
        os << "{";
        os << "x=" << p.x;
        os << ",y=" << p.y;
        os << "}";

        return os;
    }
};

class Computer
{
public:
    Computer(
        int64_t _address,
        std::string_view instructions_file,
        std::vector<std::unique_ptr<Computer>>& _network,
        NAT& _nat
    )
        : address(_address)
        , interpreter(instructions_file)
        , network(_network)
        , nat(_nat)
    {

    }

    auto Run() -> void;

    auto EnqueuePacket(
        Packet packet
    ) -> void
    {
        std::lock_guard guard{packet_lock};
        packet_queue.emplace_back(std::move(packet));
    }

    auto QueueEmpty() -> bool
    {
        std::lock_guard guard{packet_lock};
        return packet_queue.empty();
    }

private:
    int64_t address{0};
    intcode::Interpreter interpreter;
    std::vector<std::unique_ptr<Computer>>& network;
    NAT& nat;
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

class NAT
{
public:
    NAT(
        std::vector<std::unique_ptr<Computer>>& _network
    )
        : network(_network)
    {
        
    }

    auto Run() -> void
    {
        using namespace std::chrono_literals;
        while(!g_done)
        {
            // 10ms isn't long enough, so perhaps there is a more 
            // deterministic way to let all the computers execute long enough?
            std::this_thread::sleep_for(100ms);

            bool all_empty{true};
            for(auto& c : network)
            {
                if(!c->QueueEmpty())
                {
                    all_empty = false;
                    break;
                }
            }

            if(all_empty)
            {
                std::cout << "Sending packet to computer 0: " <<  current << '\n';

                if(current.y == previous.y)
                {
                    g_done = true;
                }

                network[0]->EnqueuePacket(current);
                previous = current;
            }
        }
    }

    auto EnqueuePacket(Packet packet) -> void
    {
        std::lock_guard guard{packet_lock};
        current = std::move(packet);
    }

private:
    std::vector<std::unique_ptr<Computer>>& network;

    std::mutex packet_lock;

    Packet previous;
    Packet current;
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

    NAT nat{computers};

    computers.reserve(50);
    for(std::size_t i = 0; i < 50; ++i)
    {
        computers.emplace_back(std::make_unique<Computer>(i, args[1], computers, nat));
    }

    std::vector<std::thread> workers{};
    workers.reserve(50);
    for(auto& c : computers)
    {
        workers.emplace_back([&c]() { c->Run(); });
    }

    nat.Run();

    for(auto& w : workers)
    {
        w.join();
    }

    return 0;
}

auto Computer::Run() -> void
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
                    nat.EnqueuePacket(std::move(packet));
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
                    using namespace std::chrono_literals;
                    interpreter.Input(-1);
                    std::this_thread::sleep_for(10ms);
                }
            }
            break;
        }
    }
}