#pragma once

#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVector>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "network/HttpClient.hpp"

namespace sources {

class Source {
 public:
  Source(QString name, QJsonObject eeprom, QJsonObject settingsDescriptions, QJsonObject escs);
  virtual ~Source() = default;

  void setSkipCache(bool skipCache);

  bool getRemoteVersionsList(const QUrl& url,
                             QJsonDocument* outJson,
                             QString* error = nullptr,
                             qint64 maxAgeMs = HttpClient::kOneDayMs);

  virtual QString buildDisplayName(const QJsonObject& firmware) const = 0;
  virtual QJsonArray getVersions(QString* error = nullptr) = 0;

  QJsonObject getEscLayouts() const;
  virtual QJsonArray getMcus() const;
  QJsonObject getEeprom() const;
  QJsonObject getSettingsDescriptions() const;

  QStringList getRevisions() const;
  QJsonObject getCommonSettings(const QString& revision) const;
  QJsonObject getIndividualSettings(const QString& revision) const;
  QJsonObject getDefaultSettings(const QString& revision) const;

  QString getName() const;
  virtual QVector<int> getPwm() const;
  virtual QStringList getValidNames() const;
  virtual bool isValidName(const QString& name) const;
  virtual bool canMigrateTo(const QString& name) const;

  int getLayoutSize() const;
  QJsonObject getLayout() const;
  virtual bool getDisabledLayoutSelection() const;
  bool isValidLayout(const QString& name) const;

  virtual QStringList getGroupOrder() const;
  virtual QStringList getSkipSettings(const QString& oldLayout, const QString& newLayout) const;

 protected:
  QString name_;
  QJsonObject eeprom_;
  QJsonObject settings_;
  QJsonObject escs_;
  QVector<int> pwm_;
  bool skipCache_{false};
  HttpClient httpClient_;
};

}  // namespace sources
