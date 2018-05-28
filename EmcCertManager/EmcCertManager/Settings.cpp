//Settings.cpp by Konstantine Kozachuck as neurocod - 2018.01.26 18:31:56
#include "pch.h"
#include "Settings.h"

QDir Settings::certDir() {
	QDir dir = "D:\\g\\emcssl";
	//QDir dir = "C:\\Users\\neurocod\\AppData\\Roaming\\EmerCoin\\certificates";
	bool ok = dir.mkpath(".");
	Q_ASSERT(ok && dir.exists());
	return dir;
}