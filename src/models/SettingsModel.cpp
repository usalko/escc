#include "SettingsModel.hpp"

#include <algorithm>

namespace models {

SettingsModel::SettingsModel(QObject* parent) : QAbstractListModel(parent) {}

int SettingsModel::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return items_.size();
}

QVariant SettingsModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= items_.size()) {
    return {};
  }

  const Item& item = items_.at(index.row());
  switch (role) {
    case Qt::DisplayRole:
    case KeyRole:
      return item.key;
    case ValueRole:
      return item.value;
    case ScopeRole:
      return item.scope;
    default:
      return {};
  }
}

QHash<int, QByteArray> SettingsModel::roleNames() const {
  return {
      {KeyRole, "key"},
      {ValueRole, "value"},
      {ScopeRole, "scope"},
  };
}

QVariantMap SettingsModel::commonSettings() const { return commonSettings_; }

QVariantMap SettingsModel::individualSettings() const { return individualSettings_; }

void SettingsModel::setCommonSettings(const QVariantMap& settings) {
  if (commonSettings_ == settings) {
    return;
  }

  commonSettings_ = settings;
  rebuildItems();
}

void SettingsModel::setIndividualSettings(const QVariantMap& settings) {
  if (individualSettings_ == settings) {
    return;
  }

  individualSettings_ = settings;
  rebuildItems();
}

void SettingsModel::clear() {
  if (commonSettings_.isEmpty() && individualSettings_.isEmpty()) {
    return;
  }

  commonSettings_.clear();
  individualSettings_.clear();
  rebuildItems();
}

QVariant SettingsModel::valueForKey(const QString& key) const {
  if (individualSettings_.contains(key)) {
    return individualSettings_.value(key);
  }

  return commonSettings_.value(key);
}

bool SettingsModel::setValueForKey(const QString& key, const QVariant& value, bool individual) {
  if (key.trimmed().isEmpty()) {
    return false;
  }

  QVariantMap& target = individual ? individualSettings_ : commonSettings_;
  if (target.contains(key) && target.value(key) == value) {
    return true;
  }

  target.insert(key, value);
  rebuildItems();
  return true;
}

void SettingsModel::rebuildItems() {
  QStringList keys = commonSettings_.keys();
  for (const auto& key : individualSettings_.keys()) {
    if (!keys.contains(key)) {
      keys.push_back(key);
    }
  }

  std::sort(keys.begin(), keys.end());

  QVector<Item> rebuilt;
  rebuilt.reserve(keys.size());
  for (const auto& key : keys) {
    if (individualSettings_.contains(key)) {
      rebuilt.push_back({key, individualSettings_.value(key), QStringLiteral("individual")});
      continue;
    }

    rebuilt.push_back({key, commonSettings_.value(key), QStringLiteral("common")});
  }

  beginResetModel();
  items_ = rebuilt;
  endResetModel();
}

}  // namespace models
