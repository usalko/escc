#include "LogWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

LogWidget::LogWidget(QWidget* parent) : QWidget(parent) {
  setObjectName(QStringLiteral("logWidget"));

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(16, 16, 16, 16);
  root->setSpacing(10);

  auto* title = new QLabel(tr("Log"), this);
  title->setObjectName(QStringLiteral("homeTitle"));

  auto* toolbar = new QHBoxLayout();
  toolbar->setSpacing(12);
  auto* filterLabel = new QLabel(tr("Filter"), this);
  filterCombo_ = new QComboBox(this);
  filterCombo_->addItem(tr("All"));
  filterCombo_->addItem(tr("Info"));
  filterCombo_->addItem(tr("Warning"));
  filterCombo_->addItem(tr("Error"));

  autoScrollCheck_ = new QCheckBox(tr("Auto-scroll"), this);
  autoScrollCheck_->setChecked(true);

  clearButton_ = new QPushButton(tr("Clear"), this);

  toolbar->addWidget(filterLabel);
  toolbar->addWidget(filterCombo_);
  toolbar->addWidget(autoScrollCheck_);
  toolbar->addStretch(1);
  toolbar->addWidget(clearButton_);

  listWidget_ = new QListWidget(this);
  listWidget_->setSelectionMode(QAbstractItemView::NoSelection);

  root->addWidget(title);
  root->addLayout(toolbar);
  root->addWidget(listWidget_, 1);

  connect(filterCombo_, &QComboBox::currentIndexChanged, this, &LogWidget::onFilterChanged);
  connect(clearButton_, &QPushButton::clicked, this, &LogWidget::clearLogs);

  appendLog(tr("Log initialized"), LogType::Info);
}

void LogWidget::appendLog(const QString& message, LogType type) {
  entries_.push_back({QDateTime::currentDateTime(), message, type});

  if (!passesCurrentFilter(type)) {
    return;
  }

  const LogEntry& entry = entries_.back();
  const QString line =
      QStringLiteral("%1 [%2] %3")
          .arg(entry.timestamp.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")), typeToText(entry.type),
               entry.message);

  auto* item = new QListWidgetItem(line);

  listWidget_->addItem(item);

  if (autoScrollCheck_->isChecked()) {
    listWidget_->scrollToBottom();
  }
}

void LogWidget::clearLogs() {
  entries_.clear();
  listWidget_->clear();
}

void LogWidget::onFilterChanged() { refreshVisibleLog(); }

QString LogWidget::typeToText(LogType type) {
  switch (type) {
    case LogType::Info:
      return QStringLiteral("INFO");
    case LogType::Warning:
      return QStringLiteral("WARN");
    case LogType::Error:
      return QStringLiteral("ERROR");
  }

  return QStringLiteral("INFO");
}

bool LogWidget::passesCurrentFilter(LogType type) const {
  switch (filterCombo_->currentIndex()) {
    case 0:
      return true;
    case 1:
      return type == LogType::Info;
    case 2:
      return type == LogType::Warning;
    case 3:
      return type == LogType::Error;
    default:
      return true;
  }
}

void LogWidget::refreshVisibleLog() {
  listWidget_->clear();

  for (const auto& entry : entries_) {
    if (!passesCurrentFilter(entry.type)) {
      continue;
    }

    const QString line =
        QStringLiteral("%1 [%2] %3")
            .arg(entry.timestamp.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")), typeToText(entry.type),
                 entry.message);

    auto* item = new QListWidgetItem(line);
    listWidget_->addItem(item);
  }

  if (autoScrollCheck_->isChecked()) {
    listWidget_->scrollToBottom();
  }
}
