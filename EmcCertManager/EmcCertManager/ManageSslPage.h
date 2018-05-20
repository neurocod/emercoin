//ManageSslPage.h by Konstantine Kozachuck as neurocod
#pragma once
class CertTableView;

class ManageSslPage: public QWidget {
	public:
		ManageSslPage(QWidget*parent=0);
	protected:
		QAbstractButton* _btnDelete = 0;

		void onDelete();
		void onCreate();
		void enableDeleteButton();

		CertTableView* _view = 0;
		struct TemplateDialog;
		struct Logger;
		Logger* _logger = 0;
};