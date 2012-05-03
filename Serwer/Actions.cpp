#include "Actions.h"

void ExchangeLetters::readData(boost::asio::ip::tcp::socket& sock)
{
	char size, temp;
	read(sock, boost::asio::buffer(&size, 1));
	for(int i = 0; i < size; ++i)
	{
		read(sock, boost::asio::buffer(&temp, 1));
		letters.push_back(temp);
	}
}

void PutLetters::readData(boost::asio::ip::tcp::socket& sock)
{
	char temp, size;
	read(sock, boost::asio::buffer(&temp, 1));
	orientation = static_cast<EOrientation>(temp);
	read(sock, boost::asio::buffer(&size, 1));
	PutLetter plet;
	for(int i = 0; i < size; ++i)
	{
		read(sock, boost::asio::buffer(&plet.letter, 1));
		read(sock, boost::asio::buffer(&plet.generateFromBlank, 1));
		plet.pos.readData(sock);
	}
}