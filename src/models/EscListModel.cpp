#include "EscListModel.hpp"

namespace models {

EscListModel::EscListModel(QObject* parent) : QAbstractListModel(parent) {}

int EscListModel::rowCount(const QModelIndex& parent) const {
  if (parent.isValid()) {
    return 0;
  }

  return escs_.size();
}

QVariant EscListModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= escs_.size()) {
    return {};
  }

  const EscConfig& esc = escs_.at(index.row());
  switch (role) {
    case Qt::DisplayRole:
    case NameRole:
      return esc.name;
    case IndexRole:
      return esc.index;
    case FirmwareNameRole:
      return esc.firmwareName;
    case FirmwareVersionRole:
      return esc.firmwareVersion;
    case LayoutRole:
      return esc.layout;
    case SelectedRole:
      return esc.selected;
    case SettingsRole:
      return esc.settings;
    default:
      return {};
  }
}

QHash<int, QByteArray> EscListModel::roleNames() const {
  return {
      {IndexRole, "index"},
      {NameRole, "name"},
      {FirmwareNameRole, "firmwareName"},
      {FirmwareVersionRole, "firmwareVersion"},
      {LayoutRole, "layout"},
      {SelectedRole, "selected"},
      {SettingsRole, "settings"},
  };
}

QVector<EscConfig> EscListModel::escs() const { return escs_; }

QVariantList EscListModel::toVariantList() const {
  QVariantList output;
  output.reserve(escs_.size());

  for (const auto& esc : escs_) {
    output.push_back(toVariantMap(esc));
  }

  return output;
}

void EscListModel::setEscs(const QVector<EscConfig>& escs) {
  beginResetModel();
  escs_ = escs;
  endResetModel();
}

void EscListModel::setFromVariantList(const QVariantList& escs) {
  QVector<EscConfig> parsed;
  parsed.reserve(escs.size());

  for (const auto& item : escs) {
    const QVariantMap map = item.toMap();
    const EscConfig esc = fromVariantMap(map);
    if (!esc.isValid()) {
      continue;
    }
    parsed.push_back(esc);
  }

  setEscs(parsed);
}

bool EscListModel::updateEsc(int row, const EscConfig& esc) {
  if (row < 0 || row >= escs_.size() || !esc.isValid()) {
    return false;
  }

  escs_[row] = esc;
  const QModelIndex itemIndex = index(row, 0);
  emit dataChanged(itemIndex, itemIndex);
  return true;
}

void EscListModel::clear() {
  if (escs_.isEmpty()) {
    return;
  }

  beginResetModel();
  escs_.clear();
  endResetModel();
}

EscConfig EscListModel::fromVariantMap(const QVariantMap& map) {
  EscConfig esc;
  esc.index = map.contains(QStringLiteral("index"))
                  ? map.value(QStringLiteral("index")).toInt()
                  : -1;
  esc.name = map.value(QStringLiteral("name")).toString();
  esc.firmwareName = map.value(QStringLiteral("firmwareName")).toString();
  esc.firmwareVersion = map.value(QStringLiteral("firmwareVersion")).toString();
  esc.layout = map.value(QStringLiteral("layout")).toString();
  esc.selected = map.value(QStringLiteral("selected"), true).toBool();
  esc.settings = map.value(QStringLiteral("settings")).toMap();
  return esc;
}

QVariantMap EscListModel::toVariantMap(const EscConfig& esc) {
  return {
      {QStringLiteral("index"), esc.index},
      {QStringLiteral("name"), esc.name},
      {QStringLiteral("firmwareName"), esc.firmwareName},
      {QStringLiteral("firmwareVersion"), esc.firmwareVersion},
      {QStringLiteral("layout"), esc.layout},
      {QStringLiteral("selected"), esc.selected},
      {QStringLiteral("settings"), esc.settings},
  };
}

}  // namespace models
