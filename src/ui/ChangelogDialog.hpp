#pragma once

#include <QDialog>

class QTreeWidget;

class ChangelogDialog final : public QDialog {
  Q_OBJECT

 public:
  explicit ChangelogDialog(QWidget* parent = nullptr);

 private:
  void loadEntries();

  QTreeWidget* tree_{nullptr};
};
