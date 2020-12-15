#ifndef queue_h_
#define queue_h_

#include <deque>

template<class Sygnal>
class QueueNode : std::pair<Sygnal, void*> {
public:
  QueueNode(const Sygnal & s, void * data) : std::pair<Sygnal, void *>(s, data) { };
  Sygnal sygnal() {
    return std::pair<Sygnal, void *>::first;
  }
  void * data() {
    return std::pair<Sygnal, void *>::second;
  }
};

template<class Sygnal>
class Queue : public std::deque<QueueNode<Sygnal>> {
public:
  void push(const Sygnal &, void *);
  bool isEmpty() const;
  QueueNode<Sygnal> pop();
};

template<class Sygnal>
void Queue<Sygnal>::push(const Sygnal & sygnal, void * data) {
  std::deque<QueueNode<Sygnal>>::push_back(QueueNode<Sygnal>(sygnal, data));
}

template<class Sygnal>
QueueNode<Sygnal> Queue<Sygnal>::pop() {
  const QueueNode<Sygnal> res = std::deque<QueueNode<Sygnal>>::front();
  std::deque<QueueNode<Sygnal>>::pop_front();
  return res;
}

template<class Sygnal>
bool Queue<Sygnal>::isEmpty() const {
  return std::deque<QueueNode<Sygnal>>::empty();
}

#endif
