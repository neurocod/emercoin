#include <QtCore>

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);
	QTextStream qtin(stdin);
	QString line = qtin.readLine();
	QString path = QString("D:/g/emcssl/args %1.txt")
		.arg(QDateTime::currentDateTime().toString("yyyy.MM.dd HH-mm-ss-zzz"));
	struct File: public QFile {
		File(QString path): QFile(path) {}
		void write(QString str) {
			QFile::write(str.toLatin1());
		}
	};
	File f(path);
	if(!f.open(QIODevice::WriteOnly)) {
		return 1;
	}
	f.write(QCoreApplication::applicationFilePath()+'\n');
#ifdef _DEBUG
	f.write("Debug\n");
#else
	f.write("Release\n");
#endif
	f.write(QString("argc: %1\n").arg(argc));
	for(int i = 0; i < argc; ++i) {
		QString str = QString("%1) %2\n")
			.arg(i)
			.arg(QString(argv[i]));
		f.write(str);
	}
	return 0;
}
