//ShellImitation.h by Emercoin developers - 2018.03.04 17:17:09
#pragma once

struct ShellImitation {
	static QPointer<QListWidget> s_logger;
	static void maybeLog(const QString & s);

	static bool touch(const QDir & dir, const QString & fileName, QString & err);
	static bool mkpath(const QDir & dir, const QString & path, QString & error, int tries = 5);
	static bool write(const QString & path, const QByteArray & what, QString &err);
	static bool removeRecursiveFilesOnly(QDir & dir, QString &err);
	protected:
	static QString tr(const char*c);
};