#pragma once

#include <optional>

#include <QString>

#include "core/FourWay.hpp"

namespace fourway_helper {

std::optional<QString> commandToString(fourway::Command command);
std::optional<QString> ackToString(fourway::Ack ack);

}  // namespace fourway_helper
