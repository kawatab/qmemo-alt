// qMemo/mainwindow.hpp - Main window class
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

#include <QMainWindow>

class DataHandler;
class EditPane;
class ListPane;
class QBoxLayout;


class MainWindow : public QMainWindow
{
  Q_OBJECT;

public:
  MainWindow(DataHandler* dataHandler);

public slots:
  void autoSave();
  void changeFile(int sourceIndex);
  void changeFileList(int index);
  void createNewFile();
  void moveCurrentFile();

private:
  void closeEvent(QCloseEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

  void checkItemCount();
  void prepareConnection(DataHandler* dataHandler);

  ListPane* mListPane;
  EditPane* mEditPane;
  DataHandler* mDataHandler;
  bool mTextChanged;
  bool mReadyToSave;
};
