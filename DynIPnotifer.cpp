/*
 * DynIPnotifer.cpp
 *
 *  Created on: 26 февр. 2016 г.
 *      Author: secator
 */

#include "DynIPnotifer.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <regex>
#include <unistd.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/NetException.h>
#include "Config.h"
#include "Logger.h"

using namespace std;
using namespace Poco::Net;
using namespace Poco;

DynIPnotifer::DynIPnotifer(Config *conf, Logger *log_link) {
	log = log_link;
	this->ip = "";
	this->counter = 0;
	this->getip = conf->Get("getip", 0);
	this->smtp = conf->Get("smtp", 0);
	this->from = conf->Get("from", 0);
	this->to = conf->Get("to", 0);
	this->password = conf->Get("password", 0);
	if (conf->GetCount("getiptimer"))
		this->getiptimer = stoi(conf->Get("getiptimer", 0));
	else
		this->getiptimer = -1;
}

DynIPnotifer::~DynIPnotifer() {

}

void DynIPnotifer::Main() {
	this->counter++;
	if (this->counter == this->getiptimer) {
		string temp_ip = "";
		temp_ip = this->GetIP();
		if (temp_ip == "error") {
			this->counter = 0;
			log->log("DynIPnotifer->error: can't get ip.", NULL);
			return;
		}
		if (this->ip == temp_ip) {
			this->counter = 0;
			return;
		} else {
			this->ip = temp_ip;
			if (!this->SendMail())
				log->log("DynIPnotifer->error: can't send email.", NULL);
		}
		this->counter = 0;
		return;
	}
}

bool DynIPnotifer::SendMail() {
	UInt16 port = 25;
	string subject = "IP  Changed";
	subject = MailMessage::encodeWord(subject, "UTF-8");
	string content = "Now ip is: " + this->ip + ".";
	MailMessage message;
	message.setSender(this->from);
	message.addRecipient(
			MailRecipient(MailRecipient::PRIMARY_RECIPIENT, this->to));
	message.setSubject(subject);
	message.setContentType("text/plain; charset=UTF-8");
	message.setContent(content, MailMessage::ENCODING_8BIT);
	try {
		SMTPClientSession session(this->smtp, port);
		session.open();
		try {
			session.login(SMTPClientSession::AUTH_LOGIN, this->from,
					this->password);
			session.sendMessage(message);
			log->log(
					"DynIPnotifer->SendMail: Message successfully sent. Ip is:",
					this->ip.c_str(), NULL);
			session.close();
			return true;
		} catch (SMTPException &e) {
			log->log("DynIPnotifer->SendMail->error: ", e.displayText().c_str(),
			NULL);
			session.close();
			return false;
		}
	} catch (NetException &e) {
		log->log("DynIPnotifer->SendMail->error: ", e.displayText().c_str(),
		NULL);
		return false;
	}
	return true;
}

string DynIPnotifer::GetIP() {
	string buffer;
	try {
		URI uri(this->getip);
		HTTPClientSession session(uri.getHost(), uri.getPort());
		string path(uri.getPathAndQuery());
		if (path.empty())
			path = "/";
		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		session.sendRequest(req);
		HTTPResponse res;
		log->log("DynIPnotifer->GetIP: Reason: ", res.getReason().c_str(),
		NULL);
		istream &is = session.receiveResponse(res);
		StreamCopier::copyToString(is, buffer);
	} catch (Exception &ex) {
		log->log("DynIPnotifer->GetIP->error:", ex.displayText().c_str(), NULL);
		return "error";
	}
	smatch match;
	regex reg("<div id=\"my_ip\">(.+)</div>");
	if (regex_search(buffer, match, reg))
		return match[1];
	return "error";
}

