#include "DatabaseService.h"
#include "platform/CCFileUtils.h"
USING_NS_CC;
using namespace std;
DatabaseService* DatabaseService::_instance = nullptr;
DatabaseService* DatabaseService::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new DatabaseService();
	}
	return _instance;
}
DatabaseService::DatabaseService()
{

}
DatabaseService::~DatabaseService()
{
	if (_instance)
		delete _instance;
}

sqlite3* DatabaseService::open(string path)
{
	sqlite3* pDB = NULL;
	char* errMsg = NULL;
	string sqlstr;
	int result;
	string dbPath = FileUtils::getInstance()->getWritablePath() + path;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (!FileUtils::getInstance()->isFileExist(dbPath.c_str())) {
		Data data =
			FileUtils::getInstance()->getDataFromFile(FileUtils::getInstance()->fullPathForFilename(path.c_str()));
		unsigned char* readData = data.getBytes();
		FILE* file = fopen(dbPath.c_str(), "wb");
		fwrite(readData, data.getSize(), 1, file);
		fclose(file);
	}
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	Data data =
		FileUtils::getInstance()->getDataFromFile(FileUtils::getInstance()->fullPathForFilename(path.c_str()));
	unsigned char* readData = data.getBytes();
	if (!FileUtils::getInstance()->isFileExist(dbPath)) {
		std::ofstream outfile(dbPath.c_str(), std::ofstream::out);
		outfile.write((char*)&readData[0], data.getSize());
		outfile.close();
	}
#endif
	result = sqlite3_open_v2(dbPath.c_str(), &pDB, SQLITE_OPEN_READWRITE, NULL);
	if (result != SQLITE_OK)
	{
		return NULL;
	}
	return pDB;
}
bool DatabaseService::insertData(sqlite3* db, std::string table, std::vector<std::string> field, std::vector<std::string> values)
{
	string querryStr = "";
	querryStr.append("insert into ").append(table).append("(");
	for (vector<string>::iterator i = field.begin(); i != field.end(); ++i)
	{
		querryStr.append(*i);
		if (i + 1 != field.end())
		{
			querryStr.append(", ");
		}
	}
	querryStr.append(") values(");
	for (vector<string>::iterator i = values.begin(); i != values.end(); ++i)
	{
		querryStr.append(*i);
		if (i + 1 != values.end())
		{
			querryStr.append(", ");
		}
	}
	querryStr.append(")");
	char *str = &querryStr[0];
	sqlite3_stmt *statement;
	int result = sqlite3_prepare_v2(db, querryStr.c_str(), -1, &statement, NULL);
	if (result == SQLITE_OK)
	{
		int res = sqlite3_step(statement);
		result = res;
		sqlite3_finalize(statement);
		return true;
	}
	return false;
}
bool DatabaseService::deleteData(sqlite3* db, string table, string fieldName,string value)
{
	string query ="delete from" + table + " where" + fieldName + " =" + value;
	sqlite3_stmt *statement;
	int result;
	if (sqlite3_prepare(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int res = sqlite3_step(statement);
		result = res;
		sqlite3_finalize(statement);
		return true;
	}
	return false;
}
bool DatabaseService::updateData(sqlite3* db, std::string table, std::vector<std::string> field, std::vector<std::string> values)
{
	return false;
}
bool DatabaseService::query(sqlite3*db, std::string query)
{
	sqlite3_stmt *statement;
	int result;
	if (sqlite3_prepare(db, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int res = sqlite3_step(statement);
		result = res;
		sqlite3_finalize(statement);
		return true;
	}
	return false;
}

vector<vector<string>> DatabaseService::readDataTable(sqlite3* db, string tableName)
{
	vector<vector<string> > table;
	sqlite3_stmt *statement;
	int result = sqlite3_prepare_v2(db, ("SELECT * FROM " + tableName).c_str(), -1, &statement, NULL);
	if (result == SQLITE_OK)
	{
		int columnCount = sqlite3_column_count(statement);
		int res = 0;

		while (1)
		{
			res = sqlite3_step(statement);

			if (res == SQLITE_ROW)
			{
				vector<string> column;
				for (int i = 0; i < columnCount; i++)
				{
					string s = (char*)sqlite3_column_text(statement, i);
					column.push_back(s);
				}
				table.push_back(column);
			}

			if (res == SQLITE_DONE || res == SQLITE_ERROR)
			{
				break;
			}
		}
	}
	return table;
}