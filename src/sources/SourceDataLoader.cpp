#include "SourceDataLoader.hpp"

#include <QFile>
#include <QJsonParseError>

namespace sources {

bool SourceDataLoader::loadJsonDocument(const QString& filePath,
                                        QJsonDocument* outDocument,
                                        QString* error) {
  if (outDocument == nullptr) {
    if (error != nullptr) {
      *error = QStringLiteral("Output document pointer is null");
    }
    return false;
  }

  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    if (error != nullptr) {
      *error = QStringLiteral("Cannot open file: %1").arg(filePath);
    }
    return false;
  }

  const QByteArray payload = file.readAll();
  QJsonParseError parseError;
  const QJsonDocument document = QJsonDocument::fromJson(payload, &parseError);
  if (parseError.error != QJsonParseError::NoError) {
    if (error != nullptr) {
      *error = parseError.errorString();
    }
    return false;
  }

  *outDocument = document;
  return true;
}

bool SourceDataLoader::loadJsonObject(const QString& filePath,
                                      QJsonObject* outObject,
                                      QString* error) {
  if (outObject == nullptr) {
    if (error != nullptr) {
      *error = QStringLiteral("Output object pointer is null");
    }
    return false;
  }

  QJsonDocument document;
  if (!loadJsonDocument(filePath, &document, error)) {
    return false;
  }

  if (!document.isObject()) {
    if (error != nullptr) {
      *error = QStringLiteral("JSON document is not an object");
    }
    return false;
  }

  *outObject = document.object();
  return true;
}

}  // namespace sources
