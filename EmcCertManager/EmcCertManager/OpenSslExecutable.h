﻿//OpenSslExecutable.h by Emercoin developers - 2018.03.04 17:33:22
#pragma once

class OpenSslExecutable: public QProcess {
	public:
		QString _strOutput;
		OpenSslExecutable();
		QString errorString()const;
		QString exec(const QStringList & args);
		bool generateKeyAndCertificateRequest(const QString & baseName, const QString & subj);
		bool generateCertificate(const QString & baseName, const QString & configDir);
		bool createCertificatePair(const QString & baseName, const QString & configDir);
		bool sha256FromCertificate(const QString & baseName, QString & sha256);
		void setLogger(QTextBrowser*l);
	protected:
		QString _path;
		QPointer<QTextBrowser> _logger;
		void willNeedUserInput(bool b=true);
		bool existsOrExit(const QDir & dir, const QString & file);
		bool deleteOrExit(QDir & dir, const QString & file, int tries=5);
		void readToMe();
		QString log(const QString & s);
};