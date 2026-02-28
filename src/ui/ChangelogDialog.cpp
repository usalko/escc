#include "ChangelogDialog.hpp"

#include <QDialogButtonBox>
#include <QFile>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

ChangelogDialog::ChangelogDialog(QWidget* parent)
    : QDialog(parent), tree_(new QTreeWidget(this)) {
  setObjectName(QStringLiteral("changelogDialog"));
  setWindowTitle(tr("Changelog"));
  resize(760, 520);

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(12, 12, 12, 12);
  root->setSpacing(10);

  auto* title = new QLabel(tr("Release history"), this);
  title->setObjectName(QStringLiteral("homeSectionTitle"));

  tree_->setHeaderHidden(true);
  tree_->setRootIsDecorated(true);
  tree_->setAlternatingRowColors(false);
  tree_->header()->setSectionResizeMode(QHeaderView::Stretch);

  auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  root->addWidget(title);
  root->addWidget(tree_, 1);
  root->addWidget(buttonBox);

  loadEntries();
}

void ChangelogDialog::loadEntries() {
  tree_->clear();

  QFile file(QStringLiteral(":/data/changelog.json"));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    auto* item = new QTreeWidgetItem(tree_);
    item->setText(0, tr("Failed to load changelog resource"));
    return;
  }

  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
    auto* item = new QTreeWidgetItem(tree_);
    item->setText(0, tr("Invalid changelog format"));
    return;
  }

  const QJsonArray versions = doc.array();
  for (int i = 0; i < versions.size(); ++i) {
    const QJsonObject versionObj = versions.at(i).toObject();
    const QString versionTitle = versionObj.value(QStringLiteral("title")).toString();
    const QJsonArray items = versionObj.value(QStringLiteral("items")).toArray();

    if (versionTitle.isEmpty()) {
      continue;
    }

    auto* versionItem = new QTreeWidgetItem(tree_);
    versionItem->setText(0, versionTitle);

    for (const auto& changeValue : items) {
      const QString changeText = changeValue.toString();
      if (changeText.isEmpty()) {
        continue;
      }

      auto* changeItem = new QTreeWidgetItem(versionItem);
      changeItem->setText(0, QStringLiteral("• %1").arg(changeText));
    }

    if (i == 0) {
      versionItem->setExpanded(true);
    }
  }
}
