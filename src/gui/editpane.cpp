// qMemo/editpane.cpp - List view class
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

#include "editpane.hpp"

#include <QBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>


EditPane::EditPane()
  : mTextEdit(new QPlainTextEdit)
{
  auto selectAllButton { new QPushButton(tr("Select all")) };
  auto cutButton { new QPushButton(tr("Cut")) };
  auto copyButton { new QPushButton(tr("Copy")) };
  auto pasteButton { new QPushButton(tr("Paste")) };

  auto hbox { new QHBoxLayout };
  hbox->addWidget(selectAllButton);
  hbox->addWidget(cutButton);
  hbox->addWidget(copyButton);
  hbox->addWidget(pasteButton);

  auto vbox { new QVBoxLayout };
  vbox->setSpacing(2);
  vbox->setMargin(2);
  vbox->addLayout(hbox);
  vbox->addWidget(mTextEdit);
  setLayout(vbox);

  // button clicked
  connect(selectAllButton, SIGNAL(clicked()), mTextEdit, SLOT(selectAll()));
  connect(cutButton, SIGNAL(clicked()), mTextEdit, SLOT(cut()));
  connect(copyButton, SIGNAL(clicked()), mTextEdit, SLOT(copy()));
  connect(pasteButton, SIGNAL(clicked()), mTextEdit, SLOT(paste()));

  // enable buttons
  connect(this, SIGNAL(editableRequested(bool)), cutButton, SLOT(setEnabled(bool))); 
  connect(this, SIGNAL(editableRequested(bool)), pasteButton, SLOT(setEnabled(bool))); 

  // text changed
  connect(mTextEdit, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
}

void EditPane::setEditable(bool b)
{
  mTextEdit->setReadOnly(!b);
  emit editableRequested(b);
}

void EditPane::setText(const QString& text)
{
  mTextEdit->setPlainText(text);
  mTextEdit->moveCursor(QTextCursor::Start);
  mTextEdit->setFocus(Qt::OtherFocusReason);
}

QString EditPane::text() const
{
  return mTextEdit->toPlainText();
}
