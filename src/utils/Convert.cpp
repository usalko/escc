#include "Convert.hpp"

#include <QVariantList>

namespace convert {

QVariantMap arrayToSettingsObject(const QByteArray& settings, const Layout& layout) {
  QVariantMap object;

  for (auto it = layout.cbegin(); it != layout.cend(); ++it) {
    const QString& prop = it.key();
    const LayoutEntry& entry = it.value();

    if (entry.size == 1) {
      object.insert(prop, static_cast<quint8>(settings.at(entry.offset)));
      continue;
    }

    if (entry.size == 2) {
      const auto hi = static_cast<quint8>(settings.at(entry.offset));
      const auto lo = static_cast<quint8>(settings.at(entry.offset + 1));
      object.insert(prop, static_cast<quint16>((hi << 8) | lo));
      continue;
    }

    if (entry.size > 2) {
      const QByteArray raw = settings.mid(entry.offset, entry.size);
      if (prop == QStringLiteral("STARTUP_MELODY")) {
        QVariantList melody;
        melody.reserve(raw.size());
        for (const auto byte : raw) {
          melody.push_back(static_cast<quint8>(byte));
        }
        object.insert(prop, melody);
      } else {
        object.insert(prop, QString::fromLatin1(raw).trimmed());
      }
    }
  }

  return object;
}

QByteArray objectToSettingsArray(const QVariantMap& settings,
                                 const Layout& layout,
                                 int layoutSize) {
  QByteArray array(layoutSize, static_cast<char>(0xFF));

  for (auto it = layout.cbegin(); it != layout.cend(); ++it) {
    const QString& prop = it.key();
    const LayoutEntry& entry = it.value();
    const QVariant value = settings.value(prop);

    if (entry.size == 1) {
      array[entry.offset] = static_cast<char>(value.toUInt() & 0xFF);
      continue;
    }

    if (entry.size == 2) {
      const auto num = value.toUInt();
      array[entry.offset] = static_cast<char>((num >> 8) & 0xFF);
      array[entry.offset + 1] = static_cast<char>(num & 0xFF);
      continue;
    }

    if (entry.size > 2) {
      if (prop == QStringLiteral("STARTUP_MELODY")) {
        const QVariantList melody = value.toList();
        for (int i = 0; i < entry.size; ++i) {
          array[entry.offset + i] =
              i < melody.size() ? static_cast<char>(melody.at(i).toUInt() % 256) : 0;
        }
      } else {
        const QString text = value.toString();
        for (int i = 0; i < entry.size; ++i) {
          array[entry.offset + i] =
              i < text.size() ? static_cast<char>(text.at(i).unicode() & 0xFF) : ' ';
        }
      }
    }
  }

  return array;
}

QString bufferToAscii(const QByteArray& buffer) {
  return QString::fromLatin1(buffer);
}

QByteArray asciiToBuffer(const QString& ascii) {
  return ascii.toLatin1();
}

}  // namespace convert
