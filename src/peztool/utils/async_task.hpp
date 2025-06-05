#pragma once
#include <functional>
#include <future>


namespace pez
{

struct AsyncTask
{
public:
    AsyncTask() = default;

    virtual ~AsyncTask()
    {
        if (m_running) {
            m_future.wait();
        }
    }

    [[nodiscard]]
    bool isUpdated() const
    {
        return m_future.valid() && m_updated;
    }

    void setConsumed()
    {
        m_updated = false;
    }

protected:
    void runTask()
    {
        // Don't allow multiple concurrent runs
        if (m_running) {
            return;
        }

        m_future = std::async(std::launch::async, [this]{
            m_updated = true;
            m_running = true;
            task();
            m_running = false;
        });
    }

    /// Blocks until the task is done
    void waitForCompletion() const
    {
        if (m_running) {
            m_future.wait();
        }
    }

    virtual void task() = 0;

private:
    std::future<void> m_future;
    std::function<void()> m_task;
    bool              m_updated{true};
    bool              m_running{false};
};

}