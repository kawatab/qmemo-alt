// qMemo/listpane.hpp - List view class
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


#pragma once

#include <QWidget>
#include "../fileinfoproxy.hpp"

class DataHandler;
class QAbstractItemModel;
class QItemSelection;
class QListView;


class ListPane : public QWidget
{
  Q_OBJECT
  
public:
  explicit ListPane();
  
  bool checkCount();
  int currentSourceIndex() const;
  void setCurrentSourceIndex(int sourceIndex);

public slots:
  void changeSelectedFile(const QItemSelection& selected, const QItemSelection& deselected);
  void setFileList(QAbstractItemModel* model);
  
signals:
  void isEditableChanged(bool editable);
  void itemCounted(bool exists);
  void moveButtonClicked(bool checked);
  void newButtonClicked(bool checked);
  void selectedFileListChanged(int index);
  void selectedFileChanged(int sourceIndex);

private:
  void prepareListView();

  QListView* mListView;
  FileInfoProxy mFileInfoProxy;
};
