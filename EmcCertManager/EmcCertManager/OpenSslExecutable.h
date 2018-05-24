//OpenSslExecutable.h by Emercoin developers - 2018.03.04 17:33:22
#pragma once
class CertLogger;

class OpenSslExecutable: public QProcess {
	public:
		OpenSslExecutable();
		QString errorString()const;
		QString exec(const QStringList & args);
		bool generateKeyAndCertificateRequest(const QString & baseName, const QString & subj);
		bool generateCertificate(const QString & baseName, const QString & configDir);
		bool createCertificatePair(const QString & baseName, const QString & configDir, const QString & pass);
		bool sha256FromCertificate(const QString & baseName, QString & sha256);
		void setLogger(CertLogger*l);
		QString log(const QString & s);
		static bool found();
	protected:
		QString _path;
		QString _strOutput;
		CertLogger* _logger = 0;
		bool existsOrExit(const QDir & dir, const QString & file);
		bool deleteOrExit(QDir & dir, const QString & file, int tries=5);
		void readToMe();
		static QString path();
};