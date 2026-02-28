#pragma once

#include <QByteArray>
#include <QString>
#include <QVector>
#include <QWidget>

#include <memory>

class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QTemporaryFile;

#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
class QAudioOutput;
class QMediaPlayer;
#endif

class MelodyEditorWidget final : public QWidget {
  Q_OBJECT

 public:
  explicit MelodyEditorWidget(QWidget* parent = nullptr);

 private slots:
  void loadBuiltinMelodies();
  void applySelectedPreset();
  void loadFromFile();
  void saveToFile();
  void previewMelody();
  void stopPreview();

 private:
  struct BuiltinMelody {
    QString name;
    QString track;
  };

  void setStatusMessage(const QString& message);
  static QByteArray renderRtttlToWav(const QString& rtttl, QString* error);

  QComboBox* presetCombo_;
  QPlainTextEdit* rtttlEditor_;
  QLabel* statusLabel_;
  QPushButton* previewButton_;
  QPushButton* stopButton_;
  QVector<BuiltinMelody> builtinMelodies_;

#if defined(ESCC_HAS_QT_MULTIMEDIA) && ESCC_HAS_QT_MULTIMEDIA
  QMediaPlayer* player_;
  QAudioOutput* audioOutput_;
  std::unique_ptr<QTemporaryFile> previewFile_;
#endif
};
