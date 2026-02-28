#include "General.hpp"

#include <QThread>

namespace general {

bool compare(const QByteArray& a, const QByteArray& b) {
  return a == b;
}

void delay(int ms) {
  if (ms <= 0) {
    return;
  }

  QThread::msleep(static_cast<unsigned long>(ms));
}

bool retry(const RetryOperation& operation,
           int maxRetries,
           int iterationDelayMs,
           QString* error,
           int* attempts) {
  if (!operation || maxRetries <= 0) {
    if (attempts != nullptr) {
      *attempts = 0;
    }
    if (error != nullptr && maxRetries <= 0) {
      *error = QStringLiteral("maxRetries must be positive");
    }
    return false;
  }

  int currentAttempts = 0;
  QString lastError;

  while (currentAttempts < maxRetries) {
    ++currentAttempts;
    QString attemptError;
    if (operation(&attemptError)) {
      if (attempts != nullptr) {
        *attempts = currentAttempts;
      }
      if (error != nullptr) {
        error->clear();
      }
      return true;
    }

    lastError = attemptError;
    if (currentAttempts < maxRetries && iterationDelayMs > 0) {
      delay(iterationDelayMs * currentAttempts);
    }
  }

  if (attempts != nullptr) {
    *attempts = currentAttempts;
  }
  if (error != nullptr) {
    *error = lastError;
  }

  return false;
}

std::optional<hardware::McuProfile> findMcu(quint16 signature,
                                            const QVector<hardware::McuProfile>& mcuList) {
  for (const auto& mcu : mcuList) {
    if (mcu.signature == signature) {
      return mcu;
    }
  }

  return std::nullopt;
}

bool isValidFlash(const QString& mcu, const QByteArray& flash) {
  if (!(mcu.contains(QStringLiteral("#BLHELI#")) || mcu.contains(QStringLiteral("#BLHELI$")))) {
    return false;
  }

  if (flash.size() < 3) {
    return false;
  }

  const QByteArray firstBytes = flash.first(3);
  const QByteArray ljmpReset = QByteArray::fromHex("0219FD");
  const QByteArray ljmpResetBb51 = QByteArray::fromHex("022FFD");
  const QByteArray ljmpCheckBootload = QByteArray::fromHex("0219E0");

  return compare(firstBytes, ljmpReset) || compare(firstBytes, ljmpCheckBootload) ||
         compare(firstBytes, ljmpResetBb51);
}

}  // namespace general
