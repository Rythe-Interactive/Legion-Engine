#pragma once
#include <atomic>
#include <memory>
#include <thread>

#include <core/platform/platform.hpp>
#include <core/types/types.hpp>
#include <core/async/wait_priority.hpp>
#include <core/containers/delegate.hpp>

namespace legion::core::async
{
    struct async_progress
    {
    protected:
        size_type m_size;
        std::atomic<size_type> m_progress;

    public:
        async_progress(size_type size) : m_size(size), m_progress(0) {}

        size_type size() const noexcept;
        size_type rawProgress() const noexcept;

        void complete() noexcept;
        void advance_progress(size_type progress = 1) noexcept;
        bool is_done() const noexcept;
        float progress() const noexcept;
    };

    template<typename functor>
    struct async_operation
    {
    protected:
        std::shared_ptr<async_progress> m_progress;
        functor m_repeater;
    public:
        async_operation(const std::shared_ptr<async_progress>& progress, functor&& repeater) : m_progress(progress), m_repeater(repeater) {}
        async_operation() = default;
        async_operation(const async_operation&) = default;
        async_operation(async_operation&&) = default;

        bool is_done() const noexcept
        {
            return m_progress->is_done();
        }

        float progress() const noexcept
        {
            return m_progress->progress();
        }

        virtual void wait(wait_priority priority = wait_priority_normal) const noexcept
        {
            OPTICK_EVENT("legion::core::async::async_operation<T>::wait");
            while (!m_progress->is_done())
            {
                switch (priority)
                {
                case wait_priority::sleep:
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                    break;
                case wait_priority::normal:
                    std::this_thread::yield();
                    break;
                case wait_priority::real_time:
                default:
                    L_PAUSE_INSTRUCTION();
                    break;
                }
            }
        }

        template<typename... argument_types>
        auto then(argument_types... args) const
        {
            wait();
            return m_repeater(std::forward<argument_types>(args)...);
        }

        template<typename... argument_types>
        auto then(wait_priority priority, argument_types... args) const
        {
            wait(priority);
            return m_repeater(std::forward<argument_types>(args)...);
        }

        virtual ~async_operation() = default;
    };

#if !defined(DOXY_EXCLUDE)
    template<typename functor>
    async_operation(const std::shared_ptr<async_progress>&, functor&&)->async_operation<functor>;
#endif
}
