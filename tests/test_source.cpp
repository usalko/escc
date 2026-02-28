#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "sources/Source.hpp"

namespace {

QJsonObject makeEeprom() {
  QJsonObject layout;
  layout.insert(QStringLiteral("NAME"), QJsonObject{{QStringLiteral("offset"), 0}, {QStringLiteral("size"), 12}});

  QJsonObject eeprom;
  eeprom.insert(QStringLiteral("NAMES"), QJsonArray{QStringLiteral("LAYOUT_A"), QStringLiteral("LAYOUT_B")});
  eeprom.insert(QStringLiteral("LAYOUT_SIZE"), 64);
  eeprom.insert(QStringLiteral("LAYOUT"), layout);
  return eeprom;
}

QJsonObject makeSettingsDescriptions() {
  const QJsonObject commonRev{{QStringLiteral("base"), QJsonArray{QJsonObject{{QStringLiteral("name"), QStringLiteral("PWM")}}}}};
  const QJsonObject individualRev{{QStringLiteral("base"), QJsonArray{QJsonObject{{QStringLiteral("name"), QStringLiteral("DIR")}}}}};
  const QJsonObject defaultsRev{{QStringLiteral("PWM"), 24}};

  QJsonObject settings;
  settings.insert(QStringLiteral("COMMON"), QJsonObject{{QStringLiteral("1"), commonRev}});
  settings.insert(QStringLiteral("INDIVIDUAL"), QJsonObject{{QStringLiteral("1"), individualRev}});
  settings.insert(QStringLiteral("DEFAULTS"), QJsonObject{{QStringLiteral("1"), defaultsRev}});
  return settings;
}

QJsonObject makeEscs() {
  QJsonObject layouts;
  layouts.insert(QStringLiteral("LAYOUT_A"), QJsonObject{});
  layouts.insert(QStringLiteral("LAYOUT_B"), QJsonObject{});

  QJsonObject escs;
  escs.insert(QStringLiteral("layouts"), layouts);
  escs.insert(QStringLiteral("mcus"), QJsonArray{QJsonObject{{QStringLiteral("signature"), QStringLiteral("0xE8B2")}}});
  return escs;
}

}  // namespace

class TestSource final : public sources::Source {
 public:
  TestSource() : Source(QStringLiteral("TestSource"), makeEeprom(), makeSettingsDescriptions(), makeEscs()) {}

  QString buildDisplayName(const QJsonObject& firmware) const override {
    return firmware.value(QStringLiteral("name")).toString();
  }

  QJsonArray getVersions(QString* error = nullptr) override {
    Q_UNUSED(error);
    return QJsonArray{QJsonObject{{QStringLiteral("version"), QStringLiteral("1.0.0")}}};
  }
};

class TestServer final : public QObject {
  Q_OBJECT

 public:
  bool start() {
    const bool ok = server_.listen(QHostAddress::LocalHost);
    if (!ok) {
      return false;
    }

    connect(&server_, &QTcpServer::newConnection, this, [this]() {
      auto* socket = server_.nextPendingConnection();
      connect(socket, &QTcpSocket::readyRead, this, [socket]() {
        socket->readAll();

        const QByteArray body = "[{\"version\":\"2.0.0\"}]";
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

  QUrl url() const {
    return QUrl(QStringLiteral("http://127.0.0.1:%1/versions").arg(server_.serverPort()));
  }

 private:
  QTcpServer server_;
};

class SourceTests final : public QObject {
  Q_OBJECT

 private slots:
  void baseGetters_workAsExpected();
  void remoteVersions_fetchesJson();
};

void SourceTests::baseGetters_workAsExpected() {
  TestSource source;

  QCOMPARE(source.getName(), QStringLiteral("TestSource"));
  QCOMPARE(source.getLayoutSize(), 64);
  QVERIFY(source.isValidName(QStringLiteral("LAYOUT_A")));
  QVERIFY(!source.isValidName(QStringLiteral("UNKNOWN")));
  QVERIFY(source.isValidLayout(QStringLiteral("LAYOUT_B")));
  QVERIFY(source.canMigrateTo(QStringLiteral("LAYOUT_A")));

  const auto revisions = source.getRevisions();
  QVERIFY(revisions.contains(QStringLiteral("1")));
  QCOMPARE(source.getCommonSettings(QStringLiteral("1")).value(QStringLiteral("base")).toArray().size(), 1);
  QCOMPARE(source.getIndividualSettings(QStringLiteral("1")).value(QStringLiteral("base")).toArray().size(), 1);
}

void SourceTests::remoteVersions_fetchesJson() {
  TestServer server;
  QVERIFY(server.start());

  TestSource source;
  QJsonDocument json;
  QString error;

  QVERIFY(source.getRemoteVersionsList(server.url(), &json, &error));
  QVERIFY(error.isEmpty());
  QVERIFY(json.isArray());
  QCOMPARE(json.array().at(0).toObject().value(QStringLiteral("version")).toString(),
           QStringLiteral("2.0.0"));
}

QTEST_MAIN(SourceTests)
#include "test_source.moc"
