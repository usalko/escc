#include <QApplication>
#include <QFile>

#include "i18n/LanguageManager.hpp"
#include "ui/MainWindow.hpp"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  QApplication::setOrganizationName(QStringLiteral("stylesuxx"));
  QApplication::setApplicationName(QStringLiteral("escc"));

  QFile stylesheetFile(QStringLiteral(":/styles/styles/app.qss"));
  if (stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    app.setStyleSheet(QString::fromUtf8(stylesheetFile.readAll()));
  }

  LanguageManager::instance();

  MainWindow window;
  window.show();

  return app.exec();
}
