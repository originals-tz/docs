#include <atomic>
#include <mutex>
#include <optional>
#include <unordered_map>

template <typename T>
class Queue
{
    struct Node
    {
        std::optional<T> m_data;
        Node* next = nullptr;
    };

public:
    Queue()
    {
        m_cur = &m_root;
        m_size = 0;
    }

    ~Queue()
    {
        while (m_root.next)
        {
            auto tmp = m_root.next;
            m_root.next = tmp->next;
            delete tmp;
        }
    }

    void Add(const T& data)
    {
        auto node = new Node();
        node->m_data = data;
        m_cur->next = node;
        m_cur = node;
        m_size++;
    }

    void Add()
    {
        auto node = new Node();
        m_cur->next = node;
        m_cur = node;
        m_size++;
    }

    std::optional<T> Pop()
    {
        auto tmp = m_root.next;
        if (tmp)
        {
            auto op = std::move(tmp->m_data);
            m_root.next = tmp->next;
            delete tmp;
            m_size--;
            return op;
        }
        return std::nullopt;
    }

    size_t GetSize() { return m_size; }

private:
    Node m_root;
    Node* m_cur;
    std::atomic_size_t m_size;
};

template <typename T>
class SCSP
{
public:
    SCSP()
        : m_begin(0)
        , m_end(0)
        , m_stop(false)
    {
        m_queue.Add();
    }

    void Stop()
    {
        m_stop = true;
        cond.notify_one();
    }

    void Push(const T& data)
    {
        m_queue.Add(data);
        cond.notify_one();
        m_end++;
    }

    std::optional<T> Pop()
    {
        if (m_begin == m_end)
        {
            std::unique_lock<std::mutex> lk(mut);
            cond.wait(lk, [&] { return m_stop || m_begin < m_end; });
        }
        auto data = m_queue.Pop();
        m_begin++;
        return data;
    }

private:
    Queue<T> m_queue;
    std::mutex mut;
    std::condition_variable cond;
    std::atomic_int m_begin;
    std::atomic_int m_end;
    std::atomic_bool m_stop;
};
