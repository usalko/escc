#pragma once

#include <functional>
#include <optional>

#include <QByteArray>
#include <QVector>

#include "hardware/Mcu.hpp"

namespace general {

bool compare(const QByteArray& a, const QByteArray& b);
void delay(int ms);

using RetryOperation = std::function<bool(QString* error)>;
bool retry(const RetryOperation& operation,
           int maxRetries,
           int iterationDelayMs = 0,
           QString* error = nullptr,
           int* attempts = nullptr);

std::optional<hardware::McuProfile> findMcu(quint16 signature,
                                            const QVector<hardware::McuProfile>& mcuList);
bool isValidFlash(const QString& mcu, const QByteArray& flash);

}  // namespace general
