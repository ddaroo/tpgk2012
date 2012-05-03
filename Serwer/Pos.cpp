#include "Pos.h"

Pos::Pos()
{
	x = y = -1;
}

Pos::Pos(int _x, int _y)
{
	x = _x;
	y = _y;
}

Pos Pos::operator+(const Pos &rhs) const
{
	return Pos(x + rhs.x, y + rhs.y);
}

bool Pos::isValid() const
{
	return iswithin(x, 0, WIDTH-1) && iswithin(y, 0, HEIGHT - 1);
}

vector<Pos> Pos::validNeighbors() const
{
	vector<Pos> ret;
	FOREACH(const auto &dir, directions)
	{
		Pos n = *this + dir;
		if(n.isValid())
			ret.push_back(n);
	}
	return ret;
}

bool Pos::operator<(const Pos &rhs) const
{
	if(x != rhs.x)
		return x < rhs.x;
	return y < rhs.y;
}

Pos & Pos::operator+=(const Pos &rhs)
{
	x = x + rhs.x;
	y = y + rhs.y;
	return *this;
}

Pos Pos::operator-() const
{
	return Pos(-x, -y);
}

string Pos::toString() const
{
	string ret(3, 0);
	ret[0] = intToChar(x);
	ret[1] = intToChar(y);
	return ret;
}

void Pos::readData(boost::asio::ip::tcp::socket& sock) 
{
	uint16_t temp;
	read(sock, boost::asio::buffer(&temp, 2));
	x = ntohs(temp);
	read(sock, boost::asio::buffer(&temp, 2));
	y = ntohs(temp);
}

void Pos::writeData(boost::asio::ip::tcp::socket& sock) const
{
	uint16_t temp = htons(x);
	write(sock, boost::asio::buffer(&temp, 2));
	temp = htons(y);
	write(sock, boost::asio::buffer(&temp, 2));
}

bool Pos::operator!=(const Pos &rhs) const
{
	return x != rhs.x || y != rhs.y;
}

bool Pos::operator==(const Pos &rhs) const
{
	return !(*this != rhs);
}

char intToChar(int i)
{
	if(i < 10)
		return '0' + i;
	else
		return 'A' + i - 10;
}

ostream & operator<<(ostream &out, const Pos &p)
{
	return out << p.toString();
}

EOrientation flip(EOrientation orien)
{
	switch(orien)
	{
	case HORIZONTAL:	return VERTICAL;
	case VERTICAL:		return HORIZONTAL;
	default:			throw invalid_argument("flip called with invalid argument!");
	}
}

EOrientation orientationOfPos(const Pos &a, const Pos &b)
{
	const bool differOnX = a.x != b.x,
		differOnY = a.y != b.y;

	if(differOnX && differOnY)
		return INVALID; 
	if(differOnX)
		return HORIZONTAL;
	if(differOnY)
		return VERTICAL;
	return INVALID; //identyczne argumenty
}

Pos forwardDirection(EOrientation orientation)
{
	if(orientation == INVALID)
		throw invalid_argument("forwardDirection called with invalid argument!");

	return orientation == HORIZONTAL 
		? RIGHT 
		: DOWN;
}

Pos backwardDirection(EOrientation orientation)
{
	if(orientation == INVALID)
		throw invalid_argument("forwardDirection called with invalid argument!");

	return orientation==HORIZONTAL 
		? LEFT 
		: UP;
}

WordRange::WordRange(Pos f, Pos l) : first(f), last(l)
{

}

EOrientation WordRange::getOrientation() const
{
	EOrientation ret = orientationOfPos(first, last);
	assert(ret != INVALID);
	return ret;
}

int WordRange::length() const
{
	//special case needed, because at least two letters are needed to determine orientation
	if(first == last)
		return 1;

	auto coordinate = (getOrientation()==HORIZONTAL) ? (&Pos::x) : (&Pos::y);
	return 1 + last.*coordinate - first.*coordinate;
}