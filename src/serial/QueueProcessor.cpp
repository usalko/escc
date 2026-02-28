#include "QueueProcessor.hpp"

#include <QTimer>

QueueProcessor::QueueProcessor(QObject* parent) : QObject(parent) {}

void QueueProcessor::enqueue(Task task) {
  queue_.push(std::move(task));

  if (!running_) {
    processNext();
  }
}

void QueueProcessor::processNext() {
  if (queue_.empty()) {
    running_ = false;
    emit queueDrained();
    return;
  }

  running_ = true;
  auto task = std::move(queue_.front());
  queue_.pop();

  task();

  QTimer::singleShot(0, this, &QueueProcessor::processNext);
}
