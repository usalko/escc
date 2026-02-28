#pragma once

#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace models {

struct FirmwareInfo {
  QString name;
  QString version;
  QString platform;
  QString source;
  QString downloadUrl;

  [[nodiscard]] bool isValid() const {
    return !name.trimmed().isEmpty() && !version.trimmed().isEmpty() &&
           !platform.trimmed().isEmpty();
  }

};

struct FlightControllerInfo {
  QString apiVersion;
  QString variant;
  QString version;
  QString boardName;
  QString boardId;
  bool supports4Way{false};

  [[nodiscard]] bool isValid() const {
    return !variant.trimmed().isEmpty() && !version.trimmed().isEmpty();
  }

};

struct MotorState {
  int index{-1};
  int speed{0};
  bool reversed{false};
  bool enabled{false};

  [[nodiscard]] bool isValid() const { return index >= 0 && speed >= 0; }

};

struct EscConfig {
  int index{-1};
  QString name;
  QString firmwareName;
  QString firmwareVersion;
  QString layout;
  bool selected{true};
  QVariantMap settings;

  [[nodiscard]] bool isValid() const {
    return index >= 0 && !firmwareName.trimmed().isEmpty() &&
           !firmwareVersion.trimmed().isEmpty();
  }

};

}  // namespace models
