#pragma once

struct CWord : boost::noncopyable
{
	const char *ptr; //wskaznik na slowo (terminowany NULL-em string)

	CWord(crstring string); //konstruktor konwertujacy
	CWord(const char *_ptr); //konstruktor konwertujacy
	CWord(CWord &&rhs); //konstruktor kradnacy

	CWord & operator=(CWord && rhs); //przypisanie przenoszace -> potrzebne do sortowania
	bool operator<(const CWord &rhs) const; //porownanie slow wg porzadku strcmp
};

ostream & operator<<(ostream &out, const CWord &rhs);

class CDictionary : boost::noncopyable
{
	vector<char> memory; //we keep all words in one big memory block -> kazde slowo to ciag znakow odseparowany z obu stron NULL-ami (pierwszy bajt to tez NULL!)


	vector<CWord> words; //indeks slow

	//helpers for creating -> funkcje nalezy wolac w kolejnosci deklaracji 
	void prepareMemory(crstring fname); //alokuje pamiec
	void readFile(crstring fname); //przerzuca plik do pamieci
	void formatMemory(); //zamienia znaki nowej linii na NULL-e -> slowa w pamieci staja sie C-style stringami
	void buildWordlist(); //przeglada pamiec, buduje spis slow
	void sortWordlist(); //sortuje w porzadku strcmp (bedzie mozna wyszukiwac binarnie wg tego)
	void validateWordlist(); //upewniamy sie, ze lista jest po kolei

public:

	CDictionary(crstring fname);
	~CDictionary(){}

	bool contains(const CWord &word) const;
};

extern CDictionary *dictionary;