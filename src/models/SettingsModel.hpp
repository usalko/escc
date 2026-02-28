#pragma once

#include <QAbstractListModel>
#include <QVariantMap>
#include <QVector>

namespace models {

class SettingsModel final : public QAbstractListModel {
  Q_OBJECT

 public:
  enum Role {
    KeyRole = Qt::UserRole + 1,
    ValueRole,
    ScopeRole,
  };

  explicit SettingsModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  QHash<int, QByteArray> roleNames() const override;

  QVariantMap commonSettings() const;
  QVariantMap individualSettings() const;

  void setCommonSettings(const QVariantMap& settings);
  void setIndividualSettings(const QVariantMap& settings);
  void clear();

  QVariant valueForKey(const QString& key) const;
  bool setValueForKey(const QString& key, const QVariant& value, bool individual);

 private:
  struct Item {
    QString key;
    QVariant value;
    QString scope;
  };

  void rebuildItems();

  QVariantMap commonSettings_;
  QVariantMap individualSettings_;
  QVector<Item> items_;
};

}  // namespace models
