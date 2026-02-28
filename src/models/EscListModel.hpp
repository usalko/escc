#pragma once

#include <QAbstractListModel>
#include <QVector>
#include <QVariantList>

#include "models/Types.hpp"

namespace models {

class EscListModel final : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Role {
    IndexRole = Qt::UserRole + 1,
    NameRole,
    FirmwareNameRole,
    FirmwareVersionRole,
    LayoutRole,
    SelectedRole,
    SettingsRole,
  };

  explicit EscListModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  QVector<EscConfig> escs() const;
  QVariantList toVariantList() const;

  void setEscs(const QVector<EscConfig>& escs);
  void setFromVariantList(const QVariantList& escs);
  bool updateEsc(int row, const EscConfig& esc);
  void clear();

 private:
  static EscConfig fromVariantMap(const QVariantMap& map);
  static QVariantMap toVariantMap(const EscConfig& esc);

  QVector<EscConfig> escs_;
};

}  // namespace models
