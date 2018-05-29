//OpenSslConfigWriter.h by Emercoin developers - 2018.05.28 01:55:26
#pragma once

class OpenSslConfigWriter {
	public:
		static QString checkAndWrite();
	protected:
		static bool writeIfAbsent(const QString & subPath, const char* strContents, QString & error);
};