// qMemo/mainwindow.cpp - Main window class
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


#include "mainwindow.hpp"

#include <QBoxLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTimer>
#include "../datahandler.hpp"
#include "editpane.hpp"
#include "listpane.hpp"


MainWindow::MainWindow(DataHandler* dataHandler)
  : QMainWindow(),
    mListPane(new ListPane),
    mEditPane(new EditPane),
    mDataHandler(dataHandler),
    mTextChanged(false),
    mReadyToSave(false)
{
  prepareConnection(dataHandler);

  dataHandler->setActiveMode(true);

  mListPane->setFixedWidth(300);

  auto central { new QWidget };
  auto layout { new QHBoxLayout };
  layout->setSpacing(0);
  layout->setMargin(0);
  central->setLayout(layout);
  layout->addWidget(mListPane);
  layout->addWidget(mEditPane);
  setCentralWidget(central);
  central->setFixedWidth(this->width());

  setMinimumSize(QSize(600, 300));
  resize(800, 600);
  setWindowTitle(tr("qMemo"));
  setVisible(true);

  auto timer { new QTimer(this) };
  connect(timer, SIGNAL(timeout()), this, SLOT(autoSave()));
  timer->start(2000);
}

void MainWindow::prepareConnection(DataHandler* dataHandler)
{
  connect(dataHandler, &DataHandler::fileListSwitched, mListPane, &ListPane::setFileList);
  connect(dataHandler, SIGNAL(isEditableChanged(bool)), mListPane, SIGNAL(isEditableChanged(bool)));
  connect(dataHandler, &DataHandler::isEditableChanged, mEditPane, &EditPane::setEditable);
  connect(mListPane, &ListPane::selectedFileChanged, this, &MainWindow::changeFile);
  connect(mListPane, &ListPane::selectedFileListChanged, this, &MainWindow::changeFileList);
  connect(mListPane, &ListPane::newButtonClicked, this, &MainWindow::createNewFile);
  connect(mListPane, &ListPane::moveButtonClicked, this, &MainWindow::moveCurrentFile);
  connect(mEditPane, &EditPane::textChanged, [=]() { mTextChanged = true; } );

}

void MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
  this->centralWidget()->setFixedWidth(this->width());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  if (mDataHandler->isEditable()) {
    if (mDataHandler->hasCurrentFile()) {
      if (mEditPane->text().trimmed().isEmpty()) {
	mDataHandler->deleteEmptyFile();
      } else if (mTextChanged) {
	mDataHandler->saveCurrentFile(mEditPane->text());
      }
    } else {
      mDataHandler->createNewFile(mEditPane->text());
    }
  }

  event->accept();
}

void MainWindow::changeFile(int sourceIndex)
{
  if (mDataHandler->isEditable() && mDataHandler->hasCurrentFile()) {
    if (mEditPane->text().trimmed().isEmpty()) {
      int previousIndex { mDataHandler->deleteEmptyFile() };
      if (sourceIndex > previousIndex) --sourceIndex;
    } else if (mTextChanged) {
      mDataHandler->saveAndCloseCurrentFile(mEditPane->text());
    }
  }

  mDataHandler->selectFile(sourceIndex);
  mEditPane->setText(mDataHandler->loadCurrentFile());
  mTextChanged = mReadyToSave = false;
}

void MainWindow::changeFileList(int index)
{
  if (index == 0) {
    mDataHandler->setActiveMode(true);
  } else {
    if (mEditPane->text().trimmed().isEmpty()) {
      mDataHandler->deleteEmptyFile();
    } else if (mTextChanged) {
      mDataHandler->saveAndCloseCurrentFile(mEditPane->text());
    }

    mDataHandler->setActiveMode(false);
  }

  checkItemCount();
}
		
void MainWindow::createNewFile()
{
  if (mEditPane->text().isEmpty() && mDataHandler->hasCurrentFile()) return;
  
  int sourceIndex { mDataHandler->createNewFile("") };

  if (sourceIndex < 0) {
    qCritical("Failed to create a new file: MainWindow::createNewFile()");
  } else {
    mTextChanged = mReadyToSave = false;
    mListPane->setCurrentSourceIndex(sourceIndex);
  }
}

void MainWindow::moveCurrentFile()
{
  if (mDataHandler->isEditable() && mEditPane->text().isEmpty()) return;

  if (mTextChanged) mDataHandler->saveAndCloseCurrentFile(mEditPane->text());

  mEditPane->setText("");
  mTextChanged = mReadyToSave = false;
  mDataHandler->moveCurrentFile(mListPane->currentSourceIndex());
  checkItemCount();
}

void MainWindow::checkItemCount()
{
  if (!mListPane->checkCount()) {
    mDataHandler->releaseCurrentFile(); // release before text clear
    mEditPane->setText("");
    mTextChanged = mReadyToSave = false;
  }
}

void MainWindow::autoSave()
{
  if (mDataHandler->isEditable()) {
    if (mTextChanged) {
      if (mReadyToSave) {
	if (mDataHandler->hasCurrentFile()) {
	  if (mDataHandler->saveCurrentFile(mEditPane->text())) {
	    mTextChanged = mReadyToSave = false;
	    qInfo("Saved automatically: MainWindow::autoSave()");
	  } else {
	    qCritical("Failed to save file: MainWindow::autoSave()");
	    return; // do nothing if failed
	  }
	} else {
	  int sourceIndex { mDataHandler->createNewFile(mEditPane->text()) };

	  if (sourceIndex < 0) {
	    qCritical("Failed to create a new file: MainWindow::autoSave()");
	  } else {
	    mTextChanged = mReadyToSave = false;
	    mListPane->setCurrentSourceIndex(sourceIndex);
	    qInfo("Created a new file: MainWindow::autoSave()");
	  }
	}
      }
    }
    
    mReadyToSave = mTextChanged;
  }
}
