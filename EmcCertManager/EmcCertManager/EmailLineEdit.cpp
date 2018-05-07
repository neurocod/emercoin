//EmailLineEdit.cpp by Konstantine Kozachuck as neurocod - 2018.01.28 15:01:53
#include "pch.h"
#include "EmailLineEdit.h"

EmailLineEdit::EmailLineEdit() {
	_validator = new EmailValidator(this);
	setValidator(_validator);
}