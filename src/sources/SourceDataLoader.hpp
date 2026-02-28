#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

namespace sources {

class SourceDataLoader {
 public:
  static bool loadJsonDocument(const QString& filePath,
                               QJsonDocument* outDocument,
                               QString* error = nullptr);

  static bool loadJsonObject(const QString& filePath,
                             QJsonObject* outObject,
                             QString* error = nullptr);
};

}  // namespace sources
