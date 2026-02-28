#include "Msp.hpp"

#include <QtGlobal>

namespace msp {

namespace {

quint8 crc8DvbS2(const QByteArray& data) {
  quint8 crc = 0;

  for (const auto& raw : data) {
    crc ^= static_cast<quint8>(raw);

    for (int i = 0; i < 8; ++i) {
      const bool msb = (crc & 0x80U) != 0;
      crc <<= 1;
      if (msb) {
        crc ^= 0xD5U;
      }
    }
  }

  return crc;
}

}  // namespace

QByteArray encodeV1(quint16 command, const QByteArray& payload) {
  QByteArray frame;
  frame.append('$');
  frame.append('M');
  frame.append('<');
  frame.append(static_cast<char>(payload.size()));
  frame.append(static_cast<char>(command & 0xFF));
  frame.append(payload);

  quint8 checksum = static_cast<quint8>(payload.size()) ^ static_cast<quint8>(command & 0xFF);
  for (const auto& byte : payload) {
    checksum ^= static_cast<quint8>(byte);
  }

  frame.append(static_cast<char>(checksum));
  return frame;
}

QByteArray encodeV2(quint16 command, const QByteArray& payload, quint8 flags) {
  QByteArray frame;
  frame.append('$');
  frame.append('X');
  frame.append('<');
  frame.append(static_cast<char>(flags));
  frame.append(static_cast<char>(command & 0xFF));
  frame.append(static_cast<char>((command >> 8) & 0xFF));

  const quint16 size = static_cast<quint16>(payload.size());
  frame.append(static_cast<char>(size & 0xFF));
  frame.append(static_cast<char>((size >> 8) & 0xFF));
  frame.append(payload);

  QByteArray crcInput;
  crcInput.append(static_cast<char>(flags));
  crcInput.append(static_cast<char>(command & 0xFF));
  crcInput.append(static_cast<char>((command >> 8) & 0xFF));
  crcInput.append(static_cast<char>(size & 0xFF));
  crcInput.append(static_cast<char>((size >> 8) & 0xFF));
  crcInput.append(payload);

  frame.append(static_cast<char>(crc8DvbS2(crcInput)));
  return frame;
}

QByteArray encode(quint16 command, const QByteArray& payload) {
  if (command <= 0xFF) {
    return encodeV1(command, payload);
  }

  return encodeV2(command, payload);
}

bool decode(const QByteArray& buffer, Frame* outFrame, int* consumedBytes) {
  if (consumedBytes != nullptr) {
    *consumedBytes = 0;
  }

  if (outFrame == nullptr || buffer.size() < 6) {
    return false;
  }

  if (buffer.at(0) != '$') {
    return false;
  }

  if (buffer.at(1) == 'M') {
    const int payloadSize = static_cast<quint8>(buffer.at(3));
    const int frameSize = 6 + payloadSize;
    if (buffer.size() < frameSize) {
      return false;
    }

    const quint8 command = static_cast<quint8>(buffer.at(4));
    const QByteArray payload = buffer.mid(5, payloadSize);
    const quint8 checksum = static_cast<quint8>(buffer.at(5 + payloadSize));

    quint8 expected = static_cast<quint8>(payloadSize) ^ command;
    for (const auto& byte : payload) {
      expected ^= static_cast<quint8>(byte);
    }

    if (checksum != expected) {
      return false;
    }

    outFrame->command = command;
    outFrame->payload = payload;
    outFrame->v2 = false;
    if (consumedBytes != nullptr) {
      *consumedBytes = frameSize;
    }
    return true;
  }

  if (buffer.at(1) == 'X') {
    if (buffer.size() < 9) {
      return false;
    }

    const quint8 flags = static_cast<quint8>(buffer.at(3));
    const quint16 command = static_cast<quint8>(buffer.at(4)) |
                            (static_cast<quint16>(static_cast<quint8>(buffer.at(5))) << 8);
    const quint16 payloadSize = static_cast<quint8>(buffer.at(6)) |
                                (static_cast<quint16>(static_cast<quint8>(buffer.at(7))) << 8);
    const int frameSize = 9 + payloadSize;
    if (buffer.size() < frameSize) {
      return false;
    }

    const QByteArray payload = buffer.mid(8, payloadSize);
    const quint8 checksum = static_cast<quint8>(buffer.at(8 + payloadSize));

    QByteArray crcInput;
    crcInput.append(static_cast<char>(flags));
    crcInput.append(static_cast<char>(command & 0xFF));
    crcInput.append(static_cast<char>((command >> 8) & 0xFF));
    crcInput.append(static_cast<char>(payloadSize & 0xFF));
    crcInput.append(static_cast<char>((payloadSize >> 8) & 0xFF));
    crcInput.append(payload);

    if (crc8DvbS2(crcInput) != checksum) {
      return false;
    }

    outFrame->command = command;
    outFrame->payload = payload;
    outFrame->v2 = true;
    if (consumedBytes != nullptr) {
      *consumedBytes = frameSize;
    }
    return true;
  }

  return false;
}

}  // namespace msp
