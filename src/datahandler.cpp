// qMemo/datahandler.hpp - class for file loading/saving
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


#include "datahandler.hpp"

#include <QDateTime>
#include <QFileInfo>
#include <QList>
#include <QTextStream>


const QString DataHandler::TIMESTAMP_PATTERN { "yyyy-MM-dd HH:mm:ss" };
const QString DataHandler::DEFAULT_DIRECTORY { ".memo" };
const QString DataHandler::ARCHIVE_DIRECTORY { "archive" };

DataHandler::DataHandler()
  : QObject(), mWorkDirectory(), mArchiveDirectory(),
    mCurrentFile(),
    mCurrentFileList(nullptr), mActiveFileList(), mArchiveFileList()
{
  mWorkDirectory = setDirectory(QDir::home(), DEFAULT_DIRECTORY);
  mArchiveDirectory = setDirectory(mWorkDirectory, ARCHIVE_DIRECTORY);
  setFileList(mWorkDirectory, &mActiveFileList);
  setFileList(mArchiveDirectory, &mArchiveFileList);
  mCurrentFileList = &mActiveFileList;
}

QDir DataHandler::setDirectory(QDir path, const QString& name)
{
  if (path.absolutePath() != "" && !path.cd(name)) {
    qDebug("Create work directory: DataHandler::setDirectory()");

    if (!path.mkdir(name) || !path.cd(name)) {
      qFatal("Tried to create directory, but failed.: DataHandler::setDirectory()");

      return QDir();
    }
  }

  return path;
}

void DataHandler::setFileList(const QDir& dir, FileInfoModel* list)
{
  for (auto fileInfo : dir.entryInfoList(QDir::Files, QDir::Time)) {
    QUrl url { QUrl::fromLocalFile(fileInfo.filePath()) };
    QString modified { getLastModifiedDate(url) };
    QString preview { getPreviewOfContents(url) };
    list->appendItem(url, modified, preview);
  }
}

bool DataHandler::isAvailable() const
{
  return !mWorkDirectory.absolutePath().isEmpty() && !mArchiveDirectory.absolutePath().isEmpty();
}

void DataHandler::setActiveMode(bool b)
{
  setCurrentFileList(b ? &mActiveFileList : &mArchiveFileList);
}

QUrl DataHandler::currentFile() const
{
  return mCurrentFile;
}

void DataHandler::setCurrentFile(const QUrl& url)
{
  mCurrentFile = url;
}

bool DataHandler::hasCurrentFile() const
{
  return !currentFile().toLocalFile().isEmpty();
}

void DataHandler::releaseCurrentFile()
{
  setCurrentFile(QUrl());
}

void DataHandler::setCurrentFileList(FileInfoModel* model)
{
  mCurrentFileList = static_cast<FileInfoModel*>(model);
  emit fileListSwitched(mCurrentFileList);
  emit isEditableChanged(mCurrentFileList == &mActiveFileList);
}

bool DataHandler::isEditable() const
{
  return mCurrentFileList == &mActiveFileList;
}

int DataHandler::createNewFile(const QString& text)
{
  QUrl newFile { createFile() };

  if (newFile.toLocalFile().isEmpty()) {
    qCritical("Failed to create new file: DataHandler::createNewFile()");
    return -1;
  } else {
    if (!text.isEmpty()) {
      saveFile(newFile, text);
    }
    
    mCurrentFileList->appendItem(newFile, getLastModifiedDate(newFile), getPreviewOfContents(newFile));
    releaseCurrentFile();
    qInfo("Created a new file successfully: DataHandler::createNewFile()");
    return mCurrentFileList->rowCount() - 1;
  }
}

QString DataHandler::getLastModifiedDate(const QUrl& path) const
{
  QFileInfo fileInfo { path.toLocalFile() };

  return fileInfo.lastModified().toString(TIMESTAMP_PATTERN);
}

QString DataHandler::getPreviewOfContents(const QUrl& path) const
{
  static const int MAX_LENGTH_OF_PREVIEW { 300 };
  QFile file { path.toLocalFile() };
  QStringList lines;

  if (file.exists()) {
    if (loadFile(&file, &lines, MAX_LENGTH_OF_PREVIEW, DataHandler::withTrim)) {
      return lines.join(' ').left(MAX_LENGTH_OF_PREVIEW);
    }
  }

  return "";
}

QUrl DataHandler::createFile() const
{
  QString name { QString::number(QDateTime::currentMSecsSinceEpoch()) + ".txt" };

  if (mWorkDirectory.absolutePath().isEmpty()) {
    qCritical("Cannot find \".memo\": DataHandler::createFile()");

    return QUrl();
  } else {
    QFileInfo fileInfo { mWorkDirectory, name };
    QUrl path { QUrl::fromLocalFile(fileInfo.filePath()) };
    QFile file { path.toLocalFile() };

    return file.open(QIODevice::WriteOnly) ? path : QUrl();
  }
}

QString DataHandler::loadCurrentFile() const
{
  static const int MAX_LENGTH_OF_TEXT { 0xffffff };

  QFile file { currentFile().toLocalFile() };
  QStringList contents;
  loadFile(&file, &contents, MAX_LENGTH_OF_TEXT, withoutTrim);
  return contents.join("");
}

bool DataHandler::loadFile(QFile* file, QStringList* contents, int maxLength, QString (*func)(const QByteArray&)) const
{
  if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical("File wasn't loaded: DataHandler::loadFile()");

    return false;
  }

  while (!file->atEnd()) {
    QString line { func(file->readLine(maxLength)) };

    if (contents->length() + line.length() > maxLength) break;

    contents->append(line);
  }

  return true;
}

QString DataHandler::withoutTrim(const QByteArray& byteArray)
{
  return byteArray;
}

QString DataHandler::withTrim(const QByteArray& byteArray)
{
  return QString::fromUtf8(byteArray).trimmed();
}

void DataHandler::selectFile(int index)
{
  if (index < 0) {
    releaseCurrentFile();
  } else {
    setCurrentFile(mCurrentFileList->data(mCurrentFileList->index(index, 0), FileInfoModel::FileURLRole).toUrl());
  }
}

bool DataHandler::saveCurrentFile(const QString& text) const
{
  if (!hasCurrentFile()) {
    qInfo("No file to save: DataHandler::saveCurrentFile()");
    return false;
  } else if (saveFile(currentFile(), text)) {
    updateFileInfo(currentFile());
    qInfo("Saved successfully: DataHandler::saveCurrentFile()");
    return true;
  } else {
    qCritical("Failed to save: DataHandler::saveCurrentFile()");
    return false;
  }
}

bool DataHandler::saveAndCloseCurrentFile(const QString& text)
{
  bool isSaved { saveCurrentFile(text) };
  releaseCurrentFile();
  return isSaved;
}

bool DataHandler::saveFile(const QUrl& path, const QString& lines) const
{
  QFile file { path.toLocalFile() };

  if (!file.exists() ||
      !file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

  QTextStream out { &file };
  out << lines;

  return true;
}

int DataHandler::deleteEmptyFile()
{
  if (hasCurrentFile()) {
    QUrl dispose { currentFile() };

    if (deleteFile(currentFile())) {
      qInfo("Delete empty file: DataHandler::deleteEmptyFile()");
      releaseCurrentFile();
      QModelIndex sourceIndex { mCurrentFileList->removeItem(dispose) };

      if (sourceIndex.isValid()) {
	return sourceIndex.row();
      }
    }
  }

  qCritical("Failed to delete empty file: DataHandler::deleteEmptyFile()");
  return -1;
}

bool DataHandler::deleteFile(const QUrl& path) const
{
  QFile file { path.toLocalFile() };

  return file.exists() && file.remove();
}

void DataHandler::updateFileInfo(const QUrl& url) const
{
  mCurrentFileList->modifyItem(url, getLastModifiedDate(url), getPreviewOfContents(url));
}

void DataHandler::moveCurrentFile(int index)
{
  QModelIndex proxyIndex { mCurrentFileList->index(index, 0) };
  QUrl url { mCurrentFileList->get(proxyIndex, "fileURL").toUrl() };
  FileInfoModel* otherFileList { mCurrentFileList == &mActiveFileList ? &mArchiveFileList : &mActiveFileList };
		
  if (url == currentFile()) {
    QUrl newUrl { moveCurrentFile(url) };
    otherFileList->appendItem(newUrl, getLastModifiedDate(newUrl), getPreviewOfContents(newUrl));
    releaseCurrentFile();
    mCurrentFileList->removeItem(url); // invoke onCurrentIndexChanged()
    qInfo("Moved successfully: DataHandler::moveCurrentFile()");
  } else {
    qCritical("Failed to move: DataHandler::moveCurrentFile()");
  }
}

QUrl DataHandler::moveCurrentFile(const QUrl& url) const
{
  QFileInfo file { url.toLocalFile() };
  QDir dir { (file.path() == mWorkDirectory.path()) ? mArchiveDirectory : mWorkDirectory };

  QFileInfo newPath { dir, file.fileName() };
  QFile oldPath { file.filePath() };
  oldPath.rename(newPath.filePath());

  return QUrl::fromLocalFile(newPath.filePath());
}
