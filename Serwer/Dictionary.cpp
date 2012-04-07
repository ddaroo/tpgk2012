#include "stdafx.h"
#include "Dictionary.h"

CDictionary *dictionary;

ostream & operator<<(ostream &out, const CWord &rhs)
{
	return out << rhs.ptr;
}

CWord::CWord(const char *_ptr) : ptr(_ptr)
{
}

CWord::CWord(CWord &&rhs) : ptr(rhs.ptr)
{
	rhs.ptr = nullptr;
}

CWord::CWord(crstring string) : ptr(string.c_str())
{
}

CWord & CWord::operator=(CWord && rhs)
{
	ptr = rhs.ptr;
	rhs.ptr = nullptr;
	return *this;
}

bool CWord::operator<(const CWord &rhs) const
{
	return strcmp(ptr, rhs.ptr) < 0;
}

CDictionary::CDictionary(crstring fname)
{
	CHECK_TIME("Constructing dictionary - total");
	prepareMemory(fname); //alokacja
	readFile(fname); //przerzucenie zawartosci pliku
	formatMemory(); //konwersja do ciagu c-stringow (separacja NULLami)
	buildWordlist(); //budowa zbioru slow
	sortWordlist(); //ulozenie slow w naszym porzadku
	validateWordlist(); //sprawdzenie, czy sa po kolei

	dictionary = this;
}

void CDictionary::prepareMemory(crstring fname)
{
	CHECK_TIME("Preparing memory for dictionary");
	unsigned fileSize = (unsigned)boost::filesystem::file_size(fname);
	//alokacja pamieci o rozmiarze pliku plus troche
	memory.resize(fileSize + 50, 0);
}

void CDictionary::readFile(crstring fname)
{
	CHECK_TIME("Reading dictionary file");
	int totalReadCount = 0;
	ifstream file(fname);
	while(file)
	{
		//pierwszego bajtu nie ruszamy, chcemy miec wiodacy NULL (zeby pierwsze slowo tez z obu stron bylo otoczone NULL-ami)
		char * const whereToPutFile = &memory[totalReadCount] + 1;
		const int MAX_BYTES_TO_READ = memory.size() - 2;
		//wlasciwe czytanie
		file.read(whereToPutFile, MAX_BYTES_TO_READ); 
		totalReadCount += (int)file.gcount();
	}

	memory.resize(totalReadCount + 2); //pozbywamy sie zbednej przestrzeni z konca
}

void CDictionary::formatMemory()
{
	CHECK_TIME("Putting NULLs");
	replace(memory, '\n', '\0');
}

void CDictionary::buildWordlist()
{
	CHECK_TIME("Creating wordlist");
	words.reserve(EXPECTED_WORD_COUNT); //zeby sie nie realokowalo

	for (int i = 0; i < (int)memory.size()-1; i++)
		if(!memory[i])
			words.emplace_back(&memory[i+1]);
}

void CDictionary::validateWordlist()
{
	CHECK_TIME("Validating wordlist");
	for(int i = 0; i < (int)words.size() - 1; i++)
		if(!(words[i] < words[i+1]))
			LOGFL("Problem - word %d \"%s\" should be before word %d \"%s\"!", i % words[i] % (i+1) % words[i+1]);

	for(int i = 0; i < (int)words.size(); i++)
		if(!strlen(words[i].ptr))
			LOGFL("Problem - word %d is empty!", i);
}

void CDictionary::sortWordlist()
{
	CHECK_TIME("Sorting wordlist");
	sort(words);
}

bool CDictionary::contains(const CWord &word) const
{
	//CHECK_TIME("Dictionary lookup");
	return binary_search(words, word);
}
