#include "AppState.hpp"

AppState::AppState(QObject* parent) : QObject(parent) {}

bool AppState::connected() const { return connected_; }

void AppState::setConnected(bool value) {
  if (connected_ == value) {
    return;
  }

  connected_ = value;
  emit connectedChanged();
  emit stateChanged(QStringLiteral("connected"));
}

QVariantList AppState::escs() const { return escs_; }

void AppState::setEscs(const QVariantList& value) {
  if (escs_ == value) {
    return;
  }

  escs_ = value;
  emit escsChanged();
  emit stateChanged(QStringLiteral("escs"));
}

QVariantMap AppState::settings() const { return settings_; }

void AppState::setSettings(const QVariantMap& value) {
  if (settings_ == value) {
    return;
  }

  settings_ = value;
  emit settingsChanged();
  emit stateChanged(QStringLiteral("settings"));
}

QString AppState::language() const { return language_; }

void AppState::setLanguage(const QString& value) {
  if (language_ == value) {
    return;
  }

  language_ = value;
  emit languageChanged();
  emit stateChanged(QStringLiteral("language"));
}

QStringList AppState::log() const { return log_; }

void AppState::setLog(const QStringList& value) {
  if (log_ == value) {
    return;
  }

  log_ = value;
  emit logChanged();
  emit stateChanged(QStringLiteral("log"));
}

void AppState::appendLog(const QString& message) {
  if (message.isEmpty()) {
    return;
  }

  log_.push_back(message);
  emit logChanged();
  emit stateChanged(QStringLiteral("log"));
}

void AppState::clearLog() {
  if (log_.isEmpty()) {
    return;
  }

  log_.clear();
  emit logChanged();
  emit stateChanged(QStringLiteral("log"));
}

QStringList AppState::melodies() const { return melodies_; }

void AppState::setMelodies(const QStringList& value) {
  if (melodies_ == value) {
    return;
  }

  melodies_ = value;
  emit melodiesChanged();
  emit stateChanged(QStringLiteral("melodies"));
}

bool AppState::cookiesAccepted() const { return cookiesAccepted_; }

void AppState::setCookiesAccepted(bool value) {
  if (cookiesAccepted_ == value) {
    return;
  }

  cookiesAccepted_ = value;
  emit cookiesChanged();
  emit stateChanged(QStringLiteral("cookies"));
}
