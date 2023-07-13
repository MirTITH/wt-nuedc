#pragma once

namespace control_system
{

template <typename T>
class RingList
{
private:
    typedef struct Node {
        T data;
        Node *next;
    } node_t;

    node_t *head;

    void Construct(int length)
    {
        if (length == 0) {
            length = 1;
        }

        head      = new node_t;
        Node *ptr = head;

        for (size_t i = length - 1; i > 0; i--) {
            ptr->next = new node_t;
            ptr       = ptr->next;
        }

        ptr->next = head;
    }

    void DeleteAll()
    {
        Node *ptr  = head;
        Node *next = ptr->next;

        while (next != head) {
            delete ptr;
            ptr  = next;
            next = ptr->next;
        }

        delete ptr;
        head = nullptr;
    }

public:
    RingList(size_t length = 1)
    {
        Construct(length);
    }

    T &spin()
    {
        head = head->next;
        return head->data;
    }

    T &get()
    {
        return head->data;
    }

    void resize(size_t size)
    {
        DeleteAll();
        Construct(size);
    }

    void insert_after(const T &data)
    {
        auto temp  = new T(data);
        temp->next = head->next;
        head->next = temp;
    }

    void fill(const T &data)
    {
        auto ptr = head;
        do {
            ptr->data = data;
            ptr       = ptr->next;
        } while (ptr != head);
    }

    ~RingList()
    {
        DeleteAll();
    }
};

} // namespace control_system
