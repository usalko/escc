#pragma once

#include <QAction>
#include <QMainWindow>
#include <QString>

class QResizeEvent;
class QStackedWidget;
class HomeWidget;
class ButtonBarWidget;
class FlashWidget;
class MelodyEditorWidget;
class MotorControlWidget;
class LogWidget;
class OverlayWidget;
class StatusBarWidget;

class MainWindow final : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);

 protected:
  void resizeEvent(QResizeEvent* event) override;

 private slots:
  void showHomeScreen();
  void showFlashScreen();
  void showMelodyEditorScreen();
  void showMotorControlScreen();
  void showLogScreen();
  void showChangelogDialog();
  void showAppSettingsDialog();

 private:
  QWidget* centralContainer_;
  ButtonBarWidget* buttonBarWidget_;
  QStackedWidget* stackedWidget_;
  HomeWidget* homeWidget_;
  FlashWidget* flashWidget_;
  MelodyEditorWidget* melodyEditorWidget_;
  MotorControlWidget* motorControlWidget_;
  LogWidget* logWidget_;
  OverlayWidget* overlayWidget_;
  StatusBarWidget* statusWidget_;
  QAction* showHomeAction_;
  QAction* showFlashAction_;
  QAction* showMelodyEditorAction_;
  QAction* showMotorControlAction_;
  QAction* showLogAction_;
};
