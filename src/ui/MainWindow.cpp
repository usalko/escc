#include "MainWindow.hpp"

#include "ButtonBarWidget.hpp"
#include "AppSettingsDialog.hpp"
#include "ChangelogDialog.hpp"
#include "FlashWidget.hpp"
#include "FirmwareSelectorDialog.hpp"
#include "HomeWidget.hpp"
#include "i18n/LanguageManager.hpp"
#include "LogWidget.hpp"
#include "MelodyEditorWidget.hpp"
#include "MotorControlWidget.hpp"
#include "OverlayWidget.hpp"
#include "StatusBarWidget.hpp"

#include <QActionGroup>
#include <QMenu>
#include <QMenuBar>
#include <QResizeEvent>
#include <QStatusBar>
#include <QStackedWidget>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    centralContainer_(new QWidget(this)),
    buttonBarWidget_(new ButtonBarWidget(this)),
      stackedWidget_(new QStackedWidget(this)),
      homeWidget_(new HomeWidget(this)),
      flashWidget_(new FlashWidget(this)),
      melodyEditorWidget_(new MelodyEditorWidget(this)),
      motorControlWidget_(new MotorControlWidget(this)),
      logWidget_(new LogWidget(this)),
      overlayWidget_(new OverlayWidget(centralContainer_)),
      statusWidget_(new StatusBarWidget(this)),
      fileMenu_(nullptr),
      viewMenu_(nullptr),
      helpMenu_(nullptr),
      mainToolBar_(nullptr),
      quitAction_(nullptr),
      appSettingsAction_(nullptr),
      changelogAction_(nullptr),
      aboutAction_(nullptr),
      showHomeAction_(new QAction(tr("Home"), this)),
      showFlashAction_(new QAction(tr("Flash"), this)),
      showMelodyEditorAction_(new QAction(tr("Melody Editor"), this)),
      showMotorControlAction_(new QAction(tr("Motor Control"), this)),
      showLogAction_(new QAction(tr("Log"), this)) {
  setWindowTitle(tr("ESC Configurator C++"));
  resize(1000, 700);

  stackedWidget_->addWidget(homeWidget_);
  stackedWidget_->addWidget(flashWidget_);
  stackedWidget_->addWidget(melodyEditorWidget_);
  stackedWidget_->addWidget(motorControlWidget_);
  stackedWidget_->addWidget(logWidget_);

  auto* centralLayout = new QVBoxLayout(centralContainer_);
  centralLayout->setContentsMargins(0, 0, 0, 0);
  centralLayout->setSpacing(0);
  centralLayout->addWidget(buttonBarWidget_);
  centralLayout->addWidget(stackedWidget_, 1);
  setCentralWidget(centralContainer_);

  overlayWidget_->setGeometry(centralContainer_->rect());
  overlayWidget_->hide();

  connect(buttonBarWidget_, &ButtonBarWidget::readSettingsRequested, this, [this]() {
    overlayWidget_->showOverlay(tr("Reading settings"), tr("Fetching ESC settings from connected device..."));
    QTimer::singleShot(700, overlayWidget_, &OverlayWidget::hideOverlay);
    statusBar()->showMessage(tr("Read settings requested"), 2000);
    logWidget_->appendLog(tr("Read settings requested"), LogWidget::LogType::Info);
    statusWidget_->setPortUtilization(15, 10);
  });
  connect(buttonBarWidget_, &ButtonBarWidget::writeSettingsRequested, this, [this]() {
    overlayWidget_->showOverlay(tr("Writing settings"), tr("Applying updated configuration to ESCs..."));
    QTimer::singleShot(700, overlayWidget_, &OverlayWidget::hideOverlay);
    statusBar()->showMessage(tr("Write settings requested"), 2000);
    logWidget_->appendLog(tr("Write settings requested"), LogWidget::LogType::Info);
    statusWidget_->setPortUtilization(20, 18);
  });
  connect(buttonBarWidget_, &ButtonBarWidget::flashAllRequested, this, [this]() {
    overlayWidget_->showOverlay(tr("Flashing firmware"), tr("Starting flash operation for all ESCs..."));
    QTimer::singleShot(900, overlayWidget_, &OverlayWidget::hideOverlay);
    stackedWidget_->setCurrentIndex(1);
    showFlashAction_->setChecked(true);
    statusBar()->showMessage(tr("Flash all requested"), 2000);
    logWidget_->appendLog(tr("Flash all requested"), LogWidget::LogType::Warning);
    statusWidget_->setPortUtilization(35, 28);
  });
  connect(buttonBarWidget_, &ButtonBarWidget::selectFirmwareRequested, this, [this]() {
    FirmwareSelectorDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
      stackedWidget_->setCurrentIndex(1);
      showFlashAction_->setChecked(true);
      statusBar()->showMessage(
          tr("Selected %1 %2 (%3)%4%5")
              .arg(dialog.selectedSource(),
                   dialog.selectedVersion(),
                   dialog.selectedEscVariant(),
                   dialog.forceFlashEnabled() ? tr(", force") : QString(),
                   dialog.migrateEnabled() ? tr(", migrate") : QString()),
          3500);
      logWidget_->appendLog(
          tr("Firmware selected: %1 %2 (%3)")
              .arg(dialog.selectedSource(), dialog.selectedVersion(), dialog.selectedEscVariant()),
          LogWidget::LogType::Info);
        statusWidget_->setFcVersion(tr("%1 %2").arg(dialog.selectedSource(), dialog.selectedVersion()));
      return;
    }

    statusBar()->showMessage(tr("Firmware selection canceled"), 1500);
    logWidget_->appendLog(tr("Firmware selection canceled"), LogWidget::LogType::Warning);
  });

  connect(homeWidget_, &HomeWidget::connectRequested, this, [this]() {
    statusBar()->showMessage(tr("Connect requested"), 2000);
    logWidget_->appendLog(tr("Connect requested"), LogWidget::LogType::Info);
    statusWidget_->setConnectionStatus(true);
    statusWidget_->setPortUtilization(12, 9);
  });
  connect(homeWidget_, &HomeWidget::disconnectRequested, this, [this]() {
    statusBar()->showMessage(tr("Disconnect requested"), 2000);
    logWidget_->appendLog(tr("Disconnect requested"), LogWidget::LogType::Warning);
    statusWidget_->setConnectionStatus(false);
    statusWidget_->setPortUtilization(0, 0);
  });
  connect(homeWidget_, &HomeWidget::readSettingsRequested, this, [this]() {
    statusBar()->showMessage(tr("Read settings requested"), 2000);
    logWidget_->appendLog(tr("Read settings requested"), LogWidget::LogType::Info);
    statusWidget_->setPortUtilization(18, 14);
  });
  connect(homeWidget_, &HomeWidget::flashFirmwareRequested, this, [this]() {
    overlayWidget_->showOverlay(tr("Flashing firmware"), tr("Starting firmware flashing..."));
    QTimer::singleShot(900, overlayWidget_, &OverlayWidget::hideOverlay);
    stackedWidget_->setCurrentIndex(1);
    showFlashAction_->setChecked(true);
    statusBar()->showMessage(tr("Flash firmware requested"), 2000);
    logWidget_->appendLog(tr("Flash firmware requested"), LogWidget::LogType::Warning);
    statusWidget_->setPortUtilization(30, 24);
    statusWidget_->incrementPacketErrors(1);
  });

  fileMenu_ = menuBar()->addMenu(tr("&File"));
  quitAction_ = fileMenu_->addAction(tr("Quit"));
  connect(quitAction_, &QAction::triggered, this, &MainWindow::close);

  viewMenu_ = menuBar()->addMenu(tr("&View"));
  viewMenu_->addAction(showHomeAction_);
  viewMenu_->addAction(showFlashAction_);
  viewMenu_->addAction(showMelodyEditorAction_);
  viewMenu_->addAction(showMotorControlAction_);
  viewMenu_->addAction(showLogAction_);

  helpMenu_ = menuBar()->addMenu(tr("&Help"));
  appSettingsAction_ = helpMenu_->addAction(tr("App Settings"));
  changelogAction_ = helpMenu_->addAction(tr("Changelog"));
  aboutAction_ = helpMenu_->addAction(tr("About"));
  connect(appSettingsAction_, &QAction::triggered, this, &MainWindow::showAppSettingsDialog);
  connect(changelogAction_, &QAction::triggered, this, &MainWindow::showChangelogDialog);
  connect(aboutAction_, &QAction::triggered, this, [this]() {
    statusBar()->showMessage(tr("ESC Configurator C++ migration UI scaffold"), 3000);
  });

  auto* screensGroup = new QActionGroup(this);
  screensGroup->setExclusive(true);
  showHomeAction_->setCheckable(true);
  showFlashAction_->setCheckable(true);
  showMelodyEditorAction_->setCheckable(true);
  showMotorControlAction_->setCheckable(true);
  showLogAction_->setCheckable(true);
  screensGroup->addAction(showHomeAction_);
  screensGroup->addAction(showFlashAction_);
  screensGroup->addAction(showMelodyEditorAction_);
  screensGroup->addAction(showMotorControlAction_);
  screensGroup->addAction(showLogAction_);
  showHomeAction_->setChecked(true);

  mainToolBar_ = addToolBar(tr("Main"));
  mainToolBar_->setMovable(false);
  mainToolBar_->addAction(showHomeAction_);
  mainToolBar_->addAction(showFlashAction_);
  mainToolBar_->addAction(showMelodyEditorAction_);
  mainToolBar_->addAction(showMotorControlAction_);
  mainToolBar_->addAction(showLogAction_);

  connect(showHomeAction_, &QAction::triggered, this, &MainWindow::showHomeScreen);
  connect(showFlashAction_, &QAction::triggered, this, &MainWindow::showFlashScreen);
  connect(showMelodyEditorAction_, &QAction::triggered, this, &MainWindow::showMelodyEditorScreen);
  connect(showMotorControlAction_, &QAction::triggered, this, &MainWindow::showMotorControlScreen);
  connect(showLogAction_, &QAction::triggered, this, &MainWindow::showLogScreen);

  statusBar()->showMessage(tr("Ready"));
  statusBar()->addPermanentWidget(statusWidget_, 1);

  connect(&LanguageManager::instance(), &LanguageManager::languageChanged, this,
          &MainWindow::onLanguageChanged);
}

void MainWindow::showHomeScreen() {
  stackedWidget_->setCurrentIndex(0);
  statusBar()->showMessage(tr("Home screen"), 1500);
}

void MainWindow::showFlashScreen() {
  stackedWidget_->setCurrentIndex(1);
  statusBar()->showMessage(tr("Flash screen"), 1500);
}

void MainWindow::showMelodyEditorScreen() {
  stackedWidget_->setCurrentIndex(2);
  statusBar()->showMessage(tr("Melody editor"), 1500);
}

void MainWindow::showMotorControlScreen() {
  stackedWidget_->setCurrentIndex(3);
  statusBar()->showMessage(tr("Motor control"), 1500);
}

void MainWindow::showLogScreen() {
  stackedWidget_->setCurrentIndex(4);
  statusBar()->showMessage(tr("Log"), 1500);
}

void MainWindow::showChangelogDialog() {
  ChangelogDialog dialog(this);
  dialog.exec();
}

void MainWindow::showAppSettingsDialog() {
  AppSettingsDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted) {
    statusBar()->showMessage(tr("Application settings updated"), 2000);
    logWidget_->appendLog(tr("Application settings updated"), LogWidget::LogType::Info);
  } else {
    statusBar()->showMessage(tr("Application settings canceled"), 1500);
  }
}

void MainWindow::onLanguageChanged() {
  retranslateUiTexts();
  statusBar()->showMessage(tr("Language changed"), 2000);
}

void MainWindow::retranslateUiTexts() {
  setWindowTitle(tr("ESC Configurator C++"));

  if (fileMenu_ != nullptr) {
    fileMenu_->setTitle(tr("&File"));
  }
  if (viewMenu_ != nullptr) {
    viewMenu_->setTitle(tr("&View"));
  }
  if (helpMenu_ != nullptr) {
    helpMenu_->setTitle(tr("&Help"));
  }
  if (mainToolBar_ != nullptr) {
    mainToolBar_->setWindowTitle(tr("Main"));
  }

  if (quitAction_ != nullptr) {
    quitAction_->setText(tr("Quit"));
  }
  if (appSettingsAction_ != nullptr) {
    appSettingsAction_->setText(tr("App Settings"));
  }
  if (changelogAction_ != nullptr) {
    changelogAction_->setText(tr("Changelog"));
  }
  if (aboutAction_ != nullptr) {
    aboutAction_->setText(tr("About"));
  }

  showHomeAction_->setText(tr("Home"));
  showFlashAction_->setText(tr("Flash"));
  showMelodyEditorAction_->setText(tr("Melody Editor"));
  showMotorControlAction_->setText(tr("Motor Control"));
  showLogAction_->setText(tr("Log"));
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  QMainWindow::resizeEvent(event);

  if (overlayWidget_ != nullptr && centralContainer_ != nullptr) {
    overlayWidget_->setGeometry(centralContainer_->rect());
    overlayWidget_->raise();
  }
}
