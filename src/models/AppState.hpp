#pragma once

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QString>
#include <QStringList>

class AppState final : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
  Q_PROPERTY(QVariantList escs READ escs WRITE setEscs NOTIFY escsChanged)
  Q_PROPERTY(QVariantMap settings READ settings WRITE setSettings NOTIFY settingsChanged)
  Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
  Q_PROPERTY(QStringList log READ log WRITE setLog NOTIFY logChanged)
  Q_PROPERTY(QStringList melodies READ melodies WRITE setMelodies NOTIFY melodiesChanged)
  Q_PROPERTY(bool cookiesAccepted READ cookiesAccepted WRITE setCookiesAccepted NOTIFY cookiesChanged)

 public:
  explicit AppState(QObject* parent = nullptr);

  bool connected() const;
  void setConnected(bool value);

  QVariantList escs() const;
  void setEscs(const QVariantList& value);

  QVariantMap settings() const;
  void setSettings(const QVariantMap& value);

  QString language() const;
  void setLanguage(const QString& value);

  QStringList log() const;
  void setLog(const QStringList& value);
  void appendLog(const QString& message);
  void clearLog();

  QStringList melodies() const;
  void setMelodies(const QStringList& value);

  bool cookiesAccepted() const;
  void setCookiesAccepted(bool value);

 signals:
  void connectedChanged();
  void escsChanged();
  void settingsChanged();
  void languageChanged();
  void logChanged();
  void melodiesChanged();
  void cookiesChanged();
  void stateChanged(const QString& section);

 private:
  bool connected_{false};
  QVariantList escs_;
  QVariantMap settings_;
  QString language_{QStringLiteral("en")};
  QStringList log_;
  QStringList melodies_;
  bool cookiesAccepted_{false};
};
