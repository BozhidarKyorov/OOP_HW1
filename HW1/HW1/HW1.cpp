#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#pragma warning (disable : 4996)

void readCharacterReferences(char*);


namespace GlobalConstants
{
	constexpr int FIELD_MAX_SIZE = 50;
	constexpr int MAX_FIELDS_ROW = 30;
	constexpr int ROWS_MAX_SIZE = 300;
	constexpr int BUFFER_SIZE = 1024;
	constexpr char SEP = '|';
}

void removeSpaces(char* tag) {

	char newTag[GlobalConstants::FIELD_MAX_SIZE];
	size_t newTagCounter = 0;
	for (size_t i = 0; i < strlen(tag); i++) {
		if (tag[i] != ' ') {
			newTag[newTagCounter++] = tag[i];
		}
	}
	newTag[newTagCounter] = '\0';
	strcpy(tag, newTag);
}


struct Field {
	char data[GlobalConstants::FIELD_MAX_SIZE] = "\0";
	bool isHeader = false;
};

struct Row {
	Field fields[GlobalConstants::MAX_FIELDS_ROW];

	size_t parseRow(const char* row) {
		std::stringstream ss(row);

		size_t currentColumnCount = 0;
		while (!ss.eof())
		{
			ss.getline(fields[currentColumnCount++].data, GlobalConstants::FIELD_MAX_SIZE, GlobalConstants::SEP);
		}
		return currentColumnCount;
	}
	size_t getFieldsCount() const {
		size_t counter = 0;
		while (strcmp(fields[counter++].data, "") != 0);
		return counter;
	}
	void printRow(size_t fieldsCount, const size_t* fieldsSize) const {
		for (size_t i = 0; i < fieldsCount; i++) {
			if (fields[i].isHeader) {
				std::cout << "|*" << std::left << std::setw(fieldsSize[i])
					<< fields[i].data << "*";
			}
			else {
				std::cout << "| " << std::left << std::setw(fieldsSize[i])
					<< fields[i].data << " ";
			}
		}
		std::cout << "|\n";
	}
};

class HTMLTable {
public:

	Row rows[GlobalConstants::ROWS_MAX_SIZE];
	size_t rowsCount = 0;
	size_t collsCount = 0;
	size_t fieldMaxLength[GlobalConstants::MAX_FIELDS_ROW] = {1};



	bool add(size_t rowNumber, const char* row) {
		if (rowNumber >= GlobalConstants::ROWS_MAX_SIZE || rowNumber == 0) {
			std::cout << "Row number out of bound!\n";
			return false;
		}
		else
		{
			if (rowNumber>rowsCount) {
				rowsCount = rowNumber;
			}

			size_t fieldsCount = rows[rowNumber - 1].parseRow(row);

			if (fieldsCount > collsCount) {
				collsCount = fieldsCount;
			}
			for (size_t i = 0; i < fieldsCount; i++) {
				if (strlen(rows[rowNumber - 1].fields[i].data) > fieldMaxLength[i]) {
					fieldMaxLength[i] = strlen(rows[rowNumber - 1].fields[i].data);
				}

			}
		}
		return true;

	}
	bool remove(size_t rowNumber) {
		if (rowNumber > rowsCount || rowNumber == 0) {
			std::cout << "Row number out of bound!\n";
			return false;
		}

		if (rows[rowNumber - 1].getFieldsCount() - 1 == collsCount) {
			collsCount = 0;
		}

		for (size_t i = 0; i < rowsCount - 1; i++)
		{
			size_t tempFieldsCount = rows[i].getFieldsCount();
			if (rowNumber - 1 == i)
				tempFieldsCount = 0;
			if (tempFieldsCount > collsCount) {
				collsCount = tempFieldsCount;
			}

			if (i >= rowNumber - 1) {
				rows[i] = rows[i + 1];
			}
		}
		rowsCount--;
	}
	bool edit(size_t rowNumber, size_t collNumber, const char* newValue) {
		if (rowNumber > rowsCount || collNumber > collsCount || rowNumber == 0 || collNumber == 0) {
			std::cout << "Row number out of bound!\n";
			return false;
		}
		strcpy(rows[rowNumber - 1].fields[collNumber - 1].data, newValue);
		if (strlen(newValue) > fieldMaxLength[collNumber - 1]) {
			fieldMaxLength[collNumber - 1] = strlen(newValue);
		}
	}
	
	void print() const
	{
		for (size_t i = 0; i < rowsCount; i++) {
			rows[i].printRow(collsCount, fieldMaxLength);
		}
		std::cout << "\n";
	}
};

bool readTable(const char* fileName, HTMLTable& table) {
	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		std::cout << "Failed to open file!" << std::endl;
		return false;
	}
	char row[GlobalConstants::BUFFER_SIZE];
	size_t curCollumn = 0;
	//size_t currentRow = 0;
	while (!ifs.eof()) {
		ifs.getline(row, GlobalConstants::BUFFER_SIZE, '\n');
		//std::cout << "currRow: " << row << std::endl;
		std::stringstream ss(row);
		

		//size_t currentField = -1;
		while (!ss.eof()) {
			char tag[GlobalConstants::FIELD_MAX_SIZE];
			char field[GlobalConstants::FIELD_MAX_SIZE];

			ss.getline(tag, GlobalConstants::FIELD_MAX_SIZE, '>');
			removeSpaces(tag);
			if (strcmp(tag, "\0") == 0) {
				continue;
			}
			//std::cout << tag << std::endl;
			strcat(tag, ">");

			

			if (strcmp(tag, "<tr>") == 0) {
				if (table.collsCount < curCollumn + 1) {
					table.collsCount = curCollumn + 1;
				}
				curCollumn = 0;
			}
			else if (strcmp(tag, "</tr>") == 0) {
				table.rowsCount++;
			}
			else if (strcmp(tag, "<th>") == 0) {
				ss.get(field, GlobalConstants::FIELD_MAX_SIZE, '<');
				readCharacterReferences(field);

				strcpy(table.rows[table.rowsCount].fields[curCollumn].data, field);
				table.rows[table.rowsCount].fields[curCollumn].isHeader = true;


				//std::cout << "headerField: " << field << std::endl;
				if (strcmp(field, "\0") == 0)
					ss.clear();

				if (table.fieldMaxLength[curCollumn] < strlen(field)) {
					table.fieldMaxLength[curCollumn] = strlen(field);
				}
				curCollumn++;

			}
			else if (strcmp(tag, "<td>") == 0) {
				ss.get(field, GlobalConstants::FIELD_MAX_SIZE, '<');
				readCharacterReferences(field);
				strcpy(table.rows[table.rowsCount].fields[curCollumn].data, field);
				table.rows[table.rowsCount].fields[curCollumn].isHeader = false;

				//std::cout << "notheaderField: " << field << std::endl;
				if (strcmp(field, "\0") == 0)
					ss.clear();
				

				if (table.fieldMaxLength[curCollumn] < strlen(field)) {
					table.fieldMaxLength[curCollumn] = strlen(field);
				}
				
				curCollumn++;

			}
			

		}
		
	}
	return true;
}

void readCharacterReferences(char* data) {
	std::stringstream ss(data);
	char ch;
	char code[4];

	while (!ss.eof()) {
		ch = char(ss.get());
		if (ch == '&') {
			ch = char(ss.get());
			if (ch == '#') {
				ss.getline(code, 3);
			}
			else {
				ss.seekg(-1, std::ios::cur);
				ss.getline(code, 4);
			}

			std::stringstream stringToNum(code);
			int num;
			stringToNum >> num;
			//std::cout << char(num);
			bool f = false;
			size_t  i = 0;
			for (size_t j = 3; j < strlen(data); i++, j++) {
				if (data[i] == '&') {
					data[i] = char(num);
					f = true;
					continue;
				}
				if (f) {
					data[i] = data[j];
				}
			}
			data[i] = '\0';


			return;
		}
	}
}

bool saveToFile(const char* fileName, const HTMLTable& table) {
	std::ofstream ofs(fileName, std::ios::trunc);
	if (!ofs.is_open()) {
		std::cout << "Failed opening file!" << std::endl;
		return false;;
	}
	else {
		ofs << "<table>\n";
		for (size_t i = 0; i < table.rowsCount; i++) {
			ofs << "  <tr>\n";
			for (size_t j = 0; j < table.collsCount; j++) {
				if (strcmp(table.rows[i].fields[j].data, "\0") == 0) {
					continue;
				}
				else {
					if (table.rows[i].fields[j].isHeader) {
						ofs << "    <th>" << table.rows[i].fields[j].data << "</th>\n";
					}
					else {
						ofs << "    <td>" << table.rows[i].fields[j].data << "</td>\n";

					}

				}
			}
			ofs << "  </tr>\n";
		}
		ofs << "</table>\n";

	}
	return true;
}



int main()
{
	HTMLTable table;
	char command[GlobalConstants::BUFFER_SIZE];
	std::cout << "Enter file name: ";
	std::cin >> command;
	if (!readTable("table.html", table)) {
		std::cout << "Failed loading file!\n";
		return -1;
	}
	while (true) {
		
		std::cout << "Commands: add, remove, edit, print, save, quit\n";
		std::cin >> command;
		std::cout << command << std::endl;

		if (strcmp(command, "add") == 0) {
			int rowNumber = 0;
			char rowText[GlobalConstants::BUFFER_SIZE];
			std::cout << "row number: ";
			std::cin >> rowNumber;
			std::cin.ignore();
			std::cout << "row (use '|' as separator between rows): ";
			std::cin.getline(rowText, GlobalConstants::BUFFER_SIZE, '\n');

			if (!table.add(rowNumber, rowText))
				return -1;

		}
		else if (strcmp(command, "remove") == 0) {
			int rowNumber = 0;
			std::cout << "row number: ";
			std::cin >> rowNumber;
			if (!table.remove(rowNumber))
				return -1;
		}
		else if (strcmp(command, "edit") == 0) {
			int rowNumber = 0;
			int collNumber = 0;
			char field[GlobalConstants::BUFFER_SIZE];
			std::cout << "row number: ";
			std::cin >> rowNumber;
			std::cout << "coll number: ";
			std::cin >> collNumber;
			std::cout << "field text: ";
			std::cin.ignore();
			std::cin.getline(field, GlobalConstants::BUFFER_SIZE, '\n');
			if (!table.edit(rowNumber, collNumber, field))
				return -1;

		}
		else if (strcmp(command, "print") == 0) {
			table.print();
		}
		else if (strcmp(command, "save") == 0) {
			char fileName[GlobalConstants::BUFFER_SIZE];
			std::cout << "File name: ";
			std::cin >> fileName;
			if (!saveToFile(fileName, table))
				return false;

		}
		else if (strcmp(command, "quit") == 0 || command[0] == 'q') {
			return 0;
		}
		else {
			std::cout << "Wrong command!\n";
		}



	}
	return 0;

	//readTable("table.html", table);
	//table.print();
	//table.add(6, "Ivan Petrov|12|1234|un1|un2");
	//table.print();
	//table.add(6, "Petur Ivanov|213|1234");
	////table.remove(2);
	////table.print();
	////table.edit(1, 1, "NAMEEEEEEEEEEEEEEEEEEEEEEEEEEEE");
	////table.print();
	//saveToFile("table2.html", table);

 //   return 0;
 }

