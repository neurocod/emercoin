//CertTableView.cpp by Konstantine Kozachuck as neurocod - 2018.02.08 16:10:37
#include "pch.h"
#include "CertTableView.h"
#include "CertTableModel.h"

CertTableView::CertTableView() {
	horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	_model = new Model(this);
	setModel(_model);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	recreateButtons();
	connect(_model, &Model::modelReset, this, &CertTableView::recreateButtons);
}
CertTableView::Model* CertTableView::model()const {
	return _model;
}
void CertTableView::recreateButtons() {
	for(int row = 0; row < _model->rowCount(); ++row) {
		auto w = new QToolBar;
		setIndexWidget(_model->index(row, Model::ColMenu), w);
		
		auto gen = new QAction(tr("Generate again"));
		gen->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-yes-32.png"));
		gen->setProperty("row", row);
		connect(gen, &QAction::triggered, this, &CertTableView::onGenerateCert);
		w->addAction(gen);

		auto show = new QAction(tr("Show in explorer"));
		show->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-open-32.png"));
		show->setProperty("row", row);
		connect(show, &QAction::triggered, this, &CertTableView::showInExplorer);
		w->addAction(show);
	}
}
struct CertTableView::Dialog: public QDialog {
	QComboBox* _certType = new QComboBox;
	QLineEdit* _pass = new QLineEdit;
	QLineEdit* _pass2 = new QLineEdit;
	QLabel * _labelError = new QLabel;
	Dialog(QWidget*parent): QDialog(parent) {
		setWindowFlag(Qt::WindowContextHelpButtonHint, false);
		auto lay = new QVBoxLayout(this);
		auto form = new QFormLayout;
		lay->addLayout(form);

		_certType->addItem("EC", (int)CertTableModel::EC);
		_certType->addItem("RSA", (int) CertTableModel::RSA);
		form->addRow(tr("Certificate type:"), _certType);

		_pass->setEchoMode(QLineEdit::Password);
		_pass2->setEchoMode(QLineEdit::Password);
		form->addRow(new QLabel(tr("Enter password for certificate package.\n"
			"You will use this password when installing certificate into browser:")));
		form->addRow(tr("Password:"), _pass);
		form->addRow(tr("Password again:"), _pass2);
		form->addRow(_labelError);
		_labelError->hide();
		{
			auto box = new QDialogButtonBox;
			lay->addWidget(box);
			auto ok = box->addButton(QDialogButtonBox::Ok);
			auto cancel = box->addButton(QDialogButtonBox::Cancel);
			ok->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-32.png"));
			cancel->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-32.png"));
			connect(ok, &QAbstractButton::clicked, this, &QDialog::accept);
			connect(cancel, &QAbstractButton::clicked, this, &QDialog::reject);
		}
	}
	void showError(const QString & str) {
		QString str2 = QString("<font color='red'>%1</font>").arg(str.toHtmlEscaped());
		str2.replace("\n", "<br>\n");
		_labelError->setText(str2);
		_labelError->show();
	}
	void accept()override {
		if(_pass->text().isEmpty()) {
			showError(tr("Empty password"));
			return;
		}
		if(_pass->text() != _pass2->text()) {
			showError(tr("Passwords differ"));
			return;
		}
		QDialog::accept();
	}
};
void CertTableView::showInGraphicalShell(QWidget *parent, const QString &pathIn) {
#ifdef Q_OS_WIN
	const QString explorer = "explorer.exe";
	QString param;
	if(!QFileInfo(pathIn).isDir())
		param = QLatin1String("/select,");
	param += QDir::toNativeSeparators(pathIn);
	QString command = explorer + " " + param;
	if(!QProcess::startDetached(command)) {
		QMessageBox::warning(parent,
			tr("Launching Windows Explorer failed"),
			tr("Could not find explorer.exe in path to launch Windows Explorer."));
	}
	return;
#endif
#if defined(Q_OS_MAC)
	Q_UNUSED(parent)
		QStringList scriptArgs;
	scriptArgs << QLatin1String("-e")
		<< QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"")
		.arg(pathIn);
	QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
	scriptArgs.clear();
	scriptArgs << QLatin1String("-e")
		<< QLatin1String("tell application \"Finder\" to activate");
	QProcess::execute("/usr/bin/osascript", scriptArgs);
	return;
#endif
	// we cannot select a file here, because no file browser really supports it
	const QFileInfo fileInfo(pathIn);
	QString dir = fileInfo.isDir() ? fileInfo.absolutePath() : fileInfo.dir().absolutePath();
	QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}
void CertTableView::showInExplorer() {
	auto b = qobject_cast<QAction*>(sender());
	Q_ASSERT(b);
	if(!b)
		return;
	int nRow = b->property("row").toInt();
	if(nRow<0 || nRow >= _model->rowCount())
		return;
	const auto & row = _model->_rows[nRow];
	showInGraphicalShell(this, row._templateFile);
}
void CertTableView::onGenerateCert() {
	auto b = qobject_cast<QAction*>(sender());
	Q_ASSERT(b);
	if(!b)
		return;
	int nRow = b->property("row").toInt();
	if(nRow<0 || nRow >=_model->rowCount())
		return;
	const auto & row = _model->_rows[nRow];
	Dialog dlg(this);
	if(dlg.exec()!=QDialog::Accepted)
		return;
	auto certType = (CertTableModel::CertType)dlg._certType->currentData().toInt();
	QString sha256;
	QString msg = row.generateCert(certType, sha256);//dlg._pass->text()
	if(!msg.isEmpty()) {
		QMessageBox::critical(this, tr("Error"), msg);
		return;
	}
	_model->reload();
}