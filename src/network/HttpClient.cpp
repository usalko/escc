#include "HttpClient.hpp"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

HttpClient::HttpClient(QObject* parent)
    : QObject(parent), networkManager_(new QNetworkAccessManager(this)) {}

void HttpClient::get(const QUrl& url, bool skipCache, qint64 maxAgeMs) {
  fetch(url, skipCache, maxAgeMs, false);
}

void HttpClient::getJson(const QUrl& url, bool skipCache, qint64 maxAgeMs) {
  fetch(url, skipCache, maxAgeMs, true);
}

void HttpClient::getHex(const QUrl& url, qint64 maxAgeMs) {
  fetch(url, false, maxAgeMs, false);
}

void HttpClient::enableDiskCache(const QString& directory, qint64 maxBytes) {
  if (directory.isEmpty()) {
    return;
  }

  auto* diskCache = qobject_cast<QNetworkDiskCache*>(networkManager_->cache());
  if (diskCache == nullptr) {
    diskCache = new QNetworkDiskCache(networkManager_);
    networkManager_->setCache(diskCache);
  }

  diskCache->setCacheDirectory(directory);
  if (maxBytes > 0) {
    diskCache->setMaximumCacheSize(maxBytes);
  }
}

void HttpClient::disableDiskCache() {
  auto* diskCache = qobject_cast<QNetworkDiskCache*>(networkManager_->cache());
  if (diskCache != nullptr) {
    diskCache->clear();
    networkManager_->setCache(nullptr);
  }
}

bool HttpClient::hasDiskCache() const {
  return qobject_cast<QNetworkDiskCache*>(networkManager_->cache()) != nullptr;
}

QString HttpClient::diskCacheDirectory() const {
  const auto* diskCache = qobject_cast<QNetworkDiskCache*>(networkManager_->cache());
  if (diskCache == nullptr) {
    return {};
  }

  return diskCache->cacheDirectory();
}

void HttpClient::clearCache() {
  cache_.clear();
  auto* diskCache = qobject_cast<QNetworkDiskCache*>(networkManager_->cache());
  if (diskCache != nullptr) {
    diskCache->clear();
  }
}

int HttpClient::cacheSize() const {
  return cache_.size();
}

bool HttpClient::isCacheValid(const QUrl& url, qint64 maxAgeMs) const {
  const auto key = url.toString();
  if (!cache_.contains(key)) {
    return false;
  }

  const auto& entry = cache_.value(key);
  const qint64 age = QDateTime::currentMSecsSinceEpoch() - entry.timestampMs;
  return age <= maxAgeMs;
}

void HttpClient::fetch(const QUrl& url, bool skipCache, qint64 maxAgeMs, bool expectJson) {
  const auto key = url.toString();
  if (!skipCache && isCacheValid(url, maxAgeMs)) {
    const auto data = cache_.value(key).data;
    if (expectJson) {
      QJsonParseError parseError;
      QJsonDocument::fromJson(data, &parseError);
      if (parseError.error != QJsonParseError::NoError) {
        emit requestFailed(parseError.errorString());
        return;
      }
      emit jsonReceived(data);
    }

    emit dataReceived(url, data, true);
    return;
  }

  auto* reply = networkManager_->get(QNetworkRequest(url));

  connect(reply, &QNetworkReply::finished, this, [this, reply, url, key, expectJson]() {
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
      emit requestFailed(reply->errorString());
      return;
    }

    const QByteArray payload = reply->readAll();

    if (expectJson) {
      QJsonParseError parseError;
      QJsonDocument::fromJson(payload, &parseError);
      if (parseError.error != QJsonParseError::NoError) {
        emit requestFailed(parseError.errorString());
        return;
      }

      emit jsonReceived(payload);
    }

    cache_.insert(key, CacheEntry{payload, QDateTime::currentMSecsSinceEpoch()});
    emit dataReceived(url, payload, false);
  });
}
