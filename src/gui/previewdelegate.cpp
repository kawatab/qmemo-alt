// qMemo/previewdelegate.cpp - preview style
// qMemo is a note taking application
//
//  Copyright (C) 2019  Yasuhiro Yamakawa <kawatab@outlook.com>
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


#include "previewdelegate.hpp"

#include <QFontMetrics>
#include <QLabel>
#include <QTextEdit>
#include <QTextLayout>
#include <QtWidgets>
#include "../fileinfomodel.hpp"


void PreviewDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  Q_UNUSED(index);

  static const int fontSize { 10 };
  static const int lineHeight { 16 };

  if (option.state & QStyle::State_Selected) {
    painter->fillRect(option.rect, option.palette.highlight());
  }

  painter->setPen(Qt::black);

  // draw modified
  QRectF modifiedRect { option.rect };
  modifiedRect.setHeight(lineHeight);

  QFont modifiedFont { QFont("Arial", fontSize, QFont::Bold) };
  painter->setFont(modifiedFont);

  painter->drawText(modifiedRect,
		    Qt::AlignLeft,
		    index.data(FileInfoModel::ModifiedRole).toString());

  // draw preview
  QRectF previewRect { option.rect };
  previewRect.setTop(modifiedRect.bottom());
  previewRect.setHeight(2 * lineHeight);

  QFont previewFont { QFont("Arial", fontSize) };
  QFontMetrics fontMetrics { previewFont };
  QString previewText { index.data(FileInfoModel::PreviewRole).toString() };
  QTextLayout previewLayout { previewText, previewFont };
  previewLayout.beginLayout();
  QTextLine firstLine { previewLayout.createLine() };
  
  if (firstLine.isValid()) {
    firstLine.setLineWidth(previewRect.width());
    firstLine.draw(painter, previewRect.topLeft());

    QTextLine secondLine { previewLayout.createLine() };

    if (secondLine.isValid()) {
      painter->setFont(previewFont);
      QString elidedLine { fontMetrics.elidedText(previewText.mid(secondLine.textStart()), Qt::ElideRight, previewRect.width()) };
      painter->drawText(QPoint(0, previewRect.top() + lineHeight + fontMetrics.ascent()), elidedLine);
			
    }
  }
}

QSize PreviewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  QSize size { QStyledItemDelegate::sizeHint(option, index) };
  size.setHeight(56);

  return size;
}

QWidget* PreviewDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void PreviewDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const {
  QStyledItemDelegate::setEditorData(editor, index);
}

void PreviewDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                const QModelIndex &index) const
{
  QStyledItemDelegate::setModelData(editor, model, index);
}

void PreviewDelegate::commitAndCloseEditor() {
  /*
    StarEditor* editor = qobject_cast<StarEditor*>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
  */
}
