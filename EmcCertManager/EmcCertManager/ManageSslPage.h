//ManageSslPage.h by Konstantine Kozachuck as neurocod
#pragma once
class CertTableView;

class ManageSslPage: public QWidget {
	public:
		ManageSslPage(QWidget*parent=0);
		virtual ~ManageSslPage() {}
	protected:
		QAbstractButton* _btnDelete = 0;

		void onDelete();
		void onCreate();

		CertTableView* _view = 0;
		struct TemplateDialog;
		struct Logger;
		Logger* _logger = 0;
};