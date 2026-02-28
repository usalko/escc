#include "FourWay.hpp"

#include <QThread>

#include "hardware/Hardware.hpp"
#include "hardware/Mcu.hpp"

namespace fourway {

quint16 crc16XmodemUpdate(quint16 crc, quint8 byte) {
	constexpr quint16 polynomial = 0x1021;
	crc ^= static_cast<quint16>(byte) << 8;
	for (int i = 0; i < 8; ++i) {
		if ((crc & 0x8000U) != 0) {
			crc = static_cast<quint16>((crc << 1U) ^ polynomial);
		} else {
			crc = static_cast<quint16>(crc << 1U);
		}
	}

	return crc;
}

quint16 crc16Xmodem(const QByteArray& bytes) {
	quint16 crc = 0;
	for (const auto& rawByte : bytes) {
		crc = crc16XmodemUpdate(crc, static_cast<quint8>(rawByte));
	}
	return crc;
}

QByteArray createMessage(Command command, QByteArray params, quint16 address) {
	if (params.isEmpty()) {
		params.append('\0');
	}

	const int payloadLength = params.size();
	const quint8 encodedLength = payloadLength == 256 ? 0 : static_cast<quint8>(payloadLength);

	QByteArray frame;
	frame.reserve(7 + payloadLength);
	frame.append(static_cast<char>(kHostStartByte));
	frame.append(static_cast<char>(command));
	frame.append(static_cast<char>((address >> 8) & 0xFF));
	frame.append(static_cast<char>(address & 0xFF));
	frame.append(static_cast<char>(encodedLength));
	frame.append(params);

	const quint16 checksum = crc16Xmodem(frame);
	frame.append(static_cast<char>((checksum >> 8) & 0xFF));
	frame.append(static_cast<char>(checksum & 0xFF));

	return frame;
}

ParseStatus parseMessage(const QByteArray& bytes, Response* outResponse, int* consumedBytes) {
	if (consumedBytes != nullptr) {
		*consumedBytes = 0;
	}

	if (outResponse == nullptr) {
		return ParseStatus::NotEnoughData;
	}

	if (bytes.size() < 9) {
		return ParseStatus::NotEnoughData;
	}

	if (static_cast<quint8>(bytes.at(0)) != kDeviceStartByte) {
		return ParseStatus::InvalidStart;
	}

	int paramCount = static_cast<quint8>(bytes.at(4));
	if (paramCount == 0) {
		paramCount = 256;
	}

	const int frameSize = 8 + paramCount;
	if (bytes.size() < frameSize) {
		return ParseStatus::NotEnoughData;
	}

	Response response;
	response.command = static_cast<quint8>(bytes.at(1));
	response.address = static_cast<quint16>(static_cast<quint8>(bytes.at(2)) << 8) |
										 static_cast<quint8>(bytes.at(3));
	response.params = bytes.mid(5, paramCount);
	response.ack = static_cast<quint8>(bytes.at(5 + paramCount));
	response.checksum = static_cast<quint16>(static_cast<quint8>(bytes.at(6 + paramCount)) << 8) |
											static_cast<quint8>(bytes.at(7 + paramCount));

	const QByteArray forChecksum = bytes.left(6 + paramCount);
	const quint16 expected = crc16Xmodem(forChecksum);
	if (expected != response.checksum) {
		return ParseStatus::InvalidChecksum;
	}

	*outResponse = response;
	if (consumedBytes != nullptr) {
		*consumedBytes = frameSize;
	}

	return ParseStatus::Ok;
}

}  // namespace fourway

FourWay::FourWay(QObject* parent) : QObject(parent) {}

void FourWay::setTransport(Transport transport) {
	transport_ = std::move(transport);
}

QByteArray FourWay::buildInterfaceTestAlive(quint16 address) {
	return fourway::createMessage(fourway::Command::InterfaceTestAlive, {}, address);
}

QByteArray FourWay::buildProtocolGetVersion(quint16 address) {
	return fourway::createMessage(fourway::Command::ProtocolGetVersion, {}, address);
}

QByteArray FourWay::buildInterfaceGetVersion(quint16 address) {
	return fourway::createMessage(fourway::Command::InterfaceGetVersion, {}, address);
}

QByteArray FourWay::buildInterfaceExit(quint16 address) {
	return fourway::createMessage(fourway::Command::InterfaceExit, {}, address);
}

QByteArray FourWay::buildSetMode(fourway::Mode mode, quint16 address) {
	QByteArray params;
	params.append(static_cast<char>(mode));
	return fourway::createMessage(fourway::Command::InterfaceSetMode, params, address);
}

bool FourWay::isAckOk(const fourway::Response& response) {
	return response.ack == static_cast<quint8>(fourway::Ack::Ok);
}

bool FourWay::responseMatchesCommand(const fourway::Response& response, fourway::Command command) {
	return response.command == static_cast<quint8>(command);
}

bool FourWay::validateOkResponse(const fourway::Response& response,
																 fourway::Command expectedCommand,
																 QString* error) {
	if (!responseMatchesCommand(response, expectedCommand)) {
		if (error != nullptr) {
			*error = tr("Response command mismatch");
		}
		return false;
	}

	if (!isAckOk(response)) {
		if (error != nullptr) {
			*error = tr("Response ACK is not OK");
		}
		return false;
	}

	return true;
}

std::optional<quint16> FourWay::protocolVersionFromResponse(const fourway::Response& response) {
	if (response.params.isEmpty()) {
		return std::nullopt;
	}

	if (response.params.size() == 1) {
		return static_cast<quint8>(response.params.at(0));
	}

	return static_cast<quint16>(static_cast<quint8>(response.params.at(1)) << 8) |
				 static_cast<quint8>(response.params.at(0));
}

std::optional<quint16> FourWay::interfaceVersionFromResponse(const fourway::Response& response) {
	return protocolVersionFromResponse(response);
}

std::optional<fourway::Response> FourWay::exchange(fourway::Command command,
																									 const QByteArray& params,
																									 quint16 address,
																									 bool expectResponse,
																									 QString* error) const {
	if (!transport_) {
		if (error != nullptr) {
			*error = tr("FourWay transport is not set");
		}
		return std::nullopt;
	}

	const QByteArray request = fourway::createMessage(command, params, address);

	if (!expectResponse) {
		transport_(request);
		fourway::Response synthetic;
		synthetic.command = static_cast<quint8>(command);
		synthetic.address = address;
		synthetic.ack = static_cast<quint8>(fourway::Ack::Ok);
		return synthetic;
	}

	const QByteArray rawResponse = transport_(request);
	fourway::Response parsed;
	int consumed = 0;
	const auto parseStatus = fourway::parseMessage(rawResponse, &parsed, &consumed);
	Q_UNUSED(consumed);

	if (parseStatus != fourway::ParseStatus::Ok) {
		if (error != nullptr) {
			*error = tr("Failed to parse FourWay response");
		}
		return std::nullopt;
	}

	if (!validateOkResponse(parsed, command, error)) {
		return std::nullopt;
	}

	return parsed;
}

bool FourWay::testAlive(QString* error) const {
	const auto response = exchange(fourway::Command::InterfaceTestAlive, {}, 0, true, error);
	return response.has_value();
}

std::optional<quint16> FourWay::getProtocolVersion(QString* error) const {
	const auto response = exchange(fourway::Command::ProtocolGetVersion, {}, 0, true, error);
	if (!response.has_value()) {
		return std::nullopt;
	}

	const auto version = protocolVersionFromResponse(response.value());
	if (!version.has_value() && error != nullptr) {
		*error = tr("Protocol version payload is empty");
	}

	return version;
}

std::optional<fourway::Response> FourWay::initFlash(quint8 target, QString* error) const {
	QByteArray params;
	params.append(static_cast<char>(target));
	return exchange(fourway::Command::DeviceInitFlash, params, 0, true, error);
}

std::optional<fourway::InitFlashInfo> FourWay::initFlashInfo(quint8 target, QString* error) const {
	const auto response = initFlash(target, error);
	if (!response.has_value()) {
		return std::nullopt;
	}

	if (response->params.size() < 4) {
		if (error != nullptr) {
			*error = tr("InitFlash response is too short");
		}
		return std::nullopt;
	}

	fourway::InitFlashInfo info;
	info.target = target;
	info.signature = static_cast<quint16>(static_cast<quint8>(response->params.at(1)) << 8) |
									 static_cast<quint8>(response->params.at(0));
	info.interfaceMode = static_cast<quint8>(response->params.at(3));
	info.rawParams = response->params;
	return info;
}

bool FourWay::reset(quint8 target, QString* error) const {
	QByteArray params;
	params.append(static_cast<char>(target));
	const auto response = exchange(fourway::Command::DeviceReset, params, 0, true, error);
	return response.has_value();
}

std::optional<QByteArray> FourWay::read(quint16 address, int bytes, QString* error) const {
	if (bytes <= 0 || bytes > 256) {
		if (error != nullptr) {
			*error = tr("Read byte count must be in range 1..256");
		}
		return std::nullopt;
	}

	QByteArray params;
	params.append(static_cast<char>(bytes == 256 ? 0 : bytes));
	const auto response = exchange(fourway::Command::DeviceRead, params, address, true, error);
	if (!response.has_value()) {
		return std::nullopt;
	}

	return response->params;
}

std::optional<QByteArray> FourWay::readEeprom(quint16 address, int bytes, QString* error) const {
	if (bytes <= 0 || bytes > 256) {
		if (error != nullptr) {
			*error = tr("Read byte count must be in range 1..256");
		}
		return std::nullopt;
	}

	QByteArray params;
	params.append(static_cast<char>(bytes == 256 ? 0 : bytes));
	const auto response = exchange(fourway::Command::DeviceReadEEprom, params, address, true, error);
	if (!response.has_value()) {
		return std::nullopt;
	}

	return response->params;
}

bool FourWay::write(quint16 address, const QByteArray& data, QString* error) const {
	if (data.isEmpty() || data.size() > 256) {
		if (error != nullptr) {
			*error = tr("Write payload size must be in range 1..256");
		}
		return false;
	}

	const auto response = exchange(fourway::Command::DeviceWrite, data, address, true, error);
	return response.has_value();
}

bool FourWay::writeEeprom(quint16 address, const QByteArray& data, QString* error) const {
	if (data.isEmpty() || data.size() > 256) {
		if (error != nullptr) {
			*error = tr("Write payload size must be in range 1..256");
		}
		return false;
	}

	const auto response = exchange(fourway::Command::DeviceWriteEEprom, data, address, true, error);
	return response.has_value();
}

std::optional<QByteArray> FourWay::readSettings(quint8 target,
																								quint16 eepromAddress,
																								int length,
																								QString* error) const {
	if (length <= 0) {
		if (error != nullptr) {
			*error = tr("Settings length must be positive");
		}
		return std::nullopt;
	}

	const auto info = initFlashInfo(target, error);
	if (!info.has_value()) {
		return std::nullopt;
	}

	const auto mode = static_cast<fourway::Mode>(info->interfaceMode);
	const bool useEepromCommands = fourway::isAtmelMode(mode);

	QByteArray output;
	output.reserve(length);

	int offset = 0;
	while (offset < length) {
		const int chunk = qMin(256, length - offset);
		std::optional<QByteArray> bytes;
		if (useEepromCommands) {
			bytes = readEeprom(static_cast<quint16>(eepromAddress + offset), chunk, error);
		} else {
			bytes = read(static_cast<quint16>(eepromAddress + offset), chunk, error);
		}

		if (!bytes.has_value()) {
			return std::nullopt;
		}

		output.append(bytes.value());
		offset += chunk;
	}

	return output;
}

bool FourWay::writeSettings(quint8 target,
														quint16 eepromAddress,
														const QByteArray& settings,
														QString* error) const {
	if (settings.isEmpty()) {
		if (error != nullptr) {
			*error = tr("Settings payload is empty");
		}
		return false;
	}

	const auto info = initFlashInfo(target, error);
	if (!info.has_value()) {
		return false;
	}

	const auto mode = static_cast<fourway::Mode>(info->interfaceMode);
	const bool useEepromCommands = fourway::isAtmelMode(mode);

	int offset = 0;
	while (offset < settings.size()) {
		const int chunk = qMin(256, settings.size() - offset);
		const QByteArray part = settings.mid(offset, chunk);
		const bool ok = useEepromCommands
												? writeEeprom(static_cast<quint16>(eepromAddress + offset), part, error)
												: write(static_cast<quint16>(eepromAddress + offset), part, error);
		if (!ok) {
			return false;
		}

		offset += chunk;
	}

	const auto readBack = readSettings(target, eepromAddress, settings.size(), error);
	if (!readBack.has_value()) {
		return false;
	}

	if (readBack.value() != settings) {
		if (error != nullptr) {
			*error = tr("Settings verification failed after write");
		}
		return false;
	}

	return true;
}

std::optional<QByteArray> FourWay::readSettingsAuto(quint8 target,
																										int length,
																										QString* error) const {
	const auto info = initFlashInfo(target, error);
	if (!info.has_value()) {
		return std::nullopt;
	}

	const auto profile = hardware::findMcuProfile(info->interfaceMode, info->signature);
	if (!profile.has_value() || !profile->eepromOffset.has_value()) {
		if (error != nullptr) {
			*error = tr("Unable to resolve MCU EEPROM offset for target");
		}
		return std::nullopt;
	}

	return readSettings(target, static_cast<quint16>(profile->eepromOffset.value()), length, error);
}

bool FourWay::writeSettingsAuto(quint8 target,
																const QByteArray& settings,
																QString* error) const {
	const auto info = initFlashInfo(target, error);
	if (!info.has_value()) {
		return false;
	}

	const auto profile = hardware::findMcuProfile(info->interfaceMode, info->signature);
	if (!profile.has_value() || !profile->eepromOffset.has_value()) {
		if (error != nullptr) {
			*error = tr("Unable to resolve MCU EEPROM offset for target");
		}
		return false;
	}

	return writeSettings(target, static_cast<quint16>(profile->eepromOffset.value()), settings, error);
}

bool FourWay::flashImage(quint8 target,
												 const QByteArray& image,
												 int startPage,
												 int stopPage,
												 QString* error) const {
	if (image.isEmpty()) {
		if (error != nullptr) {
			*error = tr("Flash image is empty");
		}
		return false;
	}

	if (startPage < 0 || stopPage <= startPage) {
		if (error != nullptr) {
			*error = tr("Invalid flash page range");
		}
		return false;
	}

	const auto info = initFlashInfo(target, error);
	if (!info.has_value()) {
		return false;
	}

	const auto profile = hardware::findMcuProfile(info->interfaceMode, info->signature);
	if (!profile.has_value()) {
		if (error != nullptr) {
			*error = tr("Unable to resolve MCU profile for flashing");
		}
		return false;
	}

	const auto hardware = hardware::createHardware(info->interfaceMode);
	if (!hardware) {
		if (error != nullptr) {
			*error = tr("No hardware implementation for interface mode");
		}
		return false;
	}

	const int requiredSize = stopPage * profile->pageSize;
	if (requiredSize > image.size()) {
		if (error != nullptr) {
			*error = tr("Flash image is smaller than requested page range");
		}
		return false;
	}

	if (!hardware->erasePages(*this, startPage, stopPage, profile.value(), error)) {
		return false;
	}

	if (!hardware->writePages(*this,
														startPage,
														stopPage,
														profile->pageSize,
														image,
														profile.value(),
														error)) {
		return false;
	}

	if (!hardware->verifyPages(*this,
														 startPage,
														 stopPage,
														 profile->pageSize,
														 image,
														 profile.value(),
														 error)) {
		return false;
	}

	return true;
}

bool FourWay::flashImageModeAware(quint8 target,
																	const QByteArray& image,
																	QString* error) const {
	const auto info = initFlashInfo(target, error);
	if (!info.has_value()) {
		return false;
	}

	const auto profile = hardware::findMcuProfile(info->interfaceMode, info->signature);
	if (!profile.has_value()) {
		if (error != nullptr) {
			*error = tr("Unable to resolve MCU profile for mode-aware flashing");
		}
		return false;
	}

	const auto hw = hardware::createHardware(info->interfaceMode);
	if (!hw) {
		if (error != nullptr) {
			*error = tr("No hardware implementation for interface mode");
		}
		return false;
	}

	if (info->interfaceMode == static_cast<quint8>(fourway::Mode::ARMBLB)) {
		constexpr int kStartPage = 0x04;
		constexpr int kStopPage = 0x40;
		constexpr unsigned long kResetDelayMs = 5000;

		const int required = kStopPage * profile->pageSize;
		if (image.size() < required) {
			if (error != nullptr) {
				*error = tr("ARM image is smaller than required flashing range");
			}
			return false;
		}

		if (!hw->erasePages(*this, kStartPage, kStopPage, profile.value(), error)) {
			return false;
		}
		if (!hw->writePages(*this,
												kStartPage,
												kStopPage,
												profile->pageSize,
												image,
												profile.value(),
												error)) {
			return false;
		}
		if (!hw->verifyPages(*this,
												 kStartPage,
												 kStopPage,
												 profile->pageSize,
												 image,
												 profile.value(),
												 error)) {
			return false;
		}

		if (!reset(target, error)) {
			return false;
		}

		QThread::msleep(kResetDelayMs);
		return true;
	}

	if (info->interfaceMode == static_cast<quint8>(fourway::Mode::SiLBLB) &&
			profile->pageSize == 512) {
		const int required = 0x0E * profile->pageSize;
		if (image.size() < required) {
			if (error != nullptr) {
				*error = tr("SiLabs image is smaller than required flashing range");
			}
			return false;
		}

		if (!hw->erasePages(*this, 0x02, 0x0D, profile.value(), error)) {
			return false;
		}
		if (!hw->writePages(*this,
												0x02,
												0x0D,
												profile->pageSize,
												image,
												profile.value(),
												error)) {
			return false;
		}
		if (!hw->verifyPages(*this,
												 0x02,
												 0x0D,
												 profile->pageSize,
												 image,
												 profile.value(),
												 error)) {
			return false;
		}

		if (!hw->erasePages(*this, 0x00, 0x02, profile.value(), error)) {
			return false;
		}
		if (!hw->writePages(*this,
												0x00,
												0x02,
												profile->pageSize,
												image,
												profile.value(),
												error)) {
			return false;
		}
		if (!hw->verifyPages(*this,
												 0x00,
												 0x02,
												 profile->pageSize,
												 image,
												 profile.value(),
												 error)) {
			return false;
		}

		if (!hw->erasePages(*this, 0x0D, 0x0E, profile.value(), error)) {
			return false;
		}
		if (!hw->writePages(*this,
												0x0D,
												0x0E,
												profile->pageSize,
												image,
												profile.value(),
												error)) {
			return false;
		}
		if (!hw->verifyPages(*this,
												 0x0D,
												 0x0E,
												 profile->pageSize,
												 image,
												 profile.value(),
												 error)) {
			return false;
		}

		return true;
	}

	const int stopPage = image.size() / profile->pageSize;
	if (stopPage <= 0) {
		if (error != nullptr) {
			*error = tr("Image does not contain full pages for generic flash path");
		}
		return false;
	}

	return flashImage(target, image, 0, stopPage, error);
}
