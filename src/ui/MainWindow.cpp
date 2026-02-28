#include "MainWindow.hpp"

#include "ButtonBarWidget.hpp"
#include "FlashWidget.hpp"
#include "HomeWidget.hpp"

#include <QActionGroup>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QStackedWidget>
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
      statusLabel_(new QLabel(tr("Disconnected"), this)),
      showHomeAction_(new QAction(tr("Home"), this)),
      showFlashAction_(new QAction(tr("Flash"), this)) {
  setWindowTitle(tr("ESC Configurator C++"));
  resize(1000, 700);

  stackedWidget_->addWidget(homeWidget_);
  stackedWidget_->addWidget(flashWidget_);

  auto* centralLayout = new QVBoxLayout(centralContainer_);
  centralLayout->setContentsMargins(0, 0, 0, 0);
  centralLayout->setSpacing(0);
  centralLayout->addWidget(buttonBarWidget_);
  centralLayout->addWidget(stackedWidget_, 1);
  setCentralWidget(centralContainer_);

  connect(buttonBarWidget_, &ButtonBarWidget::readSettingsRequested, this, [this]() {
    statusBar()->showMessage(tr("Read settings requested"), 2000);
  });
  connect(buttonBarWidget_, &ButtonBarWidget::writeSettingsRequested, this, [this]() {
    statusBar()->showMessage(tr("Write settings requested"), 2000);
  });
  connect(buttonBarWidget_, &ButtonBarWidget::flashAllRequested, this, [this]() {
    stackedWidget_->setCurrentIndex(1);
    showFlashAction_->setChecked(true);
    statusBar()->showMessage(tr("Flash all requested"), 2000);
  });
  connect(buttonBarWidget_, &ButtonBarWidget::selectFirmwareRequested, this, [this]() {
    stackedWidget_->setCurrentIndex(1);
    showFlashAction_->setChecked(true);
    statusBar()->showMessage(tr("Select firmware requested"), 2000);
  });

  connect(homeWidget_, &HomeWidget::connectRequested, this, [this]() {
    statusBar()->showMessage(tr("Connect requested"), 2000);
  });
  connect(homeWidget_, &HomeWidget::disconnectRequested, this, [this]() {
    statusBar()->showMessage(tr("Disconnect requested"), 2000);
  });
  connect(homeWidget_, &HomeWidget::readSettingsRequested, this, [this]() {
    statusBar()->showMessage(tr("Read settings requested"), 2000);
  });
  connect(homeWidget_, &HomeWidget::flashFirmwareRequested, this, [this]() {
    stackedWidget_->setCurrentIndex(1);
    showFlashAction_->setChecked(true);
    statusBar()->showMessage(tr("Flash firmware requested"), 2000);
  });

  auto* fileMenu = menuBar()->addMenu(tr("&File"));
  auto* quitAction = fileMenu->addAction(tr("Quit"));
  connect(quitAction, &QAction::triggered, this, &MainWindow::close);

  auto* viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(showHomeAction_);
  viewMenu->addAction(showFlashAction_);

  auto* helpMenu = menuBar()->addMenu(tr("&Help"));
  auto* aboutAction = helpMenu->addAction(tr("About"));
  connect(aboutAction, &QAction::triggered, this, [this]() {
    statusBar()->showMessage(tr("ESC Configurator C++ migration UI scaffold"), 3000);
  });

  auto* screensGroup = new QActionGroup(this);
  screensGroup->setExclusive(true);
  showHomeAction_->setCheckable(true);
  showFlashAction_->setCheckable(true);
  screensGroup->addAction(showHomeAction_);
  screensGroup->addAction(showFlashAction_);
  showHomeAction_->setChecked(true);

  auto* toolbar = addToolBar(tr("Main"));
  toolbar->setMovable(false);
  toolbar->addAction(showHomeAction_);
  toolbar->addAction(showFlashAction_);

  connect(showHomeAction_, &QAction::triggered, this, &MainWindow::showHomeScreen);
  connect(showFlashAction_, &QAction::triggered, this, &MainWindow::showFlashScreen);

  statusBar()->showMessage(tr("Ready"));
  statusBar()->addPermanentWidget(statusLabel_);
}

void MainWindow::showHomeScreen() {
  stackedWidget_->setCurrentIndex(0);
  statusBar()->showMessage(tr("Home screen"), 1500);
}

void MainWindow::showFlashScreen() {
  stackedWidget_->setCurrentIndex(1);
  statusBar()->showMessage(tr("Flash screen"), 1500);
}
