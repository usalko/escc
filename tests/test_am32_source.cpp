#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "sources/AM32Source.hpp"

class Am32ApiServer final : public QObject {
  Q_OBJECT

 public:
  void setBody(QByteArray body) { body_ = std::move(body); }

  bool start() {
    if (!server_.listen(QHostAddress::LocalHost)) {
      return false;
    }

    connect(&server_, &QTcpServer::newConnection, this, [this]() {
      auto* socket = server_.nextPendingConnection();
      connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
        socket->readAll();
        const QByteArray body = body_;

        QByteArray response;
        response.append("HTTP/1.1 200 OK\r\n");
        response.append("Content-Type: application/json\r\n");
        response.append("Content-Length: " + QByteArray::number(body.size()) + "\r\n");
        response.append("Connection: close\r\n\r\n");
        response.append(body);

        socket->write(response);
        socket->flush();
        socket->disconnectFromHost();
      });
    });

    return true;
  }

  QString apiBaseUrl() const { return QStringLiteral("http://127.0.0.1:%1/repos").arg(server_.serverPort()); }
  QString webBaseUrl() const { return QStringLiteral("http://127.0.0.1:%1").arg(server_.serverPort()); }

 private:
  QByteArray body_ =
      "["
      "{\"name\":\"\",\"tag_name\":\"v1.93\",\"assets\":[{\"name\":\"a.hex\"}],\"prerelease\":false,\"published_at\":\"2026-01-01T00:00:00Z\"},"
      "{\"name\":\"\",\"tag_name\":\"v1.94\",\"assets\":[{\"name\":\"b.hex\"}],\"prerelease\":false,\"published_at\":\"2026-01-02T00:00:00Z\"}"
      "]";
  QTcpServer server_;
};

class AM32SourceTests final : public QObject {
  Q_OBJECT

 private slots:
  void getVersions_filtersByMinimumVersion();
  void getVersions_ignoresMalformedTags();
  void getFirmwareUrl_usesEscLayoutFilename();
};

void AM32SourceTests::getVersions_filtersByMinimumVersion() {
  Am32ApiServer server;
  QVERIFY(server.start());

  QJsonObject escs{{QStringLiteral("layouts"),
                    QJsonObject{{QStringLiteral("ESC1"), QJsonObject{{QStringLiteral("fileName"), QStringLiteral("F4")}}}}}};
  sources::AM32Source source(QJsonObject{}, QJsonObject{}, escs);
  source.setApiBaseUrl(server.apiBaseUrl());
  source.setWebBaseUrl(server.webBaseUrl());

  QString error;
  const QJsonArray versions = source.getVersions(&error);
  QVERIFY(error.isEmpty());
  QCOMPARE(versions.size(), 1);
  QCOMPARE(versions.at(0).toObject().value(QStringLiteral("key")).toString(), QStringLiteral("v1.94"));
}

void AM32SourceTests::getVersions_ignoresMalformedTags() {
  Am32ApiServer server;
  server.setBody(
      "["
      "{\"name\":\"\",\"tag_name\":\"nightly\",\"assets\":[{\"name\":\"a.hex\"}],\"prerelease\":true},"
      "{\"name\":\"\",\"tag_name\":\"v1.94\",\"assets\":[{\"name\":\"b.hex\"}],\"prerelease\":false}"
      "]");
  QVERIFY(server.start());

  QJsonObject escs{{QStringLiteral("layouts"),
                    QJsonObject{{QStringLiteral("ESC1"), QJsonObject{{QStringLiteral("fileName"), QStringLiteral("F4")}}}}}};
  sources::AM32Source source(QJsonObject{}, QJsonObject{}, escs);
  source.setApiBaseUrl(server.apiBaseUrl());
  source.setWebBaseUrl(server.webBaseUrl());

  QString error;
  const QJsonArray versions = source.getVersions(&error);
  QVERIFY(error.isEmpty());
  QCOMPARE(versions.size(), 1);
  QCOMPARE(versions.at(0).toObject().value(QStringLiteral("key")).toString(), QStringLiteral("v1.94"));
}

void AM32SourceTests::getFirmwareUrl_usesEscLayoutFilename() {
  QJsonObject escs{{QStringLiteral("layouts"),
                    QJsonObject{{QStringLiteral("ESC1"), QJsonObject{{QStringLiteral("fileName"), QStringLiteral("F4")}}}}}};
  sources::AM32Source source(QJsonObject{}, QJsonObject{}, escs);

  QCOMPARE(source.getFirmwareUrl(QStringLiteral("ESC1"),
                                 QStringLiteral("v1.94"),
                                 QStringLiteral("https://example/")),
           QStringLiteral("https://example/AM32_F4_1.94.hex"));
}

QTEST_MAIN(AM32SourceTests)
#include "test_am32_source.moc"
