#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "sources/BluejaySource.hpp"

class BluejayApiServer final : public QObject {
  Q_OBJECT

 public:
  bool start() {
    if (!server_.listen(QHostAddress::LocalHost)) {
      return false;
    }

    connect(&server_, &QTcpServer::newConnection, this, [this]() {
      auto* socket = server_.nextPendingConnection();
      connect(socket, &QTcpSocket::readyRead, this, [socket]() {
        socket->readAll();
        const QByteArray body =
            "["
            "{\"name\":\"\",\"tag_name\":\"v0.21.0\",\"assets\":[{\"name\":\"a.hex\"}],\"prerelease\":false,\"published_at\":\"2026-01-01T00:00:00Z\"},"
            "{\"name\":\"skip-me\",\"tag_name\":\"v0.20.0\",\"assets\":[{\"name\":\"b.hex\"}],\"prerelease\":false,\"published_at\":\"2026-01-02T00:00:00Z\"}"
            "]";

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
  QTcpServer server_;
};

class BluejaySourceTests final : public QObject {
  Q_OBJECT

 private slots:
  void getVersions_filtersBlacklist();
  void pwmAndFirmwareUrl_followRules();
};

void BluejaySourceTests::getVersions_filtersBlacklist() {
  BluejayApiServer server;
  QVERIFY(server.start());

  QJsonObject escs{{QStringLiteral("layouts"),
                    QJsonObject{{QStringLiteral("ABC"), QJsonObject{{QStringLiteral("name"), QStringLiteral("A-B C")}}}}}};
  sources::BluejaySource source(QJsonObject{}, QJsonObject{}, escs, {QStringLiteral("v0.20.0")});
  source.setApiBaseUrl(server.apiBaseUrl());
  source.setWebBaseUrl(server.webBaseUrl());

  QString error;
  const QJsonArray versions = source.getVersions(&error);
  QVERIFY(error.isEmpty());
  QCOMPARE(versions.size(), 1);
  QCOMPARE(versions.at(0).toObject().value(QStringLiteral("name")).toString(), QStringLiteral("0.21.0"));
}

void BluejaySourceTests::pwmAndFirmwareUrl_followRules() {
  QJsonObject escs{{QStringLiteral("layouts"),
                    QJsonObject{{QStringLiteral("ABC"), QJsonObject{{QStringLiteral("name"), QStringLiteral("A-B C")}}}}}};
  sources::BluejaySource source(QJsonObject{}, QJsonObject{}, escs);

  QCOMPARE(source.getPwmForVersion(QStringLiteral("0.21.0")), QVector<int>({24, 48, 96}));
  QCOMPARE(source.getPwmForVersion(QStringLiteral("0.22.0")), QVector<int>());

  QCOMPARE(source.getFirmwareUrl(QStringLiteral("ABC"),
                                 QStringLiteral("0.21.0"),
                                 48,
                                 QStringLiteral("https://example/")),
           QStringLiteral("https://example/A_B_C_48_0.21.0.hex"));
}

QTEST_MAIN(BluejaySourceTests)
#include "test_bluejay_source.moc"
