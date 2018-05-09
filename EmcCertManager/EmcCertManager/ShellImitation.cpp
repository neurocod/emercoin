﻿//ShellImitation.cpp by Emercoin developers - 2018.03.04 17:17:09
#include "pch.h"
#include "ShellImitation.h"

QPointer<QTextBrowser> ShellImitation::s_logger;
void ShellImitation::maybeLog(const QString & s) {
	if(s_logger && QThread::currentThread()==qApp->thread()) {
		s_logger->append(s + '\n');
		QCoreApplication::processEvents();
	}
}
QString ShellImitation::tr(const char*c) {
	return QObject::tr(c);
}
bool ShellImitation::touch(const QDir & dir, const QString & fileName, QString & err) {
	QString path = dir.absoluteFilePath(fileName);
	maybeLog(tr("Touching %1").arg(path));
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly)) {
		err = tr("Can't open file %1: %2").arg(path).arg(file.errorString());
		maybeLog(err);
		return false;
	}
	file.close();
	err.clear();
	return true;
}
bool ShellImitation::mkpath(const QDir & dir, const QString & path, QString & error, int tries) {
	maybeLog(tr("mkpath %1").arg(dir.absoluteFilePath(path)));
	for(int i = 0; i < tries; ++i) {
		if(i>0)
			QThread::msleep(50);
		if(dir.mkpath(path)) {
			if(dir.exists(path)) {
				error.clear();
				return true;
			}
		}
		maybeLog(tr("Trying again..."));
	}
	error = tr("Can't create directory %1").arg(dir.absoluteFilePath(path));
	maybeLog(error);
	return false;
}
bool ShellImitation::write(const QString & path, const QByteArray & what, QString &err) {
	maybeLog(tr("Writing into %1...").arg(path));
	QFile file(path);
	if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
		err = tr("Can't open file %1: %2").arg(path).arg(file.errorString());
		maybeLog(err);
		return false;
	}
	auto written = file.write(what);
	if(written == what.size()) {
		err.clear();
		return true;
	}
	err = tr("Can't write %1: only %2 bytes written instead of %3, reason: %4")
		.arg(path)
		.arg(written)
		.arg(what.size())
		.arg(file.errorString());
	maybeLog(err);
	return false;
}
bool ShellImitation::removeRecursiveFilesOnly(QDir & dir, QString &err) {
	maybeLog(tr("Remove recursive from %1").arg(dir.absolutePath()));
	err.clear();
	QDirIterator it(dir, QDirIterator::Subdirectories);
	while(it.hasNext()) {
		const QString path = it.next();
		QFileInfo info(path);
		if(info.isDir())
			continue;
		maybeLog(tr("... removing %1").arg(path));
		if(!dir.remove(path)) {
			err = tr("Can't delete %1").arg(path);
			maybeLog(err);
			return false;
		}
	}
	return true;
}