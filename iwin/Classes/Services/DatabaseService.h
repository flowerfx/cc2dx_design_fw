#ifndef _DATABASE_SERVICE_H__
#define _DATABASE_SERVICE_H__
#include<string>
#include<vector>
#include "cocos2d.h"
#include "external/sqlite3/include/sqlite3.h"

class DatabaseService
{
private:
	static DatabaseService* _instance;
public:
	static DatabaseService* getInstance();
	DatabaseService();
	~DatabaseService();

	sqlite3* open(std::string path);
	std::vector<std::vector<std::string>> readDataTable(sqlite3* db, std::string tableName);
	bool insertData(sqlite3* db, std::string table, std::vector<std::string> field, std::vector<std::string> values);
	bool deleteData(sqlite3* db, std::string table, std::string fieldName, std::string values);
	bool updateData(sqlite3* db, std::string table, std::vector<std::string> field, std::vector<std::string> values);
	bool query(sqlite3*db, std::string query);




};

#endif //_DATABASE_SERVICE_H__
