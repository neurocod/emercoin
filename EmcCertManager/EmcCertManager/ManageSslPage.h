//ManageSslPage.h by Konstantine Kozachuck as neurocod
#pragma once
class CertTableView;
class CertLogger;

class ManageSslPage: public QWidget {
	public:
		ManageSslPage(QWidget*parent=0);
	protected:
		QAbstractButton* _btnDelete = 0;

		void onDelete();
		void onCreate();
		void enableDeleteButton();
		void reloadLog();

		CertTableView* _view = 0;
		struct TemplateDialog;
		CertLogger* _logger = 0;
};