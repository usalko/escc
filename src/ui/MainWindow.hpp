#pragma once

#include <QAction>
#include <QMainWindow>
#include <QString>

class QLabel;
class QStackedWidget;
class HomeWidget;
class ButtonBarWidget;
class FlashWidget;

class MainWindow final : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private slots:
  void showHomeScreen();
  void showFlashScreen();

 private:
  QWidget* centralContainer_;
  ButtonBarWidget* buttonBarWidget_;
  QStackedWidget* stackedWidget_;
  HomeWidget* homeWidget_;
  FlashWidget* flashWidget_;
  QLabel* statusLabel_;
  QAction* showHomeAction_;
  QAction* showFlashAction_;
};
