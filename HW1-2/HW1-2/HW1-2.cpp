#include <iostream>
#include <sstream >
#include "Time_fromGithub.cpp"
#pragma warning (disable : 4996)

//File Time_fromGithub.cpp is the time class from 
//https://github.com/Angeld55/Object-oriented_programming_FMI/tree/master/Week%2004

namespace GlobalConstants
{
	constexpr int MAX_NAME_SIZE = 20 + 1;
	constexpr int DATA_MAX_LENGTH = 256;
	constexpr int MAX_SONGS_COUNT = 30;
}

enum class Types {
	Pop = 1 << 0,
	Rock = 1 << 1,
	HipHop = 1 << 2,
	Electro = 1 << 3,
	Jazz = 1 << 4
};

size_t getFileSize(std::ifstream& ifs);


class Song {
private:
	char name[GlobalConstants::MAX_NAME_SIZE] = "\0";
	Time duration;
	unsigned char type = 0;
	unsigned char data[GlobalConstants::DATA_MAX_LENGTH];
	size_t dataLength = 0;

public:
	Song() = default;
	Song(const char* name, unsigned hours, unsigned minutes, unsigned seconds, const char* type, const char* fileName) {
		setName(name);
		setTime(hours, minutes, seconds);
		setType(type);
		setData(fileName);
	}
	void setName(const char* name) {
		if (strlen(name) >= GlobalConstants::MAX_NAME_SIZE) {
			std::cout << "Invalid name!" << std::endl;
			return;
		}
		else {
			strcpy(this->name, name);
		}
	}
	void setTime(unsigned hours, unsigned minutes, unsigned seconds) {
		duration.setHours(hours);
		duration.setMins(minutes);
		duration.setSeconds(seconds);

	}
	void setType(const char* type) {
		std::stringstream ss(type);
		
		while (!ss.eof()) {
			char ch = ss.get();
			switch (ch) {
			case 'p': this->type = this->type | int(Types::Pop); break;
			case 'r': this->type = this->type | int(Types::Rock); break;
			case 'h': this->type = this->type | int(Types::HipHop); break;
			case 'e': this->type = this->type | int(Types::Electro); break;
			case 'j': this->type = this->type | int(Types::Jazz); break;
			default: break;
			}
		}
	}
	void setData(const char* fileName) {
		std::ifstream ifs(fileName, std::ios::binary);
		if (!ifs.is_open()) {
			std::cout << "Failed loading data from file!" << std::endl;
		}
		else {
			size_t fileSize = getFileSize(ifs);
			if (fileSize > GlobalConstants::DATA_MAX_LENGTH) {
				std::cout << "File is too big!" << std::endl;
				return;
			}
			ifs.read((char*) data, fileSize);
			dataLength = fileSize;

		}
	}

	const char* getName() const {
		return name;
	}
	unsigned char getType() const {
		return type;
	}
	const unsigned char* getData() const {
		return data;
	}
	size_t getDataLength() const {
		return dataLength;
	}
	const Time& getDuration() const {
		return duration;
	}

	void setKBits(unsigned period) {
		unsigned char* mask = new unsigned char[dataLength];

		for (int i = 0; i < dataLength; i++) {
			mask[i] = 0;
		}
		unsigned char tempMask = 0;
		for (size_t i = 0, j = dataLength - 1 - (i / 8); i < dataLength * 8; i++, j = dataLength - 1 - (i / 8))
		{
			if (i % 8 == 0) {
				tempMask = unsigned char(1);
				
			}

			if (i % period == period - 1) {
				mask[j] = mask[j] | tempMask;
			}

			tempMask = tempMask << 1;
		}

		
		
		for (int i = 0; i < dataLength; i++) {
			//std::cout << int(data[i]) << " ";
			//std::cout << int(mask[i]) << " ";
			//std::cout << int(mask[i]) << " ";
			data[i] = data[i] | mask[i];
			//std::cout << int(data[i]) << std::endl;
			
		}
		
		delete[] mask;
	}

	void mixWith(const Song& other) {
		size_t shorterSongLength = this->dataLength < other.dataLength ? this->dataLength: other.dataLength;
	
		for (size_t i = 0; i < shorterSongLength; i++) {
			this->data[i] = this->data[i] ^ other.data[i];
		}
	}

	void printSong() const {
		std::cout << name << ", ";
		duration.serialize(std::cout);
		std::cout << ", ";
		printType();
		std::cout << "\n";

	}

	void printData() const {
		for (int i = 0; i < dataLength; i++) {
			std::cout << int(data[i]) << " ";
		}
		std::cout << std::endl;
	}
	
	void printType() const {
		unsigned char mask;
		bool isFirst = true;
		for (size_t i = 1; i <= 5; i++) {
			mask = unsigned char(1);
			mask = mask << i - 1;
			switch (type & mask) {
				case int(Types::Rock) : {
					if (isFirst) { 
						std::cout << "Rock"; 
						isFirst = false;
					}
					else {
						std::cout << "&Rock";
					}
					break; 
				}
				case int(Types::Pop) : {
					if (isFirst) {
						std::cout << "Pop";
						isFirst = false;
					}
					else {
						std::cout << "&Pop";
					}
					break;
				}
				case int(Types::HipHop) : {
					if (isFirst) {
						std::cout << "HipHop";
						isFirst = false;
					}
					else {
						std::cout << "&HipHop";
					}
					break;
				}
				case int(Types::Jazz) : {
					if (isFirst) {
						std::cout << "Jazz";
						isFirst = false;
					}
					else {
						std::cout << "&Jazz";
					}
					break;
				}
				case int(Types::Electro) : {
					if (isFirst) {
						std::cout << "Electro";
						isFirst = false;
					}
					else {
						std::cout << "&Electro";
					}
					break;
				}
			}
		}
	}

};

class Playlist {
private:
	Song songs[GlobalConstants::MAX_SONGS_COUNT];
	size_t size = 0;

public:
	//No constructors setter and getters because they arent needed for the task
	bool addSong(const char* name, unsigned hours, unsigned minutes, unsigned seconds, 
				 const char* type, const char* fileName) {
		if (size == GlobalConstants::MAX_SONGS_COUNT) {
			std::cout << "Max songs reached!" << std::endl;
			return false;
		}
		else {
			songs[size++] = Song(name, hours, minutes, seconds, type, fileName);
			return true;
		}
	}
	bool addSong(const Song& other) {
		if (size == GlobalConstants::MAX_SONGS_COUNT) {
			std::cout << "Max songs reached!" << std::endl;
			return false;
		}
		else {
			songs[size++] = other;
			return true;
		}
	}
	void printSongs() const {
		for (size_t i = 0; i < size; i++) {
			songs[i].printSong();
		}
	}
	const Song* searchSongByName(const char* songName) const {
		for (size_t i = 0; i < size; i++) {
			if (strcmp(songs[i].getName(), songName) == 0) {
				return &songs[i];
			}
		}
		return nullptr;
	}
	Playlist searchSongsByType(const char* type) const {
		Song tempSong;
		tempSong.setType(type);
		Playlist returnPlaylist;

		for (size_t i = 0; i < size; i++) {
			if (tempSong.getType() & songs[i].getType()) {
				returnPlaylist.addSong(songs[i]);
			}
		}
		return returnPlaylist;
	}
	void sortByPred(bool(*pred)(const Song&,const Song&)) {
		for (size_t i = 0; i < size; i++) {
			size_t maxDurationIndex = i;

			for (size_t j = i + 1; j < size; j++) {
				if (pred(songs[maxDurationIndex], songs[j]))
					maxDurationIndex = j;
			}
			Song tempSwap = songs[i];
			songs[i] = songs[maxDurationIndex];
			songs[maxDurationIndex] = tempSwap;
		}

	}
	bool modifySong(const char* songName, size_t k) {
		for (size_t i = 0; i < size; i++) {
			if (strcmp(songs[i].getName(), songName) == 0) {
				songs[i].setKBits(k);
				return true;
			}
		}
		return false;
	}
	bool modifySong(const char* songName1, const char* songName2) {
		const Song* mixWithSong = searchSongByName(songName2);
		if (mixWithSong)
		{
			for (size_t i = 0; i < size; i++) {
				if (strcmp(songs[i].getName(), songName1) == 0) {
					songs[i].mixWith(*mixWithSong);
					return true;
				}
			}
			return false;
		}
		else {
			return false;
		}
		
	}
	bool safeToFile(const char* songName, const char* fileName) const {
		std::ofstream ofs(fileName, std::ios::binary);

		if (!ofs.is_open()) {
			std::cout << "Failed onpening file!\n";
			return false;
		}
		else {
			const Song* songToSafe = this->searchSongByName(songName);
			ofs.write((const char*)songToSafe->getData(), songToSafe->getDataLength());
		}
	}


};

size_t getFileSize(std::ifstream& ifs) {
	size_t curPos = ifs.tellg();
	ifs.seekg(0, std::ios::end);
	size_t size = ifs.tellg();
	ifs.seekg(curPos);
	return size;

}

bool compareByDuration(const Song& s1, const  Song& s2) {
	const Time& ref1 = s1.getDuration();
	return ref1.compare(s2.getDuration()) >= 0;
}

bool compareByName(const Song& s1, const  Song& s2) {
	return strcmp(s1.getName(), s2.getName()) == 1;
}



int main() {
	Song s1("s1", 0, 0, 12, "pr", "song1.dat");
	Song s2("s2", 0, 1, 12, "pe", "song2.dat");
	Song s3("s3", 0, 0, 59, "r", "song3.dat");

	std::cout << "Created song s1: ";
	s1.printSong();

	Playlist pl;
	pl.addSong(s3);
	pl.addSong(s1);
	pl.addSong(s2);

	std::cout << "Print songs:\n";
	pl.printSongs();
	std::cout << std::endl;

	std::cout << "Sort by name:\n";
	pl.sortByPred(compareByName);
	pl.printSongs();
	std::cout << std::endl;

	std::cout << "Sort by duration:\n";
	pl.sortByPred(compareByDuration);
	pl.printSongs();
	std::cout << std::endl;

	std::cout << "Searched for song s1: "; 
	pl.searchSongByName("s1")->printSong();
	std::cout << "Searched for song s2: ";
	pl.searchSongByName("s2")->printSong();
	std::cout << std::endl;

	std::cout << "Searched for Rock songs:\n";
	Playlist pl2 = pl.searchSongsByType("r");
	pl2.printSongs();
	std::cout << std::endl;



	std::cout << std::endl;

	std::cout << "Song s1 data: ";
	pl.searchSongByName("s1")->printData();
	std::cout << "Song s2 data: ";
	pl.searchSongByName("s2")->printData();

	std::cout << "Mixing s2 with s1:\n";
	pl.modifySong("s2", "s1");

	std::cout << "Song s1 data: ";
	pl.searchSongByName("s1")->printData();
	std::cout << "Song s2 data: ";
	pl.searchSongByName("s2")->printData();
	std::cout << std::endl;

	std::cout << "Remix on s1, every 3 bits:\n";
	std::cout << "Song s1 data: ";
	pl.searchSongByName("s1")->printData();
	pl.modifySong("s1", 3);
	std::cout << "Song s1 data: ";
	pl.searchSongByName("s1")->printData();
	std::cout << std::endl;

	std::cout << "Remix on s2, every 4 bits:\n";
	std::cout << "Song s2 data: ";
	pl.searchSongByName("s2")->printData();
	pl.modifySong("s2", 4);
	std::cout << "Song s2 data: ";
	pl.searchSongByName("s2")->printData();
	std::cout << std::endl;

	
	
	//pl.searchSongByName("s2")->printData();
	/*pl.modifySong("s1", 2);
	const Song* ptr = pl.searchSongByName("s1");
	ptr->printData();*/
	//pl.modifySong("s1", 5);

	/*Playlist pl2 = pl.searchSongsByType("j");
	pl2.printSongs();
	std::cout << std::endl;
	pl.safeToFile("s2", "song4.dat");
	pl2.addSong(Song("s2", 0, 0, 1, "j", "song4.dat"));

	pl.searchSongByName("s2")->printData();
	pl2.searchSongByName("s2")->printData();
	*/
	
	
	return 0;
}
