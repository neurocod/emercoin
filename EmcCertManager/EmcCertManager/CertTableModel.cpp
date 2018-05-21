﻿//CertTableModel.cpp by Konstantine Kozachuck as neurocod - 2018.02.08 02:03:04
#include "pch.h"
#include "CertTableModel.h"
#include "ShellImitation.h"
#include "Settings.h"
#include "OpenSslExecutable.h"

QString CertTableModel::Row::loadFromTemplateFile(const QFileInfo & entry) {//QString::isEmpty -> ok
	//file example: /CN=Konstantine Kozachuk/emailAddress=neurocod@gmail.com/UID=123
	_templateFile = entry.filePath();
	_dir = entry.dir();
	_baseName = entry.baseName();
	QFile file(_templateFile);
	if(!file.open(QFile::ReadOnly))
		return file.errorString();
	const QByteArray arr = file.readAll();
	const QString str = arr;
	auto lines = str.split('\n', QString::SkipEmptyParts);
	if(lines.count()!=1)
		return tr("Invalid file format: more than 1 non-empty line in %1").arg(_templateFile);
	const QString line = lines[0];
	_templateLine.append(line.toUtf8());
	auto parts = line.split('/', QString::SkipEmptyParts);
	bool formatOk = parts.size() == 2 || parts.size() == 3;
	if(!formatOk)
		return tr("Invalid format of file %1: %2 parts, looking for 2 or 3").arg(_templateFile).arg(parts.size());
	for(QString part: parts) {
		if(!part.contains('='))
			return tr("Invalid format: must be key=value in %1").arg(_templateFile);
		auto kv = part.split('=', QString::SkipEmptyParts);
		if(kv.size() != 2)
			return tr("Invalid format: must be key=value in %1").arg(_templateFile);
		const QString key = kv.first();
		const QString value = kv.last();
		if(key == "CN") {
			_name = value;
		} else if(key == "emailAddress") {
			_mail = value;
		} else if(key == "UID") {
			_vcardId = value;
		} else {
			return tr("Unknown key %1 in %2").arg(key).arg(_templateFile);
		}
	}
	_name = _name.trimmed();
	_mail = _mail.trimmed();
	if(_name.isEmpty())
		return tr("Invalid format: empty name in %1").arg(_templateFile);;
	if(_mail.isEmpty())
		return tr("Invalid format: empty email in %1").arg(_templateFile);;
	QString certPath = _templateFile;
	certPath.replace(".tpl", ".crt");
	QFileInfo cert(certPath);
	if(cert.exists()) {
		_certFile = certPath;
		_certCreated = cert.lastModified();
	}
	return QString();
}
using Shell = ShellImitation;
QString CertTableModel::Row::generateCert(CertType ctype, const QString & pass, QString & sha256)const {//QString::isEmpty -> ok
	QString certType;
	if(ctype == EC) {
		certType = "EC";
	} else if(ctype == RSA) {
		certType = "RSA";
	} else {
		Q_ASSERT(0);
	}
	const QString CA_DIR = "CA-" + certType;

	const QDir cur = Settings::certDir();
	QDir db = cur.absoluteFilePath("db");
	QString err;
	if(!Shell::removeRecursiveFilesOnly(db, err)
	|| !Shell::mkpath(cur, "db/certs", err)
	|| !Shell::mkpath(cur, "db/newcerts", err)
	|| !Shell::touch(db, "index.txt", err)
	|| !Shell::write(db.absoluteFilePath("serial"), _baseName.toLatin1(), err))
		return err;
	OpenSslExecutable openssl;
	openssl.setLogger(Shell::s_logger);
	if(!openssl.generateKeyAndCertificateRequest(_baseName, _templateLine)
		|| !openssl.generateCertificate(_baseName, CA_DIR)
		|| !openssl.createCertificatePair(_baseName, CA_DIR, pass)
		|| !openssl.sha256FromCertificate(_baseName, sha256))
	{
		return openssl.errorString();
	}
	return QString();
}
QString CertTableModel::Row::pathByExt(const QString & extension)const {
	return _dir.absoluteFilePath(_baseName + '.' + extension);
}
void CertTableModel::Row::installIntoSystem()const {
	QDesktopServices::openUrl(
		QUrl::fromLocalFile(pathByExt("p12")));
}
QString CertTableModel::Row::removeFiles() {
	for(auto ext: QString("crt|csr|key|p12|tpl").split('|')) {
		QString path = pathByExt(ext);
		if(QFile::exists(path)) {
			if(!QFile::remove(path)) {
				return QObject::tr("Can't remove %1").arg(path);
			}
		}
	}
	return QString();
}
CertTableModel::CertTableModel(QObject*parent): QAbstractTableModel(parent) {
	reload();
}
void CertTableModel::removeRows(const QModelIndexList & rows) {
	if(rows.isEmpty())
		return;
	int row = rows[0].row();
	if(row < 0 || row >= _rows.count()) {
		Q_ASSERT(0);
		return;
	}
	Row & r = _rows[row];
	QString error = r.removeFiles();
	if(!error.isEmpty()) {
		reload();
		QMessageBox::critical(0, tr("Error"), tr("Error removing files: %1").arg(error));
		return;
	}
	beginRemoveRows(QModelIndex(), row, row);
	_rows.removeAt(row);
	endRemoveRows();
}
void CertTableModel::reload() {
	beginResetModel();
	_rows.clear();
	QDir dir = Settings::certDir();
	const QFileInfoList list = dir.entryInfoList(QStringList() << "*.tpl", QDir::Files, QDir::Name);
	for(const QFileInfo & entry : list) {
		Row item;
		const QString code = item.loadFromTemplateFile(entry);
		if(code.isEmpty()) {
			_rows << item;
		} else {
			QMessageBox::critical(0, tr("Can't load template file"), code);
		}
	}
	endResetModel();
}
int CertTableModel::rowCount(const QModelIndex& index)const {
	return _rows.count();
}
int CertTableModel::columnCount(const QModelIndex& index)const {
	return ColEnd;
}
QVariant CertTableModel::headerData(int section, Qt::Orientation orientation, int role)const {
	if(orientation == Qt::Vertical) {
		if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
			return section + 1;
	} else {
		if(role == Qt::DisplayRole || role == Qt::ToolTipRole) {
			static_assert(ColEnd == 6, "update switch");
			switch(section) {
				case ColName: return tr("Name");
				case ColMail: return tr("email");
				case ColVcardId: return tr("vCard id");
				case ColMenu: return tr("Menu");
				case ColCertFile: return tr("certificate file");
				case ColCertCreated: return tr("Cert updated");
				//case ColTemplateFile: return tr("Template file");
			}
		}
	}
	return QVariant();
}
QVariant CertTableModel::data(const QModelIndex &index, int role) const {
	int row = index.row();
	if(row<0 || row >= rowCount())
		return QVariant();
	const auto & item = _rows.at(row);
	if(role == Qt::DisplayRole || role == Qt::ToolTipRole) {
		static_assert(ColEnd == 6, "update switch");
		switch(index.column()) {
			case ColName: return item._name;
			case ColMail: return item._mail;
			case ColVcardId: return item._vcardId;
			case ColCertFile: return item._certFile;
			case ColMenu: return QVariant();
			case ColCertCreated: return item._certCreated.toString("yyyy.MM.dd HH:mm:ss");
			//case ColTemplateFile: return item._templateFile;
		}
	}
	return QVariant();
}
int CertTableModel::indexByFile(const QString & s)const {
	for(int i = 0; i < _rows.count(); ++i) {
		if(_rows[i]._templateFile==s || _rows[i]._certFile == s)
			return i;
	}
	return -1;
}