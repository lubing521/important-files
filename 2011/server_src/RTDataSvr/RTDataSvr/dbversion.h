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
	TCHAR szFileName[ MAX_PATH ];
	
	_stprintf(szFileName, _T("%s%x\\"), lpPath, dbversion);
	utility::mkpath(szFileName);
	_tcscat(szFileName, lpName);
	
	if (utility::FILE_Ptr f = utility::fopen(szFileName, _T("w+b"))) {
		utility::fwrite(&dbversion, sizeof(dbversion), 1, f);
		utility::fwrite(&ulVersion, sizeof(ulVersion), 1, f);
		utility::fwrite(pRecords, sizeof(Tty), nRecords, f);
	}
}

template<typename Tty>
class ZDBVersionTranlator 
{
public:
	bool TranlateDBVersion(utility::FILE_Ptr f,
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
	bool TranlateDBVersionCur(utility::FILE_Ptr f, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		if (utility::fseek(f, 0, SEEK_END)) {
			return false;
		}
		uint32 sz = (uint32)utility::ftell(f);

		if (sz < 8) {
			return false;
		}
		sz -= 8;
		nRecords = sz / sizeof(Tty);
		pRecords = I8MP.malloc<Tty>(nRecords);

		utility::fseek(f, 8, SEEK_SET);
		return utility::fread(pRecords, sizeof(Tty), nRecords, f) == nRecords;
	}
};

namespace db {
namespace v101 {

struct tArea {
	TCHAR AID[MAX_GUID_LEN];
	TCHAR Name[MAX_NAME_LEN];
};

}
}

template<>
class ZDBVersionTranlator<db::tArea> 
{
public:
	typedef db::tArea Tty;
	bool TranlateDBVersion(utility::FILE_Ptr f, 
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
	bool TranlateDBVersionCur(utility::FILE_Ptr f, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		if (utility::fseek(f, 0, SEEK_END)) {
			return false;
		}
		uint32 sz = (uint32)utility::ftell(f);

		if (sz < 8) {
			return false;
		}
		sz -= 8;

		nRecords = sz / sizeof(Tty);
		pRecords = I8MP.malloc<Tty>(nRecords);

		utility::fseek(f, 8, SEEK_SET);
		return utility::fread(pRecords, sizeof(Tty), nRecords, f) == nRecords;
	}
	bool TranlateDBVersion101(utility::FILE_Ptr f, 
						   uint32& nRecords, 
						   Tty *&pRecords)
	{
		if (utility::fseek(f, 0, SEEK_END)) {
			return false;
		}
		uint32 sz = (uint32)utility::ftell(f);

		if (sz < 8) {
			return false;
		}
		sz -= 8;

		nRecords = sz / sizeof(db::v101::tArea);
		db::v101::tArea *p = I8MP.malloc<db::v101::tArea>(nRecords);
		pRecords = I8MP.malloc<Tty>(nRecords);
		memset(pRecords, 0, sizeof(Tty) * nRecords);

		utility::fseek(f, 8, SEEK_SET);
		bool b = utility::fread(p, sizeof(db::v101::tArea), nRecords, f) == nRecords;
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
