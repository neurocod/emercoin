//OpenSslExecutable.cpp by Emercoin developers - 2018.03.04 17:33:22
#include "pch.h"
#include "OpenSslExecutable.h"
#include "Settings.h"

OpenSslExecutable::OpenSslExecutable() {
	setWorkingDirectory(Settings::certDir().absolutePath());
}
void OpenSslExecutable::willNeedUserInput(bool b) {
	if(!b) {
		setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args) {});
		return;
	}
	setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args) {
		args->flags |= CREATE_NEW_CONSOLE;
		args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
		args->startupInfo->dwFlags |= STARTF_USEFILLATTRIBUTE;
		args->startupInfo->dwFillAttribute = BACKGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY;
	});
}
QString OpenSslExecutable::path()const {
	return Settings::certDir().absoluteFilePath("shell.w32-ix86/openssl.exe");
}
QString OpenSslExecutable::errorString()const {
	return __super::errorString() + '\n' + _strOutput;
}
bool OpenSslExecutable::exec(const QStringList & args) {
	log(tr("Starting openssl ") + args.join(' '));
	_strOutput.clear();
	start(path(), args, QIODevice::ReadWrite);
	const int maxTimeout = 10000 * 1000;
	if(!waitForStarted(maxTimeout)) {
		log(tr("Can't start: %1").arg(error()));
		return false;
	}
	if(!waitForFinished(maxTimeout)) {
		log(tr("Failed waitinf to finish: %1").arg(error()));
		return false;
	}
	readToMe();
	if(QProcess::NormalExit != exitStatus()) {
		log(tr("Exit status = %1").arg(exitStatus()));
		return false;
	}
	log(tr("Finished ok"));
	return true;
}
void OpenSslExecutable::readToMe() {
	_strOutput += readAllStandardError();
	_strOutput += readAllStandardOutput();
}
bool OpenSslExecutable::existsOrExit(const QDir & dir, const QString & file) {
	if(dir.exists(file))
		return true;
	_strOutput += tr("File %1 does not exist").arg(file);
	return false;
}
bool OpenSslExecutable::deleteOrExit(QDir & dir, const QString & file, int tries) {
	for(int i = 0; i<tries; ++i) {
		if(i>0)
			QThread::msleep(50);
		dir.remove(file);
		if(!dir.exists(file))
			return true;
	}
	_strOutput += tr("File %1 can't be removed").arg(file);
	return false;
}
bool OpenSslExecutable::generateKeyAndCertificateRequest(const QString & baseName, const QString & subj) {
	const QString keyFile = baseName + ".key";
	const QString csrFile = baseName + ".csr";
	QDir dir = workingDirectory();
	if(!deleteOrExit(dir, keyFile))
		return false;
	if(!deleteOrExit(dir, csrFile))
		return false;
	QStringList args = QString("req -new -newkey rsa:2048 -nodes -keyout $KEY -subj $SUBJ -out $CSR").split(' ');
	args.replaceInStrings("$KEY", keyFile);
	args.replaceInStrings("$CSR", csrFile);
	args.replaceInStrings("$SUBJ", subj);
	if(!exec(args) || exitCode() != 0)
		return false;
	return existsOrExit(dir, keyFile) && existsOrExit(dir, csrFile);
}
bool OpenSslExecutable::generateCertificate(const QString & baseName, const QString & configDir) {
	const QString csrFile = baseName + ".csr";
	const QString crtFile = baseName + ".crt";
	QDir dir = workingDirectory();
	if(!existsOrExit(dir, csrFile))
		return false;
	if(!deleteOrExit(dir, crtFile))
		return false;
	QStringList args = QString("ca -config $CA_DIR/ca.config -in $IN -out $OUT -batch").split(' ');
	args.replaceInStrings("$CA_DIR", configDir);
	args.replaceInStrings("$IN", csrFile);
	args.replaceInStrings("$OUT", crtFile);
	if(!exec(args) || exitCode() != 0)
		return false;
	return existsOrExit(dir, crtFile);
}
bool OpenSslExecutable::createCertificatePair(const QString & baseName, const QString & configDir) {
	willNeedUserInput();
	const QString keyFile = baseName + ".key";
	const QString crtFile = baseName + ".crt";
	const QString p12 = baseName + ".p12";
	QDir dir = workingDirectory();
	if(!existsOrExit(dir, keyFile))
		return false;
	if(!existsOrExit(dir, crtFile))
		return false;
	dir.remove(p12);
	QStringList args = QString("pkcs12 -export -in $CRT -inkey $KEY -certfile $CA_DIR/emcssl_ca.crt -out $P12").split(' ');
	args.replaceInStrings("$CA_DIR", configDir);
	args.replaceInStrings("$CRT", crtFile);
	args.replaceInStrings("$KEY", keyFile);
	args.replaceInStrings("$P12", p12);
	if(!exec(args))
		return false;
	if(exitCode() != 0)
		return false;
	return existsOrExit(dir, p12);
}
bool OpenSslExecutable::sha256FromCertificate(const QString & baseName, QString & sha256) {
	willNeedUserInput(false);
	const QString crtFile = baseName + ".crt";
	QDir dir = workingDirectory();
	if(!existsOrExit(dir, crtFile))
		return false;
	QStringList args = QString("x509 -noout -in $CRT -fingerprint -sha256").split(' ');
	args.replaceInStrings("$CRT", crtFile);
	if(!exec(args))
		return false;
	if(exitCode() != 0)
		return false;
	//now we have something like
	//SHA256 Fingerprint=D1:56:13:38:34:07:4C:B2:A8:01:21:EB:AF:3A:97:3E:65:3B:4E:3C:FA:C5:E9:07:C4:81:EB:82:30:D5:AE:3A
	const QString prefix = "SHA256 Fingerprint=";
	if(!_strOutput.startsWith(prefix)) {
		_strOutput.prepend(tr("Unknown SHA256 Fingerprint output"));
		return false;
	}
	sha256 = _strOutput;
	sha256.remove(':');
	sha256.remove(0, prefix.count());
	sha256.toLower();
	return true;
}
void OpenSslExecutable::log(const QString & s) {
	if(_logger) {
		_logger->append(s);
		QCoreApplication::processEvents();
	}
}