//CertTableModel.h by Konstantine Kozachuck as neurocod - 2018.02.08 02:03:04
#pragma once

class CertTableModel: public QAbstractTableModel {
	public:
	enum Columns {
		ColName,
		ColMail,
		ColVcardId,
		ColCertFile,
		ColMenu,
		ColCertCreated,
		//ColTemplateFile,

		ColEnd
	};
	enum CertType {
		EC,
		RSA
	};
	struct Row {
		QString _name;//like nickname
		QDir _dir;
		QString _mail;
		QString _vcardId;//optional
		QString _templateFile;//absolute path
		QString _baseName;//file name without path and extension
		QString _certFile;
		QDateTime _certCreated;
		QByteArray _templateLine;//it's parsed into name, mail and vcard
		
		QString loadFromTemplateFile(const QFileInfo & entry);//returns error, isEmpty() -> ok
		QString generateCert(CertType certType, QString & sha256)const;//returns error, isEmpty() -> ok
		QString removeFiles();//returns error, isEmpty() -> ok
	};
	QList<Row> _rows;
	
	CertTableModel(QObject*parent);
	void reload();
	int indexByFile(const QString & s)const;
	void removeRows(const QModelIndexList & rows);
	virtual int rowCount(const QModelIndex& index = QModelIndex())const override;
	virtual int columnCount(const QModelIndex& index = QModelIndex())const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};