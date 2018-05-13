//CertTableView.h by Konstantine Kozachuck as neurocod - 2018.02.08 16:10:37
#pragma once
#include "TableView.h"
class CertTableModel;

class CertTableView: public TableView {
	public:
		CertTableView();
		using Model = CertTableModel;
		Model* model()const;
	protected:
		void recreateButtons();
		void onGenerateCert();
		void showInExplorer();
		struct Dialog;
		CertTableModel* _model = 0;
};