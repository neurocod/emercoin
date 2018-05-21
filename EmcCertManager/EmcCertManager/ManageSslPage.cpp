//ManageSslPage.cpp by Konstantine Kozachuck as neurocod
#include "pch.h"
#include "ManageSslPage.h"
#include "Settings.h"
#include "CertTableModel.h"
#include "EmailLineEdit.h"
#include "CertTableView.h"
#include "ShellImitation.h"

struct ManageSslPage::Logger: public QTextBrowser{
	Logger() {
		setWindowTitle(tr("Operation result"));
	}
};
ManageSslPage::ManageSslPage(QWidget*parent): QWidget(parent) {
	setWindowTitle(tr("Certificates"));
	auto lay = new QVBoxLayout(this);
	//https://cryptor.net/tutorial/sozdaem-ssl-sertifikat-emcssl-dlya-avtorizacii-na-saytah
	lay->addWidget(new QLabel("EmerSSL allows you to automatically login without passwords on many sites using cerificate, stored in blockchain."));
	{
		auto lay2 = new QHBoxLayout;
		lay->addLayout(lay2);

		auto btnNew = new QPushButton(tr("New"));
		btnNew->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/file-32.png"));
		connect(btnNew, &QAbstractButton::clicked, this, &ManageSslPage::onCreate);
		lay2->addWidget(btnNew);

		_btnDelete = new QPushButton(tr("Delete"));
		_btnDelete->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-32.png"));
		connect(_btnDelete, &QAbstractButton::clicked, this, &ManageSslPage::onDelete);
		lay2->addWidget(_btnDelete);

		lay2->addStretch();
	}

	_view = new CertTableView;
	connect(_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ManageSslPage::enableDeleteButton);
	enableDeleteButton();
	lay->addWidget(_view);

	_logger = new Logger();
	lay->addWidget(_logger);
	ShellImitation::s_logger = _logger;
}
void ManageSslPage::enableDeleteButton() {
	_btnDelete->setEnabled(_view->selectionModel()->hasSelection());
}
struct ManageSslPage::TemplateDialog: public QDialog {
	QLineEdit* _name = new QLineEdit;
	EmailLineEdit* _email = new EmailLineEdit;
	QLineEdit* _ecard = new QLineEdit;
	QLabel* _emailErrorDesc = new QLabel;
	QPushButton* _okBtn = 0;
	TemplateDialog(QWidget*parent): QDialog(parent) {
		setWindowTitle(tr("New certificate template"));
		setWindowFlag(Qt::WindowContextHelpButtonHint, false);

		auto lay = new QVBoxLayout(this);
		auto form = new QFormLayout;
		lay->addLayout(form);
		const QString strMandatoryField = tr("Mandatory field");
		_name->setPlaceholderText(strMandatoryField);
		form->addRow(tr("Name or nickname:"), _name);
		{
			auto lay = new QVBoxLayout;
			lay->addWidget(_email);
			_email->setPlaceholderText(strMandatoryField);
			lay->addWidget(_emailErrorDesc);
			_email->validator()->_labelError = _emailErrorDesc;
			_emailErrorDesc->hide();
			form->addRow(tr("E-mail:"), lay);
		}
		form->addRow(tr("Your UID for retrieve vCard info:"), _ecard);
		_ecard->setPlaceholderText(tr("Optional field"));

		auto box = new QDialogButtonBox;
		lay->addWidget(box);
		_okBtn = box->addButton(QDialogButtonBox::Ok);
		auto cancel = box->addButton(QDialogButtonBox::Cancel);
		_okBtn->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-32.png"));
		cancel->setIcon(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-32.png"));
		connect(_okBtn, &QAbstractButton::clicked, this, &QDialog::accept);
		connect(cancel, &QAbstractButton::clicked, this, &QDialog::reject);
		connect(_email, &QLineEdit::textChanged, this, &TemplateDialog::enableOk);
		connect(_name, &QLineEdit::textChanged, this, &TemplateDialog::enableOk);
		connect(_ecard, &QLineEdit::textChanged, this, &TemplateDialog::enableOk);
		lay->addStretch();
		_okBtn->setEnabled(false);
	}
	bool allValid()const {
		if(_name->text().trimmed().isEmpty())
			return false;
		if(_email->text().isEmpty())//first - simplified check to prevent detailed description
			return false;
		return _email->hasAcceptableInput();
	}
	void enableOk() {
		_okBtn->setEnabled(allValid());
	}
	virtual void accept()override {
		if(allValid())
			QDialog::accept();
	}
};
void ManageSslPage::onCreate() {
	TemplateDialog dlg(this);
	if(dlg.exec()!=QDialog::Accepted)
		return;
	const QString name = dlg._name->text().trimmed();
	const QString mail = dlg._email->text().trimmed();
	const QString ecard = dlg._ecard->text().trimmed();
	QString contents = QString("/CN=%1/emailAddress=%2")
		.arg(name).arg(mail);
	if(!ecard.isEmpty())
		contents += "/UID=" + ecard;
	contents += '\n';
	QString fileNameTemplate = name + ' ' + mail;
	for(QChar & c : fileNameTemplate) {
		if(c.isDigit() || c.isLetter() || c==' ' || c=='.' || c=='@')
			c;
		else
			c = '.';
	}
	QString fileName = QUuid::createUuid().toString();
	fileName.remove('{');
	fileName.remove('-');
	fileName.remove('}');
	fileName += ".tpl";
	QDir dir = Settings::certDir();
	QString path = dir.absoluteFilePath(fileName);
	{
		QFile file(path);
		if(!file.open(QFile::WriteOnly)) {
			QMessageBox::critical(this, tr("Can't write file %1").arg(path), file.errorString());
			return;
		}
		auto arr = contents.toUtf8();
		auto written = file.write(arr);
		if(written!=arr.size())
			QMessageBox::critical(this, tr("Can't write file %1").arg(path), file.errorString());
	}
	_view->model()->reload();
	int index = _view->model()->indexByFile(path);
	Q_ASSERT(index!=-1);
	if(-1==index)
		return;
	_view->selectRow(index);
	_view->setFocus();
	_view->generateCertForSelectedRow();
}
void ManageSslPage::onDelete() {
	auto rows = _view->selectionModel()->selectedRows();
	_view->model()->removeRows(rows);
}