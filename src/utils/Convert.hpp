#pragma once

#include <QByteArray>
#include <QMap>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace convert {

struct LayoutEntry {
  int size{0};
  int offset{0};
};

using Layout = QMap<QString, LayoutEntry>;

QVariantMap arrayToSettingsObject(const QByteArray& settings, const Layout& layout);
QByteArray objectToSettingsArray(const QVariantMap& settings,
                                 const Layout& layout,
                                 int layoutSize);

QString bufferToAscii(const QByteArray& buffer);
QByteArray asciiToBuffer(const QString& ascii);

}  // namespace convert
