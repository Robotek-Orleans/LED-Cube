#pragma once

#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>
#include <RapidJson/writer.h>
#include <Mongoose/mongoose.h>
#include <string>
#define BCRYPT_FROM_BRCYPT_CPP 1
#include <bcrypt.h> // hash password
#undef BCRYPT_FROM_BRCYPT_CPP
#include "JsonMessage.h"

#ifndef BCRYPT_SHA384_ALG_HANDLE
#define BCRYPT_SHA384_ALG_HANDLE ((BCRYPT_ALG_HANDLE)0x00000051)
#endif

// A connection and its challenge
// (When the client is logging in)
class ClientChallenge
{
public:
	ClientChallenge(struct mg_connection *c);
	~ClientChallenge();

	std::string getChallenge() const;
	bool onChallengeAnswered(const std::string &clientChallenge, const std::string &receivedHash, const std::string &serverPassword);
	bool isConnection(mg_connection *connection);
	void sendChallenge();

private:
	std::string hash(const std::string key, const std::string salt);

	struct mg_connection *connection;
	const std::string serverChallenge;
};