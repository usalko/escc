#include "Flash.hpp"

#include "core/FourWay.hpp"

namespace flash {

namespace {

int hexByte(const QString& line, int start, bool* ok) {
  return line.mid(start, 2).toInt(ok, 16);
}

int hexWord(const QString& line, int start, bool* ok) {
  return line.mid(start, 4).toInt(ok, 16);
}

}  // namespace

std::optional<QByteArray> fillImage(const ParsedHex& parsed, int size, quint32 flashOffset) {
  if (size <= 0) {
    return std::nullopt;
  }

  QByteArray image(size, static_cast<char>(0xFF));

  for (const auto& block : parsed.data) {
    if (block.address < flashOffset) {
      return std::nullopt;
    }

    const quint32 address = block.address - flashOffset;
    if (address >= static_cast<quint32>(image.size())) {
      return std::nullopt;
    }

    const int clampedLength =
        qMin(block.data.size(), image.size() - static_cast<int>(address));
    for (int i = 0; i < clampedLength; ++i) {
      image[static_cast<int>(address) + i] = block.data.at(i);
    }
  }

  return image;
}

std::optional<ParsedHex> parseHex(const QString& content, QString* error) {
  QStringList lines = content.split('\n');
  if (!lines.isEmpty() && lines.back().isEmpty()) {
    lines.pop_back();
  }

  ParsedHex result;
  quint32 extendedLinearAddress = 0;
  int nextAddress = 0;

  for (int i = 0; i < lines.size(); ++i) {
    QString line = lines.at(i);
    if (line.endsWith('\r')) {
      line.chop(1);
    }

    if (!line.startsWith(':') || line.size() < 11) {
      if (error != nullptr) {
        *error = QStringLiteral("Invalid HEX line format");
      }
      return std::nullopt;
    }

    bool ok = false;
    const int byteCount = hexByte(line, 1, &ok);
    if (!ok) {
      if (error != nullptr) {
        *error = QStringLiteral("Invalid HEX byte count");
      }
      return std::nullopt;
    }

    const int address = hexWord(line, 3, &ok);
    if (!ok) {
      if (error != nullptr) {
        *error = QStringLiteral("Invalid HEX address");
      }
      return std::nullopt;
    }

    const int recordType = hexByte(line, 7, &ok);
    if (!ok) {
      if (error != nullptr) {
        *error = QStringLiteral("Invalid HEX record type");
      }
      return std::nullopt;
    }

    const int dataStart = 9;
    const int dataLen = byteCount * 2;
    if (line.size() < dataStart + dataLen + 2) {
      if (error != nullptr) {
        *error = QStringLiteral("HEX line is truncated");
      }
      return std::nullopt;
    }

    const QString payload = line.mid(dataStart, dataLen);
    const int checksum = hexByte(line, dataStart + dataLen, &ok);
    if (!ok) {
      if (error != nullptr) {
        *error = QStringLiteral("Invalid HEX checksum");
      }
      return std::nullopt;
    }

    switch (recordType) {
      case 0x00: {
        if (address != nextAddress || nextAddress == 0) {
          result.data.push_back(
              HexBlock{extendedLinearAddress + static_cast<quint32>(address), {}});
        }

        nextAddress = address + byteCount;

        int crc = byteCount;
        crc += hexByte(line, 3, &ok);
        crc += hexByte(line, 5, &ok);
        crc += recordType;

        auto& block = result.data.back();
        for (int pos = 0; pos < dataLen; pos += 2) {
          const int value = payload.mid(pos, 2).toInt(&ok, 16);
          if (!ok) {
            if (error != nullptr) {
              *error = QStringLiteral("Invalid HEX data byte");
            }
            return std::nullopt;
          }

          block.data.push_back(static_cast<char>(value & 0xFF));
          crc += value;
          result.bytes += 1;
        }

        crc = (~crc + 1) & 0xFF;
        if (crc != checksum) {
          if (error != nullptr) {
            *error = QStringLiteral("HEX checksum mismatch");
          }
          return std::nullopt;
        }
      } break;

      case 0x01:
        result.endOfFile = true;
        break;

      case 0x02:
      case 0x03:
        if (payload.toInt(&ok, 16) != 0) {
          if (error != nullptr) {
            *error = QStringLiteral("Unsupported HEX segment address record");
          }
          return std::nullopt;
        }
        break;

      case 0x04: {
        const int b0 = hexByte(payload, 0, &ok);
        const int b1 = hexByte(payload, 2, &ok);
        if (!ok) {
          if (error != nullptr) {
            *error = QStringLiteral("Invalid HEX extended linear address");
          }
          return std::nullopt;
        }

        extendedLinearAddress = (static_cast<quint32>(b0) << 24) |
                                (static_cast<quint32>(b1) << 16);
      } break;

      case 0x05:
        result.startLinearAddress = payload.toUInt(&ok, 16);
        if (!ok) {
          if (error != nullptr) {
            *error = QStringLiteral("Invalid HEX start linear address");
          }
          return std::nullopt;
        }
        break;

      default:
        break;
    }
  }

  if (!result.endOfFile) {
    if (error != nullptr) {
      *error = QStringLiteral("HEX end-of-file record is missing");
    }
    return std::nullopt;
  }

  return result;
}

std::optional<FlashInfo> getInfo(const QByteArray& params, QString* error) {
  if (params.size() < 4) {
    if (error != nullptr) {
      *error = QStringLiteral("InitFlash params are too short");
    }
    return std::nullopt;
  }

  FlashInfo info;
  info.meta.signature = static_cast<quint16>(static_cast<quint8>(params.at(1)) << 8) |
                        static_cast<quint8>(params.at(0));
  info.meta.input = static_cast<quint8>(params.at(2));
  info.meta.interfaceMode = static_cast<quint8>(params.at(3));

  const auto mode = static_cast<fourway::Mode>(info.meta.interfaceMode);
  info.isAtmel = fourway::isAtmelMode(mode);
  info.isSiLabs = fourway::isSilabsMode(mode);
  info.isArm = mode == fourway::Mode::ARMBLB;

  return info;
}

}  // namespace flash
