#include "FileReader.h"

#include <iostream>
#include <charconv>

#define SECTION_CHAR '#'
#define COMMENT_CHAR '-'

#define s_hash(section) _get_section_hash_val(str_val(section))
#define k_hash(key, section_hash_val) _get_key_hash_val(_hash_func(key), section_hash_val)
#define i_hash(key, section_hash_val) _get_key_hash_val(key, section_hash_val);

#define valid_hash(hash_val) if (hash_val < 0) { return false; }

// convert string to additive int value
size_t FileReader::str_val(const std::string_view str) {
	size_t val = 0;

	for (auto c : str) {
		val += c;
	}

	return val;
}

// string to int
size_t FileReader::int_val(const std::string_view str) {
	size_t value = 0;
	std::from_chars(str.data(), str.data() + str.size(), value);
	return value;
}

FileReader::FileReader(const char* file_path, size_t(*hash_func)(const std::string_view str)) :
	_num_lines(0),
	_query({ 0 }),
	_section_hash_val(0),
	_read(true),
	_hash_func(hash_func)
{
	std::fstream file(file_path, std::ios::in);
	if (!file.is_open()) {
		std::cout << "FileReader Error: no file at  -- " << file_path << '\n';
		_read = false;
		return;
	}

	_query_file(file);
	_data.resize(_query.size());

	std::string key, val;
	size_t section = 0;

	// Default Table -- No Section Comment
	_section_hash_val = _new_section_hash_val(str_val(""));
	_data[_section_hash_val].key_val = _hash_func("");
	_data[_section_hash_val].section = "";
	_data[_section_hash_val].table.resize(_query[section]);

	while (std::getline(file, key, ' ')) {
		std::getline(file, val);

		while (key.size() && *key.begin() == '\n') {
			key.erase(key.begin());
		}
		if (key.size()) {
			if (key[0] == SECTION_CHAR) {
				const int key_val = str_val(val);
				const int hash_val = _new_section_hash_val(key_val);
				if (hash_val == -1) {
					throw std::out_of_range("Table Sections are full");
				}
				++section;
				_data[hash_val].key_val = key_val;
				_data[hash_val].section = val;
				_data[hash_val].table.resize(_query[section]);
				_section_hash_val = hash_val;
			}
			else if (key[0] != COMMENT_CHAR) {
				const int key_val = _hash_func(key);
				const int hash_val = _new_key_hash_val(key_val, _section_hash_val);
				if (hash_val == -1) {
					throw std::out_of_range("Tables are full");
				}
				_data[_section_hash_val].table[hash_val] = { key_val, val };
			}
			++_num_lines;
		}
	}

}

bool FileReader::s_read(std::string* val, const std::string_view key, const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	valid_hash(section_hash_val);

	const int key_hash_val = k_hash(key, section_hash_val);
	valid_hash(key_hash_val);

	*val = _data[section_hash_val].table[key_hash_val].value;
	return true;
}

bool FileReader::s_read(std::string_view* val, const std::string_view key, const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	valid_hash(section_hash_val);

	const int key_hash_val = k_hash(key, section_hash_val);
	valid_hash(key_hash_val);

	*val = _data[section_hash_val].table[key_hash_val].value;
	return true;
}

bool FileReader::s_read(int* val, const std::string_view key, const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	valid_hash(section_hash_val);

	const int key_hash_val = k_hash(key, section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::s_read(unsigned int* val, const std::string_view key, const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	valid_hash(section_hash_val);

	const int key_hash_val = k_hash(key, section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::s_read(float* val, const std::string_view key, const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	valid_hash(section_hash_val);

	const int key_hash_val = k_hash(key, section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::s_read(double* val, const std::string_view key, const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	valid_hash(section_hash_val);

	const int key_hash_val = k_hash(key, section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::s_read(bool* val, const std::string_view key, const std::string_view section) {
	int int_val;

	if (!s_read(&int_val, key, section)) {
		return false;
	}

	*val = int_val;

	return true;
}

bool FileReader::read(std::string* val, const std::string_view key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = k_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	*val = _data[_section_hash_val].table[key_hash_val].value;
	return true;
}

bool FileReader::read(std::string_view* val, const std::string_view key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = k_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	*val = _data[_section_hash_val].table[key_hash_val].value;
	return true;
}

bool FileReader::read(int* val, const std::string_view key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = k_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(unsigned int* val, const std::string_view key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = k_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(float* val, const std::string_view key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = k_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(double* val, const std::string_view key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = k_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(bool* val, const std::string_view key) {
	int int_val;

	if (!read(&int_val, key)) {
		return false;
	}

	*val = int_val;

	return true;
}

bool FileReader::read(std::string* val, const int key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = i_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	*val = _data[_section_hash_val].table[key_hash_val].value;
	return true;
}

bool FileReader::read(std::string_view* val, const int key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = i_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	*val = _data[_section_hash_val].table[key_hash_val].value;
	return true;
}

bool FileReader::read(int* val, const int key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = i_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(unsigned int* val, const int key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = i_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(float* val, const int key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = i_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(double* val, const int key) {
	if (!_read) {
		return false;
	}

	const int key_hash_val = i_hash(key, _section_hash_val);
	valid_hash(key_hash_val);

	const std::string_view read_value = _data[_section_hash_val].table[key_hash_val].value;
	std::from_chars(read_value.data(), read_value.data() + read_value.size(), *val);

	return true;
}

bool FileReader::read(bool* val, const int key) {
	int int_val;

	if (!read(&int_val, key)) {
		return false;
	}

	*val = int_val;

	return true;
}

bool FileReader::set_section(const std::string_view section) {
	if (!_read) {
		return false;
	}

	const int section_hash_val = s_hash(section);
	if (section_hash_val == -1) {
		return false;
	}

	_section_hash_val = section_hash_val;
	return true;
}

int FileReader::_get_section_hash_val(const int section_val) {
	int hash_val = section_val % _data.capacity();
	const int start_val = hash_val;

	while (_data[hash_val].key_val != section_val) {
		hash_val = (hash_val + 1) % _data.capacity();

		if (hash_val == start_val) {
			return -1;
		}
	}

	return hash_val;
}

int FileReader::get_num_lines(const std::string_view section) {
	if (!_read) {
		return -1;
	}

	const int section_hash_val = s_hash(section);
	if (section_hash_val < 0) {
		return -1;
	}

	return _data[section_hash_val].table.size();
}

bool FileReader::is_read() {
	return _read;
}

int FileReader::_new_section_hash_val(const int section_val) {
	int hash_val = section_val % _data.capacity();
	const int start_val = hash_val;

	while (!_data[hash_val].table.empty()) {
		hash_val = (hash_val + 1) % _data.capacity();

		if (hash_val == start_val) {
			return -1;
		}
	}

	return hash_val;
}

int FileReader::_get_key_hash_val(const int key_val, const int section_hash_val) {
	if (_data[section_hash_val].table.empty()) {
		return -1;
	}

	int hash_val = key_val % _data[section_hash_val].table.capacity();
	const int start_val = hash_val;

	while (_data[section_hash_val].table[hash_val].key_val != key_val) {
		hash_val = (hash_val + 1) % _data[section_hash_val].table.capacity();

		if (hash_val == start_val) {
			return -1;
		}
	}

	return hash_val;
}

int FileReader::_new_key_hash_val(const int key_val, const int section_hash_val) {
	if (_data[section_hash_val].table.empty()) {
		return -1;
	}

	int hash_val = key_val % _data[section_hash_val].table.capacity();
	const int start_val = hash_val;

	while (_data[section_hash_val].table[hash_val].value != "") {
		hash_val = (hash_val + 1) % _data[section_hash_val].table.capacity();

		if (hash_val == start_val) {
			return -1;
		}
	}

	return hash_val;
}

// read num of sections and lines in each section
// sections[a] = b ; a = section, b = num of lines
void FileReader::_query_file(std::fstream& file) {
	char header;
	size_t section = 0;
	bool eof = false;
	while (!file.eof()) {
		do {
			eof = file.eof();
			file.get(header);
		} while (header == '\n' && !eof);

		if (!eof) {
			file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (header == SECTION_CHAR) {
				_query.push_back(0);
				++section;
			}
			else if (header != COMMENT_CHAR) {
				++_query[section];
			}
		}
	}

	file.clear();
	file.seekg(0, std::ios::beg);
}

std::vector<FileReader::Key_Value>::const_iterator FileReader::s_begin() const {
	return _data[_section_hash_val].table.begin();
}

std::vector<FileReader::Key_Value>::const_iterator FileReader::s_end() const {
	return _data[_section_hash_val].table.end();
}

std::vector<FileReader::Key_Table>::const_iterator FileReader::begin() const {
	return _data.begin();
}

std::vector<FileReader::Key_Table>::const_iterator FileReader::end() const {
	return _data.end();
}