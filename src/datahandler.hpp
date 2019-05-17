// qMemo/datahandler.hpp - Main application class
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


#include <QDir>
#include <QObject>
#include <QUrl>
#include "fileinfomodel.hpp"


class DataHandler : public QObject
{
  Q_OBJECT

public:
  DataHandler();
  ~DataHandler() = default;
  DataHandler(const DataHandler& other) = delete;
  DataHandler& operator=(const DataHandler& other) = delete;
  DataHandler(const DataHandler&& other) = delete;
  DataHandler& operator=(const DataHandler&& other) = delete;

  int createNewFile(const QString& text);
  int deleteEmptyFile();
  bool hasCurrentFile() const;
  bool isAvailable() const;
  bool isEditable() const;
  QString loadCurrentFile() const;
  void moveCurrentFile(int index);
  void releaseCurrentFile();
  bool saveAndCloseCurrentFile(const QString& text);
  bool saveCurrentFile(const QString& text) const;
  void selectFile(int index);
  void setActiveMode(bool b);

signals:
  void fileListSwitched(FileInfoModel* fileList);
  void isEditableChanged(bool b);

private:
  QUrl createFile() const;
  QUrl currentFile() const;
  bool deleteFile(const QUrl& path) const;
  QString getLastModifiedDate(const QUrl& path) const;
  QString getPreviewOfContents(const QUrl& path) const;
  bool loadFile(QFile* file, QStringList* contents, int maxLength, QString(*func)(const QByteArray&)) const;
  QUrl moveCurrentFile(const QUrl& url) const;
  bool saveFile(const QUrl& path, const QString& lines) const;
  void setCurrentFile(const QUrl& url);
  void setCurrentFileList(FileInfoModel* model);
  QDir setDirectory(QDir path, const QString& name);
  void setFileList(const QDir& dir, FileInfoModel* list);
  void setIsEditable(bool b);
  void updateFileInfo(const QUrl& url) const;

  static QString withTrim(const QByteArray& byteArray);
  static QString withoutTrim(const QByteArray& byteArray);

  QDir mWorkDirectory;
  QDir mArchiveDirectory;
  QUrl mCurrentFile;
  FileInfoModel* mCurrentFileList;
  FileInfoModel mActiveFileList;
  FileInfoModel mArchiveFileList;

  static const QString TIMESTAMP_PATTERN;
  static const QString DEFAULT_DIRECTORY;
  static const QString ARCHIVE_DIRECTORY;
};
