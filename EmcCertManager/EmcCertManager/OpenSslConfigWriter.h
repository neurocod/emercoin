//OpenSslConfigWriter.h by Emercoin developers - 2018.05.28 01:55:26
#pragma once

class OpenSslConfigWriter {
	public:
		static QString writeCA_EC();
		static QString writeCA_RSA();
		static QString writeCA_WINCONF();
	protected:
		static QString writeIfAbsent(const QString & subPath, const QString & contents);
};