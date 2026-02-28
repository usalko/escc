#include "MelodyEditorWidget.hpp"

#include <QtMath>

#include <QByteArray>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTemporaryFile>
#include <QUrl>
#include <QVBoxLayout>

#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
#include <QAudioOutput>
#include <QMediaPlayer>
#endif

namespace {

QByteArray littleEndian16(quint16 value) {
  QByteArray out(2, '\0');
  out[0] = static_cast<char>(value & 0xFF);
  out[1] = static_cast<char>((value >> 8) & 0xFF);
  return out;
}

QByteArray littleEndian32(quint32 value) {
  QByteArray out(4, '\0');
  out[0] = static_cast<char>(value & 0xFF);
  out[1] = static_cast<char>((value >> 8) & 0xFF);
  out[2] = static_cast<char>((value >> 16) & 0xFF);
  out[3] = static_cast<char>((value >> 24) & 0xFF);
  return out;
}

int semitoneFromNote(const QString& note) {
  if (note == QStringLiteral("c")) return 0;
  if (note == QStringLiteral("c#")) return 1;
  if (note == QStringLiteral("d")) return 2;
  if (note == QStringLiteral("d#")) return 3;
  if (note == QStringLiteral("e")) return 4;
  if (note == QStringLiteral("f")) return 5;
  if (note == QStringLiteral("f#")) return 6;
  if (note == QStringLiteral("g")) return 7;
  if (note == QStringLiteral("g#")) return 8;
  if (note == QStringLiteral("a")) return 9;
  if (note == QStringLiteral("a#")) return 10;
  if (note == QStringLiteral("b")) return 11;
  return -1;
}

}  // namespace

MelodyEditorWidget::MelodyEditorWidget(QWidget* parent)
    : QWidget(parent),
      presetCombo_(new QComboBox(this)),
      rtttlEditor_(new QPlainTextEdit(this)),
      statusLabel_(new QLabel(this)),
      previewButton_(new QPushButton(tr("Preview"), this)),
      stopButton_(new QPushButton(tr("Stop"), this))
#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
      ,
      player_(new QMediaPlayer(this)),
      audioOutput_(new QAudioOutput(this))
#endif
{
  setObjectName(QStringLiteral("melodyEditorWidget"));

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(16, 16, 16, 16);
  root->setSpacing(10);

  auto* title = new QLabel(tr("Melody Editor"), this);
  title->setObjectName(QStringLiteral("homeTitle"));

  auto* form = new QFormLayout();
  form->addRow(tr("Preset"), presetCombo_);

  auto* fileButtons = new QHBoxLayout();
  auto* loadButton = new QPushButton(tr("Load RTTTL"), this);
  auto* saveButton = new QPushButton(tr("Save RTTTL"), this);
  fileButtons->addWidget(loadButton);
  fileButtons->addWidget(saveButton);
  fileButtons->addStretch(1);

  auto* previewButtons = new QHBoxLayout();
  previewButtons->addWidget(previewButton_);
  previewButtons->addWidget(stopButton_);
  previewButtons->addStretch(1);

  rtttlEditor_->setPlaceholderText(tr("Enter RTTTL melody, e.g. name:d=4,o=5,b=140:c,e,g"));
  statusLabel_->setWordWrap(true);

  root->addWidget(title);
  root->addLayout(form);
  root->addLayout(fileButtons);
  root->addWidget(rtttlEditor_, 1);
  root->addLayout(previewButtons);
  root->addWidget(statusLabel_);

#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
  player_->setAudioOutput(audioOutput_);
#else
  previewButton_->setEnabled(false);
  stopButton_->setEnabled(false);
  setStatusMessage(tr("Preview unavailable: Qt Multimedia module is not enabled in this build."));
#endif

  connect(presetCombo_, &QComboBox::currentIndexChanged, this, &MelodyEditorWidget::applySelectedPreset);
  connect(loadButton, &QPushButton::clicked, this, &MelodyEditorWidget::loadFromFile);
  connect(saveButton, &QPushButton::clicked, this, &MelodyEditorWidget::saveToFile);
  connect(previewButton_, &QPushButton::clicked, this, &MelodyEditorWidget::previewMelody);
  connect(stopButton_, &QPushButton::clicked, this, &MelodyEditorWidget::stopPreview);

  loadBuiltinMelodies();
}

void MelodyEditorWidget::loadBuiltinMelodies() {
  presetCombo_->clear();
  builtinMelodies_.clear();

  QFile file(QStringLiteral(":/data/melodies.json"));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    setStatusMessage(tr("Failed to load built-in melody library."));
    return;
  }

  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
    setStatusMessage(tr("Invalid built-in melody library format."));
    return;
  }

  const QJsonArray melodies = doc.array();
  for (const auto& item : melodies) {
    if (!item.isObject()) {
      continue;
    }

    const QJsonObject obj = item.toObject();
    const QString name = obj.value(QStringLiteral("name")).toString();
    const QJsonArray tracks = obj.value(QStringLiteral("tracks")).toArray();
    if (name.isEmpty() || tracks.isEmpty()) {
      continue;
    }

    const QString firstTrack = tracks.at(0).toString();
    if (firstTrack.isEmpty()) {
      continue;
    }

    builtinMelodies_.push_back({name, firstTrack});
    presetCombo_->addItem(name);
  }

  if (!builtinMelodies_.isEmpty()) {
    presetCombo_->setCurrentIndex(0);
    applySelectedPreset();
    setStatusMessage(tr("Loaded %1 built-in melodies.").arg(builtinMelodies_.size()));
  } else {
    setStatusMessage(tr("Built-in melody library is empty."));
  }
}

void MelodyEditorWidget::applySelectedPreset() {
  const int index = presetCombo_->currentIndex();
  if (index < 0 || index >= builtinMelodies_.size()) {
    return;
  }

  rtttlEditor_->setPlainText(builtinMelodies_.at(index).track);
}

void MelodyEditorWidget::loadFromFile() {
  const QString path = QFileDialog::getOpenFileName(this,
                                                    tr("Open RTTTL file"),
                                                    QString(),
                                                    tr("RTTTL files (*.rtttl *.txt);;All files (*.*)"));
  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    setStatusMessage(tr("Failed to open file: %1").arg(path));
    return;
  }

  rtttlEditor_->setPlainText(QString::fromUtf8(file.readAll()));
  setStatusMessage(tr("Loaded RTTTL from %1").arg(path));
}

void MelodyEditorWidget::saveToFile() {
  const QString path = QFileDialog::getSaveFileName(this,
                                                    tr("Save RTTTL file"),
                                                    QStringLiteral("melody.rtttl"),
                                                    tr("RTTTL files (*.rtttl *.txt);;All files (*.*)"));
  if (path.isEmpty()) {
    return;
  }

  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
    setStatusMessage(tr("Failed to save file: %1").arg(path));
    return;
  }

  file.write(rtttlEditor_->toPlainText().toUtf8());
  setStatusMessage(tr("Saved RTTTL to %1").arg(path));
}

void MelodyEditorWidget::previewMelody() {
#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
  QString error;
  const QByteArray wav = renderRtttlToWav(rtttlEditor_->toPlainText().trimmed(), &error);
  if (wav.isEmpty()) {
    setStatusMessage(error.isEmpty() ? tr("Failed to generate melody preview") : error);
    return;
  }

  previewFile_ = std::make_unique<QTemporaryFile>(QStringLiteral("/tmp/escc-melody-XXXXXX.wav"));
  previewFile_->setAutoRemove(true);
  if (!previewFile_->open()) {
    setStatusMessage(tr("Failed to create temporary audio preview file."));
    previewFile_.reset();
    return;
  }

  previewFile_->write(wav);
  previewFile_->flush();

  player_->stop();
  player_->setSource(QUrl::fromLocalFile(previewFile_->fileName()));
  player_->play();
  setStatusMessage(tr("Preview started"));
#else
  setStatusMessage(tr("Preview unavailable: Qt Multimedia module is not enabled."));
#endif
}

void MelodyEditorWidget::stopPreview() {
#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
  player_->stop();
  setStatusMessage(tr("Preview stopped"));
#else
  setStatusMessage(tr("Preview unavailable: Qt Multimedia module is not enabled."));
#endif
}

void MelodyEditorWidget::setStatusMessage(const QString& message) {
  statusLabel_->setText(message);
}

QByteArray MelodyEditorWidget::renderRtttlToWav(const QString& rtttl, QString* error) {
  constexpr double kPi = 3.14159265358979323846;
  const int sampleRate = 22050;
  const int amplitude = 12000;

  const QStringList parts = rtttl.split(':');
  if (parts.size() < 3) {
    if (error) {
      *error = tr("Invalid RTTTL: expected format name:d=,o=,b=:notes");
    }
    return {};
  }

  int defaultDuration = 4;
  int defaultOctave = 5;
  int bpm = 140;

  const QStringList defaults = parts.at(1).split(',', Qt::SkipEmptyParts);
  for (const auto& item : defaults) {
    const QString token = item.trimmed();
    if (token.startsWith(QStringLiteral("d="))) {
      defaultDuration = token.mid(2).toInt();
    } else if (token.startsWith(QStringLiteral("o="))) {
      defaultOctave = token.mid(2).toInt();
    } else if (token.startsWith(QStringLiteral("b="))) {
      bpm = token.mid(2).toInt();
    }
  }

  if (defaultDuration <= 0 || bpm <= 0) {
    if (error) {
      *error = tr("Invalid RTTTL defaults (duration/bpm)");
    }
    return {};
  }

  const double wholeNoteMs = (60.0 * 1000.0 * 4.0) / static_cast<double>(bpm);
  const QStringList notes = parts.mid(2).join(QStringLiteral(":"))
                                .split(',', Qt::SkipEmptyParts);
  if (notes.isEmpty()) {
    if (error) {
      *error = tr("No notes to preview");
    }
    return {};
  }

  QByteArray pcm;
  pcm.reserve(sampleRate * 2 * 8);

  for (const auto& raw : notes) {
    QString token = raw.trimmed().toLower();
    if (token.isEmpty()) {
      continue;
    }

    int cursor = 0;
    QString number;
    while (cursor < token.size() && token.at(cursor).isDigit()) {
      number.push_back(token.at(cursor));
      ++cursor;
    }

    int duration = number.isEmpty() ? defaultDuration : number.toInt();
    if (duration <= 0) {
      duration = defaultDuration;
    }

    bool pause = false;
    QString noteName;
    if (cursor < token.size() && token.at(cursor) == QLatin1Char('p')) {
      pause = true;
      noteName = QStringLiteral("p");
      ++cursor;
    } else if (cursor < token.size()) {
      noteName.push_back(token.at(cursor));
      ++cursor;
      if (cursor < token.size() && token.at(cursor) == QLatin1Char('#')) {
        noteName.push_back(token.at(cursor));
        ++cursor;
      }
    }

    int octave = defaultOctave;
    if (cursor < token.size() && token.at(cursor).isDigit()) {
      octave = token.at(cursor).digitValue();
      ++cursor;
    }

    bool dotted = (cursor < token.size() && token.at(cursor) == QLatin1Char('.'));

    double noteMs = wholeNoteMs / static_cast<double>(duration);
    if (dotted) {
      noteMs *= 1.5;
    }

    int noteSamples = qMax(1, static_cast<int>((noteMs / 1000.0) * sampleRate));
    int toneSamples = pause ? 0 : static_cast<int>(noteSamples * 0.9);

    double frequency = 0.0;
    if (!pause) {
      const int semitone = semitoneFromNote(noteName);
      if (semitone < 0) {
        continue;
      }
      const int midi = (octave + 1) * 12 + semitone;
      frequency = 440.0 * qPow(2.0, (midi - 69) / 12.0);
    }

    for (int i = 0; i < noteSamples; ++i) {
      qint16 sample = 0;
      if (i < toneSamples && frequency > 0.0) {
        const double t = static_cast<double>(i) / static_cast<double>(sampleRate);
        sample = static_cast<qint16>(amplitude * qSin(2.0 * kPi * frequency * t));
      }
      pcm.append(static_cast<char>(sample & 0xFF));
      pcm.append(static_cast<char>((sample >> 8) & 0xFF));
    }
  }

  if (pcm.isEmpty()) {
    if (error) {
      *error = tr("Failed to synthesize melody samples");
    }
    return {};
  }

  const quint16 channels = 1;
  const quint16 bitsPerSample = 16;
  const quint32 byteRate = sampleRate * channels * (bitsPerSample / 8);
  const quint16 blockAlign = channels * (bitsPerSample / 8);
  const quint32 dataSize = static_cast<quint32>(pcm.size());
  const quint32 riffSize = 36 + dataSize;

  QByteArray wav;
  wav.reserve(static_cast<int>(44 + dataSize));
  wav.append("RIFF", 4);
  wav.append(littleEndian32(riffSize));
  wav.append("WAVE", 4);
  wav.append("fmt ", 4);
  wav.append(littleEndian32(16));
  wav.append(littleEndian16(1));
  wav.append(littleEndian16(channels));
  wav.append(littleEndian32(sampleRate));
  wav.append(littleEndian32(byteRate));
  wav.append(littleEndian16(blockAlign));
  wav.append(littleEndian16(bitsPerSample));
  wav.append("data", 4);
  wav.append(littleEndian32(dataSize));
  wav.append(pcm);

  return wav;
}
