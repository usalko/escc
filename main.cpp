#include <QApplication>
#include <QFile>

#include "ui/MainWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QFile stylesheetFile(QStringLiteral(":/styles/styles/app.qss"));
  if (stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    app.setStyleSheet(QString::fromUtf8(stylesheetFile.readAll()));
  }

  MainWindow window;
  window.show();

  return app.exec();
}
