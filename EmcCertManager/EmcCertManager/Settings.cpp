﻿//Settings.cpp by Konstantine Kozachuck as neurocod - 2018.01.26 18:31:56
#include "pch.h"
#include "Settings.h"

QDir Settings::certDir() {
	//TODO
	QDir dir = "D:\\g\\emcssl";
	Q_ASSERT(dir.exists());
	return dir;
}