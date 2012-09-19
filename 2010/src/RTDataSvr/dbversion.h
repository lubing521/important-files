#ifndef DBVERSION_H
#define DBVERSION_H

#define DB_VERSION_CUR 0x101
#define DB_VERSION_101 0x101

namespace i8desk {

template<typename Tty>
inline void BackupDatabaseFile(LPCTSTR lpPath,
							   uint32 dbversion,
							   LPCTSTR lpName,
							   ulong ulVersion, 
							   Tty *pRecords, 
							   size_t nRecords)
{
	char szFileName[ MAX_PATH ];
	
	sprintf(szFileName, "%s%x\\", lpPath, dbversion);
	mkpath(szFileName);
	strcat(szFileName, lpName);
	
	if (FILE_Ptr f = fopen(szFileName, "w+b")) {
		fwrite(&dbversion, sizeof(dbversion), 1, f);
		fwrite(&ulVersion, sizeof(ulVersion), 1, f);
		fwrite(pRecords, sizeof(Tty), nRecords, f);
	}
}

template<typename Tty>
class ZDBVersionTranlator 
{
public:
	bool TranlateDBVersion(FILE_Ptr f,
						   uint32 dbversion, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		switch (dbversion) {
		case DB_VERSION_CUR:
			return TranlateDBVersionCur(f, nRecords, pRecords);
		default:
			break;
		}
		return false;
	}
private:
	bool TranlateDBVersionCur(FILE_Ptr f, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		if (fseek(f, 0, SEEK_END)) {
			return false;
		}
		uint32 sz = (uint32)ftell(f);

		if (sz < 8) {
			return false;
		}
		sz -= 8;
		nRecords = sz / sizeof(Tty);
		pRecords = I8MP.malloc<Tty>(nRecords);

		fseek(f, 8, SEEK_SET);
		return fread(pRecords, sizeof(Tty), nRecords, f) == nRecords;
	}
};

namespace db {
namespace v101 {

struct tArea {
	char AID[MAX_GUID_LEN];
	char Name[MAX_NAME_LEN];
};

}
}

template<>
class ZDBVersionTranlator<db::tArea> 
{
public:
	typedef db::tArea Tty;
	bool TranlateDBVersion(FILE_Ptr f, 
						   uint32 dbversion, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		switch (dbversion) {
		case DB_VERSION_101:
			return TranlateDBVersion101(f, nRecords, pRecords);
		default:
			break;
		}
		return false;
	}
private:
	bool TranlateDBVersionCur(FILE_Ptr f, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		if (fseek(f, 0, SEEK_END)) {
			return false;
		}
		uint32 sz = (uint32)ftell(f);

		if (sz < 8) {
			return false;
		}
		sz -= 8;

		nRecords = sz / sizeof(Tty);
		pRecords = I8MP.malloc<Tty>(nRecords);

		fseek(f, 8, SEEK_SET);
		return fread(pRecords, sizeof(Tty), nRecords, f) == nRecords;
	}
	bool TranlateDBVersion101(FILE_Ptr f, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		if (fseek(f, 0, SEEK_END)) {
			return false;
		}
		uint32 sz = (uint32)ftell(f);

		if (sz < 8) {
			return false;
		}
		sz -= 8;

		nRecords = sz / sizeof(db::v101::tArea);
		db::v101::tArea *p = I8MP.malloc<db::v101::tArea>(nRecords);
		pRecords = I8MP.malloc<Tty>(nRecords);
		memset(pRecords, 0, sizeof(Tty) * nRecords);

		fseek(f, 8, SEEK_SET);
		bool b = fread(p, sizeof(db::v101::tArea), nRecords, f) == nRecords;
		if (b) {
			db::v101::tArea *pOld = p;
			Tty *pNew = pRecords;
			for (uint32 i = 0; i < nRecords; i++) {
				memcpy(pNew->AID, pOld->AID, sizeof(pRecords->AID));
				memcpy(pNew->Name, pOld->Name, sizeof(pRecords->AID));
				pOld++, pNew++;
			}
		}
		I8MP.free(p);
		I8MP.reclaim();

		return b;
	}
};



} //namespace i8desk

#endif //DBVERSION
