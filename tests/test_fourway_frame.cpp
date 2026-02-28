#include <QObject>
#include <QtTest>

#include "core/FourWay.hpp"

class FourWayFrameTests final : public QObject {
  Q_OBJECT

 private slots:
  void createMessage_buildsHostFrame();
  void commandBuilders_generateExpectedCommands();
  void parseMessage_readsDeviceFrame();
  void parseMessage_rejectsBadChecksum();
  void responseHelpers_validateAckAndCommand();
  void highLevelHelpers_validateAndExtractVersions();
  void transportExchange_handlesTestAlive();
  void transportExchange_getsProtocolVersion();
  void transportExchange_reportsErrors();
  void transportOps_initReadWriteResetWork();
  void transportOps_validateInputRanges();
  void initFlashInfo_parsesSignatureAndMode();
  void settingsReadWrite_roundTripAndVerify();
  void settingsWrite_failsOnVerificationMismatch();
  void settingsReadWrite_useEepromCommandsForAtmel();
  void settingsAuto_usesResolvedEepromOffset();
  void settingsAuto_failsWhenMcuUnknown();
  void flashImage_runsEraseWriteVerifyPipeline();
  void flashImage_failsWhenMcuProfileMissing();
  void flashImageModeAware_silabsUsesSafetyPhases();
  void flashImageModeAware_armResetsAfterFlash();
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

void FourWayFrameTests::createMessage_buildsHostFrame() {
  const QByteArray params = QByteArray::fromHex("0102");
  const QByteArray frame = fourway::createMessage(fourway::Command::DeviceRead, params, 0x1234);

  QCOMPARE(static_cast<quint8>(frame.at(0)), fourway::kHostStartByte);
  QCOMPARE(static_cast<quint8>(frame.at(1)), static_cast<quint8>(fourway::Command::DeviceRead));
  QCOMPARE(static_cast<quint8>(frame.at(2)), static_cast<quint8>(0x12));
  QCOMPARE(static_cast<quint8>(frame.at(3)), static_cast<quint8>(0x34));
  QCOMPARE(static_cast<quint8>(frame.at(4)), static_cast<quint8>(2));
  QCOMPARE(frame.mid(5, 2), params);

  const quint16 expectedCrc = fourway::crc16Xmodem(frame.left(frame.size() - 2));
  const quint16 actualCrc = static_cast<quint16>(static_cast<quint8>(frame.at(frame.size() - 2)) << 8) |
                            static_cast<quint8>(frame.at(frame.size() - 1));
  QCOMPARE(actualCrc, expectedCrc);
}

void FourWayFrameTests::commandBuilders_generateExpectedCommands() {
  const QByteArray alive = FourWay::buildInterfaceTestAlive();
  const QByteArray version = FourWay::buildProtocolGetVersion();
  const QByteArray exit = FourWay::buildInterfaceExit();
  const QByteArray mode = FourWay::buildSetMode(fourway::Mode::AtmBLB, 0x0102);

  QCOMPARE(static_cast<quint8>(alive.at(1)),
           static_cast<quint8>(fourway::Command::InterfaceTestAlive));
  QCOMPARE(static_cast<quint8>(version.at(1)),
           static_cast<quint8>(fourway::Command::ProtocolGetVersion));
  QCOMPARE(static_cast<quint8>(exit.at(1)),
           static_cast<quint8>(fourway::Command::InterfaceExit));

  QCOMPARE(static_cast<quint8>(mode.at(1)),
           static_cast<quint8>(fourway::Command::InterfaceSetMode));
  QCOMPARE(static_cast<quint8>(mode.at(2)), static_cast<quint8>(0x01));
  QCOMPARE(static_cast<quint8>(mode.at(3)), static_cast<quint8>(0x02));
}

void FourWayFrameTests::parseMessage_readsDeviceFrame() {
  const QByteArray params = QByteArray::fromHex("1122");

  QByteArray message;
  message.append(static_cast<char>(fourway::kDeviceStartByte));
  message.append(static_cast<char>(fourway::Command::InterfaceGetVersion));
  message.append(static_cast<char>(0xAB));
  message.append(static_cast<char>(0xCD));
  message.append(static_cast<char>(params.size()));
  message.append(params);
  message.append(static_cast<char>(fourway::Ack::Ok));

  const quint16 crc = fourway::crc16Xmodem(message);
  message.append(static_cast<char>((crc >> 8) & 0xFF));
  message.append(static_cast<char>(crc & 0xFF));

  fourway::Response response;
  int consumed = 0;
  const auto status = fourway::parseMessage(message, &response, &consumed);

  QCOMPARE(status, fourway::ParseStatus::Ok);
  QCOMPARE(consumed, message.size());
  QCOMPARE(response.command,
           static_cast<quint8>(fourway::Command::InterfaceGetVersion));
  QCOMPARE(response.address, static_cast<quint16>(0xABCD));
  QCOMPARE(response.ack, static_cast<quint8>(fourway::Ack::Ok));
  QCOMPARE(response.params, params);
}

void FourWayFrameTests::parseMessage_rejectsBadChecksum() {
  QByteArray message;
  message.append(static_cast<char>(fourway::kDeviceStartByte));
  message.append(static_cast<char>(fourway::Command::InterfaceTestAlive));
  message.append(static_cast<char>(0x00));
  message.append(static_cast<char>(0x01));
  message.append(static_cast<char>(1));
  message.append(static_cast<char>(0xAA));
  message.append(static_cast<char>(fourway::Ack::Ok));
  message.append(static_cast<char>(0x00));
  message.append(static_cast<char>(0x00));

  fourway::Response response;
  int consumed = 0;
  const auto status = fourway::parseMessage(message, &response, &consumed);

  QCOMPARE(status, fourway::ParseStatus::InvalidChecksum);
  QCOMPARE(consumed, 0);
}

void FourWayFrameTests::responseHelpers_validateAckAndCommand() {
  fourway::Response response;
  response.command = static_cast<quint8>(fourway::Command::DeviceRead);
  response.ack = static_cast<quint8>(fourway::Ack::Ok);

  QVERIFY(FourWay::isAckOk(response));
  QVERIFY(FourWay::responseMatchesCommand(response, fourway::Command::DeviceRead));
  QVERIFY(!FourWay::responseMatchesCommand(response, fourway::Command::DeviceWrite));

  response.ack = static_cast<quint8>(fourway::Ack::DeviceCommandFailed);
  QVERIFY(!FourWay::isAckOk(response));
}

void FourWayFrameTests::highLevelHelpers_validateAndExtractVersions() {
  fourway::Response response;
  response.command = static_cast<quint8>(fourway::Command::ProtocolGetVersion);
  response.ack = static_cast<quint8>(fourway::Ack::Ok);
  response.params = QByteArray::fromHex("0201");

  QString error;
  QVERIFY(FourWay::validateOkResponse(response, fourway::Command::ProtocolGetVersion, &error));
  QVERIFY(error.isEmpty());

  const auto protocolVersion = FourWay::protocolVersionFromResponse(response);
  QVERIFY(protocolVersion.has_value());
  QCOMPARE(protocolVersion.value(), static_cast<quint16>(0x0102));

  const QByteArray interfaceVersionRequest = FourWay::buildInterfaceGetVersion();
  QCOMPARE(static_cast<quint8>(interfaceVersionRequest.at(1)),
           static_cast<quint8>(fourway::Command::InterfaceGetVersion));

  response.command = static_cast<quint8>(fourway::Command::InterfaceGetVersion);
  response.params = QByteArray::fromHex("05");
  const auto interfaceVersion = FourWay::interfaceVersionFromResponse(response);
  QVERIFY(interfaceVersion.has_value());
  QCOMPARE(interfaceVersion.value(), static_cast<quint16>(0x05));

  response.ack = static_cast<quint8>(fourway::Ack::DeviceCommandFailed);
  QVERIFY(!FourWay::validateOkResponse(response, fourway::Command::InterfaceGetVersion, &error));
  QVERIFY(!error.isEmpty());
}

void FourWayFrameTests::transportExchange_handlesTestAlive() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("01"));
  });

  QString error;
  QVERIFY(fourWay.testAlive(&error));
  QVERIFY(error.isEmpty());
}

void FourWayFrameTests::transportExchange_getsProtocolVersion() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("0302"));
  });

  QString error;
  const auto version = fourWay.getProtocolVersion(&error);

  QVERIFY(version.has_value());
  QCOMPARE(version.value(), static_cast<quint16>(0x0203));
  QVERIFY(error.isEmpty());
}

void FourWayFrameTests::transportExchange_reportsErrors() {
  FourWay withoutTransport;
  QString error;
  QVERIFY(!withoutTransport.testAlive(&error));
  QVERIFY(!error.isEmpty());

  FourWay ackError;
  ackError.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("01"), fourway::Ack::DeviceCommandFailed);
  });

  error.clear();
  const auto version = ackError.getProtocolVersion(&error);
  QVERIFY(!version.has_value());
  QVERIFY(!error.isEmpty());
}

void FourWayFrameTests::transportOps_initReadWriteResetWork() {
  FourWay fourWay;
  QByteArray lastRequest;
  fourWay.setTransport([](const QByteArray& request) {
    Q_UNUSED(request);
    return makeDeviceResponse(fourway::Command::InterfaceTestAlive, QByteArray::fromHex("00"));
  });
  fourWay.setTransport([&lastRequest](const QByteArray& request) {
    lastRequest = request;
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    switch (command) {
      case fourway::Command::DeviceInitFlash:
        return makeDeviceResponse(command, QByteArray::fromHex("01EF01"));
      case fourway::Command::DeviceRead:
        return makeDeviceResponse(command, QByteArray::fromHex("A1A2A3A4"), fourway::Ack::Ok, address);
      case fourway::Command::DeviceReadEEprom:
        return makeDeviceResponse(command, QByteArray::fromHex("0B0C"), fourway::Ack::Ok, address);
      case fourway::Command::DeviceWrite:
        return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
      case fourway::Command::DeviceWriteEEprom:
        return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
      case fourway::Command::DeviceReset:
        return makeDeviceResponse(command, QByteArray::fromHex("00"));
      default:
        return makeDeviceResponse(command, QByteArray::fromHex("00"));
    }
  });

  QString error;
  const auto flash = fourWay.initFlash(2, &error);
  QVERIFY(flash.has_value());
  QVERIFY(error.isEmpty());
  QCOMPARE(static_cast<quint8>(lastRequest.at(1)), static_cast<quint8>(fourway::Command::DeviceInitFlash));
  QCOMPARE(static_cast<quint8>(lastRequest.at(5)), static_cast<quint8>(2));

  const auto readBytes = fourWay.read(0x1234, 4, &error);
  QVERIFY(readBytes.has_value());
  QCOMPARE(readBytes.value(), QByteArray::fromHex("A1A2A3A4"));
  QCOMPARE(static_cast<quint8>(lastRequest.at(1)), static_cast<quint8>(fourway::Command::DeviceRead));
  QCOMPARE(static_cast<quint16>(static_cast<quint8>(lastRequest.at(2)) << 8) |
               static_cast<quint8>(lastRequest.at(3)),
           static_cast<quint16>(0x1234));
  QCOMPARE(static_cast<quint8>(lastRequest.at(5)), static_cast<quint8>(4));

  const auto readEepromBytes = fourWay.readEeprom(0x2222, 2, &error);
  QVERIFY(readEepromBytes.has_value());
  QCOMPARE(readEepromBytes.value(), QByteArray::fromHex("0B0C"));
  QCOMPARE(static_cast<quint8>(lastRequest.at(1)), static_cast<quint8>(fourway::Command::DeviceReadEEprom));
  QCOMPARE(static_cast<quint16>(static_cast<quint8>(lastRequest.at(2)) << 8) |
               static_cast<quint8>(lastRequest.at(3)),
           static_cast<quint16>(0x2222));
  QCOMPARE(static_cast<quint8>(lastRequest.at(5)), static_cast<quint8>(2));

  QVERIFY(fourWay.write(0x3456, QByteArray::fromHex("010203"), &error));
  QCOMPARE(static_cast<quint8>(lastRequest.at(1)), static_cast<quint8>(fourway::Command::DeviceWrite));
  QCOMPARE(static_cast<quint16>(static_cast<quint8>(lastRequest.at(2)) << 8) |
               static_cast<quint8>(lastRequest.at(3)),
           static_cast<quint16>(0x3456));
  QCOMPARE(lastRequest.mid(5, 3), QByteArray::fromHex("010203"));

  QVERIFY(fourWay.writeEeprom(0x4567, QByteArray::fromHex("AABB"), &error));
  QCOMPARE(static_cast<quint8>(lastRequest.at(1)), static_cast<quint8>(fourway::Command::DeviceWriteEEprom));
  QCOMPARE(static_cast<quint16>(static_cast<quint8>(lastRequest.at(2)) << 8) |
               static_cast<quint8>(lastRequest.at(3)),
           static_cast<quint16>(0x4567));
  QCOMPARE(lastRequest.mid(5, 2), QByteArray::fromHex("AABB"));

  QVERIFY(fourWay.reset(2, &error));
  QCOMPARE(static_cast<quint8>(lastRequest.at(1)), static_cast<quint8>(fourway::Command::DeviceReset));
  QCOMPARE(static_cast<quint8>(lastRequest.at(5)), static_cast<quint8>(2));
}

void FourWayFrameTests::transportOps_validateInputRanges() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray&) {
    return makeDeviceResponse(fourway::Command::DeviceRead, QByteArray::fromHex("00"));
  });

  QString error;
  QVERIFY(!fourWay.read(0x0000, 0, &error).has_value());
  QVERIFY(!error.isEmpty());

  error.clear();
  QVERIFY(!fourWay.readEeprom(0x0000, 257, &error).has_value());
  QVERIFY(!error.isEmpty());

  error.clear();
  QVERIFY(!fourWay.write(0x0000, QByteArray(), &error));
  QVERIFY(!error.isEmpty());

  error.clear();
  QVERIFY(!fourWay.writeEeprom(0x0000, QByteArray(300, '\x01'), &error));
  QVERIFY(!error.isEmpty());
}

void FourWayFrameTests::initFlashInfo_parsesSignatureAndMode() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("34 12 00 04"));
  });

  QString error;
  const auto info = fourWay.initFlashInfo(3, &error);

  QVERIFY(info.has_value());
  QCOMPARE(info->target, static_cast<quint8>(3));
  QCOMPARE(info->signature, static_cast<quint16>(0x1234));
  QCOMPARE(info->interfaceMode, static_cast<quint8>(4));
  QVERIFY(error.isEmpty());
}

void FourWayFrameTests::settingsReadWrite_roundTripAndVerify() {
  FourWay fourWay;
  QByteArray memory(1024, '\0');
  memory.replace(0x40, 6, QByteArray::fromHex("010203040506"));

  fourWay.setTransport([&memory](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("34 12 00 01"));
    }

    if (command == fourway::Command::DeviceRead) {
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, count), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceWrite) {
      int count = static_cast<quint8>(request.at(4));
      if (count == 0) {
        count = 256;
      }
      const QByteArray payload = request.mid(5, count);
      for (int i = 0; i < payload.size() && (address + i) < memory.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  QString error;
  const auto before = fourWay.readSettings(0, 0x40, 6, &error);
  QVERIFY(before.has_value());
  QCOMPARE(before.value(), QByteArray::fromHex("010203040506"));

  const QByteArray updated = QByteArray::fromHex("A1A2A3A4A5A6");
  QVERIFY(fourWay.writeSettings(0, 0x40, updated, &error));

  const auto after = fourWay.readSettings(0, 0x40, 6, &error);
  QVERIFY(after.has_value());
  QCOMPARE(after.value(), updated);
}

void FourWayFrameTests::settingsWrite_failsOnVerificationMismatch() {
  FourWay fourWay;

  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("34 12 00 01"));
    }

    if (command == fourway::Command::DeviceRead) {
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, QByteArray(count, '\x00'), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceWrite) {
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  QString error;
  const bool ok = fourWay.writeSettings(1, 0x20, QByteArray::fromHex("DEADBEEF"), &error);
  QVERIFY(!ok);
  QVERIFY(!error.isEmpty());
}

void FourWayFrameTests::settingsReadWrite_useEepromCommandsForAtmel() {
  FourWay fourWay;
  QByteArray memory(256, '\0');
  int readEepromCalls = 0;
  int writeEepromCalls = 0;
  int readCalls = 0;
  int writeCalls = 0;

  fourWay.setTransport([&](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("34 12 00 02"));
    }

    if (command == fourway::Command::DeviceReadEEprom) {
      ++readEepromCalls;
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, count), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceWriteEEprom) {
      ++writeEepromCalls;
      int count = static_cast<quint8>(request.at(4));
      if (count == 0) {
        count = 256;
      }
      const QByteArray payload = request.mid(5, count);
      for (int i = 0; i < payload.size() && (address + i) < memory.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceRead) {
      ++readCalls;
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceWrite) {
      ++writeCalls;
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  const QByteArray settings = QByteArray::fromHex("AA55CC33");
  QString error;
  QVERIFY(fourWay.writeSettings(0, 0x10, settings, &error));

  const auto readBack = fourWay.readSettings(0, 0x10, settings.size(), &error);
  QVERIFY(readBack.has_value());
  QCOMPARE(readBack.value(), settings);

  QVERIFY(readEepromCalls > 0);
  QVERIFY(writeEepromCalls > 0);
  QCOMPARE(readCalls, 0);
  QCOMPARE(writeCalls, 0);
}

void FourWayFrameTests::settingsAuto_usesResolvedEepromOffset() {
  FourWay fourWay;
  QByteArray memory(0x2000, '\0');
  memory.replace(0x1A00, 4, QByteArray::fromHex("11223344"));
  quint16 lastAddress = 0;

  fourWay.setTransport([&](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));
    lastAddress = address;

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("B2 E8 00 01"));
    }

    if (command == fourway::Command::DeviceRead) {
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, count), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceWrite) {
      int count = static_cast<quint8>(request.at(4));
      if (count == 0) {
        count = 256;
      }
      const QByteArray payload = request.mid(5, count);
      for (int i = 0; i < payload.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  QString error;
  const auto read = fourWay.readSettingsAuto(0, 4, &error);
  QVERIFY(read.has_value());
  QCOMPARE(read.value(), QByteArray::fromHex("11223344"));
  QCOMPARE(lastAddress, static_cast<quint16>(0x1A00));

  const QByteArray update = QByteArray::fromHex("AABBCCDD");
  QVERIFY(fourWay.writeSettingsAuto(0, update, &error));
  const auto readBack = fourWay.readSettingsAuto(0, 4, &error);
  QVERIFY(readBack.has_value());
  QCOMPARE(readBack.value(), update);
}

void FourWayFrameTests::settingsAuto_failsWhenMcuUnknown() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("FF FF 00 01"));
  });

  QString error;
  QVERIFY(!fourWay.readSettingsAuto(0, 4, &error).has_value());
  QVERIFY(!error.isEmpty());

  error.clear();
  QVERIFY(!fourWay.writeSettingsAuto(0, QByteArray::fromHex("0102"), &error));
  QVERIFY(!error.isEmpty());
}

void FourWayFrameTests::flashImage_runsEraseWriteVerifyPipeline() {
  FourWay fourWay;
  QByteArray memory(2048, '\0');
  int eraseCount = 0;
  int writeCount = 0;
  int readCount = 0;

  fourWay.setTransport([&](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("B2 E8 00 01"));
    }

    if (command == fourway::Command::DevicePageErase) {
      ++eraseCount;
      return makeDeviceResponse(command, QByteArray::fromHex("00"));
    }

    if (command == fourway::Command::DeviceWrite) {
      ++writeCount;
      int count = static_cast<quint8>(request.at(4));
      if (count == 0) {
        count = 256;
      }
      const QByteArray payload = request.mid(5, count);
      for (int i = 0; i < payload.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceRead) {
      ++readCount;
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, count), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  QByteArray image(1024, '\0');
  for (int i = 0; i < image.size(); ++i) {
    image[i] = static_cast<char>(i & 0xFF);
  }

  QString error;
  QVERIFY(fourWay.flashImage(0, image, 0, 2, &error));
  QVERIFY(error.isEmpty());
  QCOMPARE(eraseCount, 2);
  QCOMPARE(writeCount, 4);
  QCOMPARE(readCount, 4);
}

void FourWayFrameTests::flashImage_failsWhenMcuProfileMissing() {
  FourWay fourWay;
  fourWay.setTransport([](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    return makeDeviceResponse(command, QByteArray::fromHex("FF FF 00 01"));
  });

  QString error;
  QByteArray image(1024, '\0');
  QVERIFY(!fourWay.flashImage(0, image, 0, 2, &error));
  QVERIFY(!error.isEmpty());
}

void FourWayFrameTests::flashImageModeAware_silabsUsesSafetyPhases() {
  FourWay fourWay;
  QByteArray memory(14 * 512, '\0');
  int eraseCount = 0;
  QList<quint8> erasedPages;

  fourWay.setTransport([&](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("B2 E8 00 01"));
    }

    if (command == fourway::Command::DevicePageErase) {
      ++eraseCount;
      erasedPages.push_back(static_cast<quint8>(request.at(5)));
      return makeDeviceResponse(command, QByteArray::fromHex("00"));
    }

    if (command == fourway::Command::DeviceWrite) {
      int count = static_cast<quint8>(request.at(4));
      if (count == 0) {
        count = 256;
      }
      const QByteArray payload = request.mid(5, count);
      for (int i = 0; i < payload.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceRead) {
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, count), fourway::Ack::Ok, address);
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  QByteArray image(14 * 512, '\0');
  for (int i = 0; i < image.size(); ++i) {
    image[i] = static_cast<char>((i * 7) & 0xFF);
  }

  QString error;
  QVERIFY(fourWay.flashImageModeAware(0, image, &error));
  QVERIFY(error.isEmpty());
  QCOMPARE(eraseCount, 14);
  QVERIFY(erasedPages.contains(static_cast<quint8>(0x02)));
  QVERIFY(erasedPages.contains(static_cast<quint8>(0x0D)));
}

void FourWayFrameTests::flashImageModeAware_armResetsAfterFlash() {
  FourWay fourWay;
  QByteArray memory(0x40 * 1024, '\0');
  bool resetCalled = false;
  int eraseCount = 0;

  fourWay.setTransport([&](const QByteArray& request) {
    const auto command = static_cast<fourway::Command>(static_cast<quint8>(request.at(1)));
    const auto address = static_cast<quint16>(static_cast<quint8>(request.at(2)) << 8) |
                         static_cast<quint8>(request.at(3));

    if (command == fourway::Command::DeviceInitFlash) {
      return makeDeviceResponse(command, QByteArray::fromHex("06 1F 00 04"));
    }

    if (command == fourway::Command::DevicePageErase) {
      ++eraseCount;
      return makeDeviceResponse(command, QByteArray::fromHex("00"));
    }

    if (command == fourway::Command::DeviceWrite) {
      int count = static_cast<quint8>(request.at(4));
      if (count == 0) {
        count = 256;
      }
      const QByteArray payload = request.mid(5, count);
      for (int i = 0; i < payload.size(); ++i) {
        memory[address + i] = payload.at(i);
      }
      return makeDeviceResponse(command, QByteArray::fromHex("00"), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceRead) {
      int count = static_cast<quint8>(request.at(5));
      if (count == 0) {
        count = 256;
      }
      return makeDeviceResponse(command, memory.mid(address, count), fourway::Ack::Ok, address);
    }

    if (command == fourway::Command::DeviceReset) {
      resetCalled = true;
      return makeDeviceResponse(command, QByteArray::fromHex("00"));
    }

    return makeDeviceResponse(command, QByteArray::fromHex("00"));
  });

  QByteArray image(0x40 * 1024, '\0');
  for (int i = 0; i < image.size(); ++i) {
    image[i] = static_cast<char>((i * 3) & 0xFF);
  }

  QString error;
  QVERIFY(fourWay.flashImageModeAware(0, image, &error));
  QVERIFY(error.isEmpty());
  QVERIFY(resetCalled);
  QCOMPARE(eraseCount, 0x40 - 0x04);
}

QTEST_APPLESS_MAIN(FourWayFrameTests)
#include "test_fourway_frame.moc"
