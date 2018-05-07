//EmailLineEdit.h by Konstantine Kozachuck as neurocod - 2018.01.28 15:01:53
#pragma once
#include "EmailValidator.h"

class EmailLineEdit: public QLineEdit {
	public:
		EmailLineEdit();
		EmailValidator* validator()const { return _validator; }
	protected:
		EmailValidator* _validator = 0;
};