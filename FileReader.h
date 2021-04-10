#ifndef FILE_READER_H
#define FILE_READER_H

#include <vector>
#include <string>
#include <string_view>
#include <fstream>

/* Reads formatted data from a file
** example file:
** "file.txt"
** # Person
** str_name Greg
** i_number 100

** Uses 2D vector of Key_Value pairs
** People = Section
** i_number = Value_Key
** Data[Section_Hash_Val] -> Table[Value_Key_Hash_Val] -> Value

** Only handles insertion not deletion - Only Reading data from files not changing it
** Hash arrays are static
*/

class FileReader {
public:
	struct Key_Value {
		int key_val = 0;
		std::string value = "";
	};

	struct Key_Table {
		int key_val = 0;
		std::string section;
		std::vector<Key_Value> table;
	};

	FileReader(const char* file_path, size_t(*hash_func)(const std::string_view str) = &str_val);

	// s_read : reads from a defined section - default is no section
	// returns false if no key exists
	bool s_read(std::string* val, const std::string_view key, const std::string_view section = "");
	bool s_read(std::string_view* val, const std::string_view key, const std::string_view section = "");
	bool s_read(int* val, const std::string_view key, const std::string_view section = "");
	bool s_read(unsigned int* val, const std::string_view key, const std::string_view section = "");
	bool s_read(float* val, const std::string_view key, const std::string_view section = "");
	bool s_read(double* val, const std::string_view key, const std::string_view section = "");
	bool s_read(bool* val, const std::string_view key, const std::string_view section = "");

	// read : reads from current section using set_section - default is no section
	// returns false if no key exists
	bool read(std::string* val, const std::string_view key);
	bool read(std::string_view* val, const std::string_view key);
	bool read(int* val, const std::string_view key);
	bool read(unsigned int* val, const std::string_view key);
	bool read(float* val, const std::string_view key);
	bool read(double* val, const std::string_view key);
	bool read(bool* val, const std::string_view key);

	bool read(std::string* val, const int key);
	bool read(std::string_view* val, const int key);
	bool read(int* val, const int key);
	bool read(unsigned int* val, const int key);
	bool read(float* val, const int key);
	bool read(double* val, const int key);
	bool read(bool* val, const int key);

	// set section to read from using read_string, read_int ...
	bool set_section(const std::string_view section);

	int get_num_lines(const std::string_view section);

	bool is_read();

	// section iterators
	std::vector<Key_Value>::const_iterator s_begin() const;
	std::vector<Key_Value>::const_iterator s_end() const;

	std::vector<Key_Table>::const_iterator begin() const;
	std::vector<Key_Table>::const_iterator end() const;

	static size_t str_val(const std::string_view str);
	static size_t int_val(const std::string_view str);
private:
	std::vector<Key_Table> _data;
	std::vector<size_t> _query;
	size_t _num_lines;
	int _section_hash_val;
	bool _read;

	int _get_section_hash_val(const int section_val);
	int _new_section_hash_val(const int section_val);
	int _get_key_hash_val(const int key_val, const int section_hash_val = 0);
	int _new_key_hash_val(const int key_val, const int section_hash_val = 0);

	void _query_file(std::fstream& file);

	size_t(*_hash_func)(const std::string_view str);
};

#endif