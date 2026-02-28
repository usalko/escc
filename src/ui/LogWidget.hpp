#pragma once

#include <QDateTime>
#include <QVector>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QListWidget;
class QPushButton;

class LogWidget final : public QWidget {
  Q_OBJECT

 public:
  enum class LogType {
    Info,
    Warning,
    Error,
  };

  explicit LogWidget(QWidget* parent = nullptr);

  void appendLog(const QString& message, LogType type = LogType::Info);
  void clearLogs();

 private slots:
  void onFilterChanged();

 private:
  struct LogEntry {
    QDateTime timestamp;
    QString message;
    LogType type;
  };

  static QString typeToText(LogType type);
  bool passesCurrentFilter(LogType type) const;
  void refreshVisibleLog();

  QComboBox* filterCombo_{nullptr};
  QCheckBox* autoScrollCheck_{nullptr};
  QPushButton* clearButton_{nullptr};
  QListWidget* listWidget_{nullptr};

  QVector<LogEntry> entries_;
};
