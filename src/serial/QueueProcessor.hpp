#pragma once

#include <functional>
#include <queue>

#include <QObject>

class QueueProcessor final : public QObject {
  Q_OBJECT

 public:
  using Task = std::function<void()>;

  explicit QueueProcessor(QObject* parent = nullptr);

  void enqueue(Task task);

 signals:
  void queueDrained();

 private:
  void processNext();

  std::queue<Task> queue_;
  bool running_{false};
};
