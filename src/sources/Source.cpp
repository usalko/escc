#include "Source.hpp"

#include <QEventLoop>
#include <QJsonValue>
#include <QTimer>

#include <stdexcept>

namespace sources {

Source::Source(QString name,
               QJsonObject eeprom,
               QJsonObject settingsDescriptions,
               QJsonObject escs)
    : name_(std::move(name)),
      eeprom_(std::move(eeprom)),
      settings_(std::move(settingsDescriptions)),
      escs_(std::move(escs)) {
  if (name_.isEmpty()) {
    throw std::invalid_argument("Source name must not be empty");
  }
}

void Source::setSkipCache(bool skipCache) {
  skipCache_ = skipCache;
}

bool Source::getRemoteVersionsList(const QUrl& url,
                                   QJsonDocument* outJson,
                                   QString* error,
                                   qint64 maxAgeMs) {
  if (outJson == nullptr) {
    if (error != nullptr) {
      *error = QStringLiteral("Output JSON pointer is null");
    }
    return false;
  }

  QEventLoop loop;
  QTimer timeout;
  timeout.setSingleShot(true);

  bool success = false;
  QString localError;

  QMetaObject::Connection jsonConnection;
  QMetaObject::Connection failConnection;
  QMetaObject::Connection timerConnection;

  jsonConnection = QObject::connect(&httpClient_, &HttpClient::jsonReceived, &loop,
                                    [&](const QByteArray& payload) {
                                      QJsonParseError parseError;
                                      const auto document = QJsonDocument::fromJson(payload, &parseError);
                                      if (parseError.error != QJsonParseError::NoError) {
                                        localError = parseError.errorString();
                                      } else {
                                        *outJson = document;
                                        success = true;
                                      }
                                      loop.quit();
                                    });

  failConnection = QObject::connect(&httpClient_, &HttpClient::requestFailed, &loop,
                                    [&](const QString& requestError) {
                                      localError = requestError;
                                      loop.quit();
                                    });

  timerConnection = QObject::connect(&timeout, &QTimer::timeout, &loop, [&]() {
    localError = QStringLiteral("HTTP request timeout");
    loop.quit();
  });

  timeout.start(15000);
  httpClient_.getJson(url, skipCache_, maxAgeMs);
  loop.exec();

  QObject::disconnect(jsonConnection);
  QObject::disconnect(failConnection);
  QObject::disconnect(timerConnection);

  if (!success && error != nullptr) {
    *error = localError;
  }

  return success;
}

QJsonObject Source::getEscLayouts() const {
  return escs_.value(QStringLiteral("layouts")).toObject();
}

QJsonArray Source::getMcus() const {
  return escs_.value(QStringLiteral("mcus")).toArray();
}

QJsonObject Source::getEeprom() const {
  return eeprom_;
}

QJsonObject Source::getSettingsDescriptions() const {
  return settings_;
}

QStringList Source::getRevisions() const {
  return settings_.value(QStringLiteral("COMMON")).toObject().keys();
}

QJsonObject Source::getCommonSettings(const QString& revision) const {
  const auto common = settings_.value(QStringLiteral("COMMON")).toObject();
  return common.value(revision).toObject();
}

QJsonObject Source::getIndividualSettings(const QString& revision) const {
  const auto individual = settings_.value(QStringLiteral("INDIVIDUAL")).toObject();
  return individual.value(revision).toObject();
}

QJsonObject Source::getDefaultSettings(const QString& revision) const {
  const auto defaults = settings_.value(QStringLiteral("DEFAULTS")).toObject();
  return defaults.value(revision).toObject();
}

QString Source::getName() const {
  return name_;
}

QVector<int> Source::getPwm() const {
  return pwm_;
}

QStringList Source::getValidNames() const {
  QStringList names;
  const auto array = eeprom_.value(QStringLiteral("NAMES")).toArray();
  for (const auto& value : array) {
    names.push_back(value.toString());
  }

  return names;
}

bool Source::isValidName(const QString& name) const {
  return getValidNames().contains(name);
}

bool Source::canMigrateTo(const QString& name) const {
  return isValidName(name);
}

int Source::getLayoutSize() const {
  return eeprom_.value(QStringLiteral("LAYOUT_SIZE")).toInt();
}

QJsonObject Source::getLayout() const {
  return eeprom_.value(QStringLiteral("LAYOUT")).toObject();
}

bool Source::getDisabledLayoutSelection() const {
  return false;
}

bool Source::isValidLayout(const QString& name) const {
  return getEscLayouts().contains(name);
}

QStringList Source::getGroupOrder() const {
  return {QStringLiteral("general")};
}

QStringList Source::getSkipSettings(const QString& oldLayout, const QString& newLayout) const {
  Q_UNUSED(oldLayout);
  Q_UNUSED(newLayout);
  return {};
}

}  // namespace sources
