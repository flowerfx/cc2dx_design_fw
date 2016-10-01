#ifndef __ARK_ARCHIVE_H__
#define __ARK_ARCHIVE_H__

#include "ArkCommon.h"
#include "../../RKString_Code/RKString.h"
namespace RKUtils
{
	class Ark
	{
	public:
		Ark();
		~Ark();
		bool                    Initialise(const char* pAbsFilename);

		bool                    GetFileHandle(ArkFileMetadata& Meta, ArkFileHandle& outHandle);

		ArkFileMetadata*        GetArkIndex();  // User must call UnloadArkIndex to free memory!
		void                    UnloadIndex();

		inline const char*      GetArkFilename()  { return m_sArkFilename.GetString(); }
		inline uint32           GetNumFiles()     { return m_Header.iNumFiles; }
		inline const RKString   GetMD5Digest()    { return m_sDigest; }
		uint32                  GetArkNumber();

		bool                    ExtractFile(const char* pFilename, const char* pAbsOutDir);

	private:
		Ark(const Ark& _kr);
		Ark& operator=(const Ark& _kr);

		bool                    LoadIndex();
		bool                    GenerateMD5Digest();
		bool                    DecompressFile(char* pCompr, uint32 iComprLen, char* pUncompr, uint32 iUncomprLen);

		// Member variables
	private:
		RKString  m_sArkFilename;
		ArkHeader m_Header;
		char*     m_pIndex;
		RKString  m_sDigest;
	};
}

#endif //__ARK_ARCHIVE_H__