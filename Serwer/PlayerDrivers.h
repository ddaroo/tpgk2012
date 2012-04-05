#pragma once
#include "Actions.h"

struct CBoard;
struct CPlayerState;
struct CRules;


struct IPlayerDriver
{
	int playerID;

	virtual void init() = 0;
	virtual TAction takeAction(const CBoard &b, const CPlayerState &ps, const CRules &r) = 0;
	virtual ~IPlayerDriver(){}

};

struct CppDummyPlayer : IPlayerDriver
{
	void init();
	virtual TAction takeAction(const CBoard &b, const CPlayerState &ps, const CRules &r) override;
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
	void init();
	virtual TAction takeAction(const CBoard &b, const CPlayerState &ps, const CRules &r) override;
	~JavaPlayer();
};