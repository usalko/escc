#pragma once

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;

class HttpClient final : public QObject {
  Q_OBJECT

 public:
  static constexpr qint64 kOneDayMs = 24LL * 60LL * 60LL * 1000LL;
  static constexpr qint64 kOneYearMs = 365LL * kOneDayMs;

  explicit HttpClient(QObject* parent = nullptr);

  void get(const QUrl& url, bool skipCache = false, qint64 maxAgeMs = kOneDayMs);
  void getJson(const QUrl& url, bool skipCache = false, qint64 maxAgeMs = kOneDayMs);
  void getHex(const QUrl& url, qint64 maxAgeMs = kOneYearMs);

  void enableDiskCache(const QString& directory, qint64 maxBytes = 50LL * 1024LL * 1024LL);
  void disableDiskCache();
  bool hasDiskCache() const;
  QString diskCacheDirectory() const;

  void clearCache();
  int cacheSize() const;

 signals:
  void dataReceived(const QUrl& url, const QByteArray& data, bool fromCache);
  void jsonReceived(const QByteArray& data);
  void requestFailed(const QString& error);

 private:
  struct CacheEntry {
    QByteArray data;
    qint64 timestampMs{0};
  };

  void fetch(const QUrl& url, bool skipCache, qint64 maxAgeMs, bool expectJson);
  bool isCacheValid(const QUrl& url, qint64 maxAgeMs) const;

  QNetworkAccessManager* networkManager_;
  QHash<QString, CacheEntry> cache_;
};
