//ShellImitation.cpp by Emercoin developers - 2018.03.04 17:17:09
#include "pch.h"
#include "ShellImitation.h"

bool ShellImitation::touch(const QDir & dir, const QString & fileName, QString & err) {
	QString path = dir.absoluteFilePath(fileName);
	QFile file(path);
	if(!file.open(QIODevice::WriteOnly)) {
		err = QObject::tr("Can't open file %1: %2").arg(path).arg(file.errorString());
		return false;
	}
	file.close();
	err.clear();
	return true;
}
bool ShellImitation::mkpath(const QDir & dir, const QString & path, QString & error, int tries) {
	for(int i = 0; i < tries; ++i) {
		if(i>0)
			QThread::msleep(50);
		if(dir.mkpath(path)) {
			if(dir.exists(path)) {
				error.clear();
				return true;
			}
		}
	}
	error = QObject::tr("Can't create directory %1").arg(dir.absoluteFilePath(path));
	return false;
}
bool ShellImitation::write(const QString & path, const QByteArray & what, QString &err) {
	QFile file(path);
	if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
		err = QObject::tr("Can't open file %1: %2").arg(path).arg(file.errorString());
		return false;
	}
	auto written = file.write(what);
	if(written == what.size()) {
		err.clear();
		return true;
	}
	err = QObject::tr("Can't write %1: only %2 bytes written instead of %3, reason: %4")
		.arg(path)
		.arg(written)
		.arg(what.size())
		.arg(file.errorString());
	return false;
}
bool ShellImitation::removeRecursiveFilesOnly(QDir & dir, QString &err) {
	err.clear();
	QDirIterator it(dir, QDirIterator::Subdirectories);
	while(it.hasNext()) {
		const QString path = it.next();
		QFileInfo info(path);
		if(info.isDir())
			continue;
		if(!dir.remove(path)) {
			err = QObject::tr("Can't delete %1").arg(path);
			return false;
		}
	}
	return true;
}