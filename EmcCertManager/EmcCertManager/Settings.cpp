//Settings.cpp by Konstantine Kozachuck as neurocod - 2018.01.26 18:31:56
#include "pch.h"
#include "Settings.h"

QDir Settings::certDir() {
	QDir dir = "D:\\g\\emcssl";
	//QDir dir = "C:\\Users\\neurocod\\AppData\\Roaming\\EmerCoin\\certificates";
	Q_ASSERT(dir.exists());
	return dir;
}