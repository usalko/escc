#include <QObject>
#include <QtTest>

#include "core/FourWay.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/Mcu.hpp"

class HardwareOpsTests final : public QObject {
  Q_OBJECT

 private slots:
  void factory_createsExpectedImplementation();
  void silabsErase_usesMultiplierForLargePages();
  void armErase_usesDirectPageIndex();
  void erasePages_erasesFullRange();
  void writeAndVerifyPages_roundTripSucceeds();
  void verifyPages_failsOnMismatch();
};

namespace {

QByteArray makeDeviceResponse(fourway::Command command,
                              const QByteArray& params,
                              fourway::Ack ack = fourway::Ack::Ok,
                              quint16 address = 0) {
  QByteArray response;
  response.append(static_cast<char>(fourway::kDeviceStartByte));
  response.append(static_cast<char>(command));
  response.append(static_cast<char>((address >> 8) & 0xFF));
  response.append(static_cast<char>(address & 0xFF));
  response.append(static_cast<char>(params.size()));
  response.append(params);
  response.append(static_cast<char>(ack));

  const quint16 crc = fourway::crc16Xmodem(response);
  response.append(static_cast<char>((crc >> 8) & 0xFF));
  response.append(static_cast<char>(crc & 0xFF));
  return response;
}

}  // namespace

void HardwareOpsTests::factory_createsExpectedImplementation() {
  auto silabs = hardware::createHardware(static_cast<quint8>(fourway::Mode::SiLBLB));
  QVERIFY(silabs != nullptr);

  auto arm = hardware::createHardware(static_cast<quint8>(fourway::Mode::ARMBLB));
  QVERIFY(arm != nullptr);

  auto unknown = hardware::createHardware(0xFF);
  QVERIFY(unknown == nullptr);
}

void HardwareOpsTests::silabsErase_usesMultiplierForLargePages() {
  FourWay fourWay;
  QByteArray lastRequest;
  fourWay.setTransport([&lastRequest](const QByteArray& request) {
    lastRequest = request;
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  const auto profile = hardware::findMcuProfile(static_cast<quint8>(fourway::Mode::SiLBLB), 0xE8B5);
  QVERIFY(profile.has_value());

  auto hw = hardware::createHardware(static_cast<quint8>(fourway::Mode::SiLBLB));
  QVERIFY(hw != nullptr);

  QString error;
  QVERIFY(hw->erasePage(fourWay, 2, profile.value(), &error));
  QVERIFY(error.isEmpty());

  QCOMPARE(static_cast<quint8>(lastRequest.at(1)),
           static_cast<quint8>(fourway::Command::DevicePageErase));
  QCOMPARE(static_cast<quint8>(lastRequest.at(5)), static_cast<quint8>(8));
}

void HardwareOpsTests::armErase_usesDirectPageIndex() {
  FourWay fourWay;
  QByteArray lastRequest;
  fourWay.setTransport([&lastRequest](const QByteArray& request) {
    lastRequest = request;
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  const auto profile = hardware::findMcuProfile(static_cast<quint8>(fourway::Mode::ARMBLB), 0x1F06);
  QVERIFY(profile.has_value());

  auto hw = hardware::createHardware(static_cast<quint8>(fourway::Mode::ARMBLB));
  QVERIFY(hw != nullptr);

  QString error;
  QVERIFY(hw->erasePage(fourWay, 7, profile.value(), &error));
  QVERIFY(error.isEmpty());

  QCOMPARE(static_cast<quint8>(lastRequest.at(1)),
           static_cast<quint8>(fourway::Command::DevicePageErase));
  QCOMPARE(static_cast<quint8>(lastRequest.at(5)), static_cast<quint8>(7));
}

void HardwareOpsTests::erasePages_erasesFullRange() {
  FourWay fourWay;
  QList<quint8> erasedPages;
  fourWay.setTransport([&erasedPages](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    if (command == fourway::Command::DevicePageErase) {
      erasedPages.push_back(static_cast<quint8>(request.at(5)));
    }
    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  const auto profile = hardware::findMcuProfile(static_cast<quint8>(fourway::Mode::ARMBLB), 0x1F06);
  QVERIFY(profile.has_value());

  auto hw = hardware::createHardware(static_cast<quint8>(fourway::Mode::ARMBLB));
  QVERIFY(hw != nullptr);

  QString error;
  QVERIFY(hw->erasePages(fourWay, 2, 5, profile.value(), &error));
  QVERIFY(error.isEmpty());

  QCOMPARE(erasedPages.size(), 3);
  QCOMPARE(erasedPages.at(0), static_cast<quint8>(2));
  QCOMPARE(erasedPages.at(1), static_cast<quint8>(3));
  QCOMPARE(erasedPages.at(2), static_cast<quint8>(4));
}

void HardwareOpsTests::writeAndVerifyPages_roundTripSucceeds() {
  FourWay fourWay;
  QByteArray memory(64, '\0');

  fourWay.setTransport([&memory](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceWrite) {
      int payloadSize = static_cast<quint8>(request.at(4));
      if (payloadSize == 0) {
        payloadSize = 256;
      }
      const QByteArray payload = request.mid(5, payloadSize);
      for (int i = 0; i < payload.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceRead) {
      int readSize = static_cast<quint8>(request.at(5));
      if (readSize == 0) {
        readSize = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, readSize), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  const auto profile = hardware::findMcuProfile(static_cast<quint8>(fourway::Mode::ARMBLB), 0x1F06);
  QVERIFY(profile.has_value());

  auto hw = hardware::createHardware(static_cast<quint8>(fourway::Mode::ARMBLB));
  QVERIFY(hw != nullptr);

  const QByteArray image = QByteArray::fromHex("01020304A1A2A3A4");
  QString error;
  QVERIFY(hw->writePages(fourWay, 0, 2, 4, image, profile.value(), &error));
  QVERIFY(hw->verifyPages(fourWay, 0, 2, 4, image, profile.value(), &error));
  QVERIFY(error.isEmpty());
}

void HardwareOpsTests::verifyPages_failsOnMismatch() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceRead) {
      int readSize = static_cast<quint8>(request.at(5));
      if (readSize == 0) {
        readSize = 256;
      }
      return makeDeviceResponse(command, QByteArray(readSize, '\x00'), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  const auto profile = hardware::findMcuProfile(static_cast<quint8>(fourway::Mode::ARMBLB), 0x1F06);
  QVERIFY(profile.has_value());

  auto hw = hardware::createHardware(static_cast<quint8>(fourway::Mode::ARMBLB));
  QVERIFY(hw != nullptr);

  QString error;
  const QByteArray image = QByteArray::fromHex("01020304");
  QVERIFY(!hw->verifyPages(fourWay, 0, 1, 4, image, profile.value(), &error));
  QVERIFY(!error.isEmpty());
}

QTEST_APPLESS_MAIN(HardwareOpsTests)
#include "test_hardware_ops.moc"
