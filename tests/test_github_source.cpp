#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "sources/GithubSource.hpp"

class TestGithubSource final : public sources::GithubSource {
 public:
  TestGithubSource()
      : GithubSource(
            QStringLiteral("GithubBased"),
            QJsonObject{{QStringLiteral("NAMES"), QJsonArray{}},
                        {QStringLiteral("LAYOUT_SIZE"), 0},
                        {QStringLiteral("LAYOUT"), QJsonObject{}}},
            QJsonObject{{QStringLiteral("COMMON"), QJsonObject{}},
                        {QStringLiteral("INDIVIDUAL"), QJsonObject{}},
                        {QStringLiteral("DEFAULTS"), QJsonObject{}}},
            QJsonObject{{QStringLiteral("layouts"), QJsonObject{}},
                        {QStringLiteral("mcus"), QJsonArray{}}}) {}

  QString buildDisplayName(const QJsonObject& firmware) const override {
    return firmware.value(QStringLiteral("name")).toString();
  }

  QJsonArray getVersions(QString* error = nullptr) override {
    Q_UNUSED(error);
    return {};
  }
};

class GithubApiServer final : public QObject {
  Q_OBJECT

 public:
  void setBody(QByteArray body) { body_ = std::move(body); }

  bool start() {
    const bool ok = server_.listen(QHostAddress::LocalHost);
    if (!ok) {
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

  QString apiBaseUrl() const {
    return QStringLiteral("http://127.0.0.1:%1/repos").arg(server_.serverPort());
  }

  QString webBaseUrl() const {
    return QStringLiteral("http://127.0.0.1:%1").arg(server_.serverPort());
  }

 private:
  QByteArray body_ =
      "["
      "{\"name\":\"\",\"tag_name\":\"v1.2.3\",\"assets\":[{\"name\":\"a.hex\"}],\"prerelease\":false,\"published_at\":\"2026-01-01T00:00:00Z\"},"
      "{\"name\":\"Nightly\",\"tag_name\":\"nightly\",\"assets\":[{\"name\":\"b.hex\"}],\"prerelease\":true,\"published_at\":\"2026-01-02T00:00:00Z\"},"
      "{\"name\":\"NoAssets\",\"tag_name\":\"v0.0.1\",\"assets\":[],\"prerelease\":false,\"published_at\":\"2026-01-03T00:00:00Z\"}"
      "]";
  QTcpServer server_;
};

class GithubSourceTests final : public QObject {
  Q_OBJECT

 private slots:
  void getRemoteVersionsList_filtersAndNormalizes();
  void getRemoteVersionsList_rejectsNonArrayResponse();
  void getRemoteVersionsList_skipsReleaseWithEmptyTag();
};

void GithubSourceTests::getRemoteVersionsList_filtersAndNormalizes() {
  GithubApiServer server;
  QVERIFY(server.start());

  TestGithubSource source;
  source.setApiBaseUrl(server.apiBaseUrl());
  source.setWebBaseUrl(server.webBaseUrl());
  QJsonArray versions;
  QString error;

  QVERIFY(source.getRemoteVersionsList(QStringLiteral("owner/repo"),
                                       &versions,
                                       {QStringLiteral("nightly")},
                                       10,
                                       &error));
  QVERIFY(error.isEmpty());

  QCOMPARE(versions.size(), 1);
  const QJsonObject first = versions.at(0).toObject();
  QCOMPARE(first.value(QStringLiteral("name")).toString(), QStringLiteral("1.2.3"));
  QCOMPARE(first.value(QStringLiteral("key")).toString(), QStringLiteral("v1.2.3"));
  QVERIFY(first.value(QStringLiteral("url")).toString().contains(QStringLiteral("releases/download/v1.2.3/")));
  QVERIFY(first.value(QStringLiteral("releaseUrl")).toString().contains(QStringLiteral("releases/tag/v1.2.3/")));
  QCOMPARE(first.value(QStringLiteral("prerelease")).toBool(), false);
  QCOMPARE(first.value(QStringLiteral("published_at")).toString(),
           QStringLiteral("2026-01-01T00:00:00Z"));
}

void GithubSourceTests::getRemoteVersionsList_rejectsNonArrayResponse() {
  GithubApiServer server;
  server.setBody("{}");
  QVERIFY(server.start());

  TestGithubSource source;
  source.setApiBaseUrl(server.apiBaseUrl());
  source.setWebBaseUrl(server.webBaseUrl());

  QJsonArray versions;
  QString error;
  QVERIFY(!source.getRemoteVersionsList(QStringLiteral("owner/repo"),
                                        &versions,
                                        {},
                                        10,
                                        &error));
  QVERIFY(error.contains(QStringLiteral("not an array")));
}

void GithubSourceTests::getRemoteVersionsList_skipsReleaseWithEmptyTag() {
  GithubApiServer server;
  server.setBody(
      "["
      "{\"name\":\"Bad\",\"tag_name\":\"\",\"assets\":[{\"name\":\"a.hex\"}]},"
      "{\"name\":\"\",\"tag_name\":\"v1.2.4\",\"assets\":[{\"name\":\"b.hex\"}],\"prerelease\":false,\"published_at\":\"2026-01-04T00:00:00Z\"}"
      "]");
  QVERIFY(server.start());

  TestGithubSource source;
  source.setApiBaseUrl(server.apiBaseUrl());
  source.setWebBaseUrl(server.webBaseUrl());

  QJsonArray versions;
  QString error;
  QVERIFY(source.getRemoteVersionsList(QStringLiteral("owner/repo"), &versions, {}, 10, &error));
  QVERIFY(error.isEmpty());
  QCOMPARE(versions.size(), 1);
  QCOMPARE(versions.at(0).toObject().value(QStringLiteral("key")).toString(), QStringLiteral("v1.2.4"));
}

QTEST_MAIN(GithubSourceTests)
#include "test_github_source.moc"
