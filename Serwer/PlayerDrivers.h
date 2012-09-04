#pragma once
#include "Actions.h"

struct CBoard;
struct CPlayerState;
struct CRules;


struct IPlayerDriver
{
	int playerID;
	string playerName;

	virtual void init(const CRules &r) = 0;
	virtual TAction takeAction(const CBoard &b, const CPlayerState &ps) = 0;
	virtual void gameFinished() = 0;
	virtual ~IPlayerDriver(){}

};

struct CppDummyPlayer : IPlayerDriver
{
	CppDummyPlayer();
	virtual void init(const CRules &rul);
	virtual TAction takeAction(const CBoard &b, const CPlayerState &ps) override;
	virtual void gameFinished() override;
	const CRules* rules;
};

struct JavaPlayer : IPlayerDriver
{
	typedef boost::asio::ip::tcp::socket TSocket;
	typedef boost::asio::ip::tcp::acceptor TAcceptor;
	typedef boost::asio::ip::tcp::endpoint TEndpoint;
	typedef boost::asio::io_service TIoService;

	unique_ptr<boost::thread> systemCallThread;

	//polaczenie TCP
	TIoService io;
	TSocket socket;

	JavaPlayer(string programName);
	virtual void init(const CRules& r);
	virtual TAction takeAction(const CBoard &b, const CPlayerState &ps) override;
	virtual void gameFinished() override;
	virtual ~JavaPlayer();

	void przyjmijPolaczenie(TAcceptor &acceptor, int portNumber);;

};