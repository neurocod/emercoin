//InfoCardsWidget.h by Konstantine Kozachuck as neurocod - 2018.01.24 22:07:30
#pragma once

class InfoCardsWidget: public QWidget {
	public:
		InfoCardsWidget(QWidget*parent=0);
		virtual ~InfoCardsWidget() {}
	protected:
		QPushButton* _btnDelete = 0;

		void onDelete();
		void onCreate();
		void onView();

		struct View;
		struct Model;
};