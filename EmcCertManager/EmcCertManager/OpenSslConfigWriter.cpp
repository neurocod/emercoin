﻿//OpenSslConfigWriter.cpp by Emercoin developers - 2018.05.28 01:55:26
#include "pch.h"
#include "OpenSslConfigWriter.h"
#include "ShellImitation.h"
#include "Settings.h"

QString OpenSslConfigWriter::writeIfAbsent(const QString & subPath, const QString & contents) {
	const QString path = Settings::certDir().absoluteFilePath(subPath);
	if(QFile::exists(path))
		return QString();
	QString ret;
	ShellImitation::write(path, contents.toUtf8(), ret);
	return ret;
}
QString OpenSslConfigWriter::writeCA_EC() {
	return writeIfAbsent("CA-EC/ca.config",
R"HOBOT([ ca ]
default_ca             = CA_CLIENT       # При подписи сертификатов
# использовать секцию CA_CLIENT

[ CA_CLIENT ]
dir                    = ./db            # Каталог для служебных файлов
certs                  = $dir/certs      # Каталог для сертификатов
new_certs_dir          = $dir/newcerts   # Каталог для новых сертификатов

database               = $dir/index.txt  # Файл с базой данных
# подписанных сертификатов
serial                 = $dir/serial     # Файл содержащий серийный номер
# сертификата
# (в шестнадцатиричном формате)
certificate            = CA-EC/emcssl_ca.crt        # Файл сертификата CA
private_key            = CA-EC/emcssl_ca.key        # Файл закрытого ключа CA

default_days           = 1825            # Срок действия подписываемого 
					 # сертификата - 5 лет
default_crl_days       = 7               # Срок действия CRL (см. $4)
default_md             = sha256          # Алгоритм подписи
policy                 = policy_anything # Название секции с описанием
# политики в отношении данных
# сертификата
x509_extensions=cert_v3		# For v3 certificates.



[ policy_anything ]
countryName            = optional        # Код страны - не обязателен
stateOrProvinceName    = optional        # ......
localityName           = optional        # ......
organizationName       = optional        # ......
organizationalUnitName = optional        # ......
commonName             = supplied        # ...... - обязателен
emailAddress           = optional        # ......
userId                 = optional	 # Reference to external info
#domainComponent	       = optional	 # EMC payment address
#surname		       = optional        # 
#givenName	       = optional        # 
#pseudonym	       = optional        # 
#

[ cert_v3 ]
# With the exception of 'CA:FALSE', there are PKIX recommendations for end-user
# # certificates that should not be able to sign other certificates.
# # 'CA:FALSE' is explicitely set because some software will malfunction without.

 subjectKeyIdentifier=	hash
 basicConstraints=	CA:FALSE
 keyUsage=		nonRepudiation, digitalSignature, keyEncipherment

 nsCertType=		client, email
 nsComment=		"EmerCoin EMCSSL PKI"

 authorityKeyIdentifier=keyid:always,issuer:always
)HOBOT");
}