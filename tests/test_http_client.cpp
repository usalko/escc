#include <QObject>
#include <QDir>
#include <QTemporaryDir>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "network/HttpClient.hpp"

class TestHttpServer final : public QObject {
  Q_OBJECT

 public:
  bool start() {
    const bool ok = server_.listen(QHostAddress::LocalHost);
    if (!ok) {
      return false;
    }

    connect(&server_, &QTcpServer::newConnection, this, [this]() {
      while (server_.hasPendingConnections()) {
        auto* socket = server_.nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
          const QByteArray request = socket->readAll();
          const QList<QByteArray> lines = request.split('\n');
          if (lines.isEmpty()) {
            return;
          }

          const QByteArray firstLine = lines.first().trimmed();
          const QList<QByteArray> parts = firstLine.split(' ');
          if (parts.size() < 2) {
            return;
          }

          const QString path = QString::fromLatin1(parts.at(1));
          requestCount_[path] = requestCount_.value(path) + 1;

          QByteArray body;
          QByteArray contentType = "text/plain";
          if (path == QStringLiteral("/json")) {
            body = "{\"ok\":true}";
            contentType = "application/json";
          } else if (path == QStringLiteral("/badjson")) {
            body = "not-json";
            contentType = "application/json";
          } else if (path == QStringLiteral("/hex")) {
            body = ":00000001FF\n";
            contentType = "text/plain";
          } else {
            body = "unknown";
          }

          QByteArray response;
          response.append("HTTP/1.1 200 OK\r\n");
          response.append("Content-Type: " + contentType + "\r\n");
          response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
          response.append("Connection: close\r\n\r\n");
          response.append(body);

          socket->write(response);
          socket->flush();
          socket->disconnectFromHost();
        });
      }
    });

    return true;
  }

  QUrl url(const QString& path) const {
    return QUrl(QStringLiteral("http://127.0.0.1:%1%2").arg(server_.serverPort()).arg(path));
  }

  int count(const QString& path) const {
    return requestCount_.value(path);
  }

 private:
  QTcpServer server_;
  QHash<QString, int> requestCount_;
};

class HttpClientTests final : public QObject {
  Q_OBJECT

 private slots:
  void getHex_usesCacheOnSecondRequest();
  void getJson_validatesAndCaches();
  void getJson_invalidPayloadFails();
  void diskCache_canBeEnabledAndDisabled();
};

void HttpClientTests::getHex_usesCacheOnSecondRequest() {
  TestHttpServer server;
  QVERIFY(server.start());

  HttpClient client;
  QSignalSpy dataSpy(&client, SIGNAL(dataReceived(QUrl,QByteArray,bool)));

  const QUrl url = server.url(QStringLiteral("/hex"));
  client.getHex(url);
  QTRY_COMPARE(dataSpy.count(), 1);

  client.getHex(url);
  QTRY_COMPARE(dataSpy.count(), 2);

  QCOMPARE(server.count(QStringLiteral("/hex")), 1);
  QCOMPARE(dataSpy.at(0).at(2).toBool(), false);
  QCOMPARE(dataSpy.at(1).at(2).toBool(), true);
  QCOMPARE(client.cacheSize(), 1);
}

void HttpClientTests::getJson_validatesAndCaches() {
  TestHttpServer server;
  QVERIFY(server.start());

  HttpClient client;
  QSignalSpy jsonSpy(&client, SIGNAL(jsonReceived(QByteArray)));

  const QUrl url = server.url(QStringLiteral("/json"));
  client.getJson(url);
  QTRY_COMPARE(jsonSpy.count(), 1);

  client.getJson(url);
  QTRY_COMPARE(jsonSpy.count(), 2);

  QCOMPARE(server.count(QStringLiteral("/json")), 1);
}

void HttpClientTests::getJson_invalidPayloadFails() {
  TestHttpServer server;
  QVERIFY(server.start());

  HttpClient client;
  QSignalSpy errorSpy(&client, SIGNAL(requestFailed(QString)));

  client.getJson(server.url(QStringLiteral("/badjson")));
  QTRY_COMPARE(errorSpy.count(), 1);
}

void HttpClientTests::diskCache_canBeEnabledAndDisabled() {
  HttpClient client;
  QTemporaryDir tempDir;
  QVERIFY(tempDir.isValid());

  client.enableDiskCache(tempDir.path(), 2 * 1024 * 1024);
  QVERIFY(client.hasDiskCache());
  const QString actual = QDir::cleanPath(client.diskCacheDirectory());
  const QString expected = QDir::cleanPath(tempDir.path());
  QCOMPARE(actual, expected);

  client.disableDiskCache();
  QVERIFY(!client.hasDiskCache());
  QVERIFY(client.diskCacheDirectory().isEmpty());
}

QTEST_MAIN(HttpClientTests)
#include "test_http_client.moc"
