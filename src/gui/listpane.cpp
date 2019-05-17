// qMemo/listpane.cpp - List view class
// qMemo is a note taking application
//
//  Copyright (C) 2019  Yasuhiro Yamakawa <kawatab@yahoo.co.jp>
//
//  This program is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or any
//  later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
//  License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "listpane.hpp"

#include <QBoxLayout>
#include <QComboBox>
#include <QListView>
#include <QPushButton>
#include "../datahandler.hpp"
#include "previewdelegate.hpp"

ListPane::ListPane()
  : QWidget(),
    mListView(new QListView),
    mFileInfoProxy()
{
  auto selectBox { new QComboBox };
  auto moveButton { new QPushButton(tr("Move")) };
  auto newButton { new QPushButton(tr("New")) };

  // selected item changed in ComboBox
  connect(selectBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ListPane::selectedFileListChanged);

  // button clicked
  connect(newButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ListPane::newButtonClicked);
  connect(moveButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ListPane::moveButtonClicked);

  // Enable buttons
  connect(this, &ListPane::isEditableChanged, newButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::setEnabled));
  connect(this, &ListPane::itemCounted, moveButton, static_cast<void (QPushButton::*)(bool)>(&QPushButton::setEnabled));

  selectBox->addItem(tr("Active"));
  selectBox->addItem(tr("Archive"));

  auto hbox { new QHBoxLayout };
  hbox->addWidget(selectBox);
  hbox->addWidget(moveButton);
  hbox->addWidget(newButton);


  prepareListView();

  auto vbox { new QVBoxLayout };
  vbox->setSpacing(2);
  vbox->setMargin(2);
  vbox->addLayout(hbox);
  vbox->addWidget(mListView);

  setLayout(vbox);
}

void ListPane::prepareListView()
{
  mListView->setModel(&mFileInfoProxy);
  mFileInfoProxy.sort(0, Qt::DescendingOrder);
  
  QAbstractItemDelegate* delegate { new PreviewDelegate()};
  mListView->setItemDelegate(delegate);
  
  connect(mListView->selectionModel(), static_cast<void (QItemSelectionModel::*)(const QItemSelection&, const QItemSelection&)>(&QItemSelectionModel::selectionChanged),
	  this, &ListPane::changeSelectedFile);
  
  mListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

}

void ListPane::setFileList(QAbstractItemModel* fileList)
{
  mFileInfoProxy.setSourceModel(fileList);
  mListView->setCurrentIndex(mFileInfoProxy.index(0, 0));
}

void ListPane::setCurrentSourceIndex(int sourceIndex)
{
  auto listModel { mFileInfoProxy.sourceModel() };
  auto indexModel { listModel->index(sourceIndex, 0)};
  mListView->setCurrentIndex(mFileInfoProxy.mapFromSource(indexModel));
  emit itemCounted(true);
}

int ListPane::currentSourceIndex() const
{
  QModelIndexList indexes { mListView->selectionModel()->currentIndex() };
  return mFileInfoProxy.mapToSource(indexes[0]).row();
}

void ListPane::changeSelectedFile(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(deselected);
  
  QModelIndexList indexes { selected.indexes() };
  
  if (!indexes.isEmpty()) {
    emit selectedFileChanged(mFileInfoProxy.mapToSource(indexes[0]).row());
  }
}

bool ListPane::checkCount()
{
  bool exists { mFileInfoProxy.rowCount() > 0 };
  emit itemCounted(exists);
  return exists;
}
