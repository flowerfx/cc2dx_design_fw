//**************************************************************************************************************************************
//*
//* RKAssert.cpp
//*
//* [Christophe Le Bouil]
//**************************************************************************************************************************************
#include <string.h>

#include "RKAssert.h"
#include "RKStdSAFE.h"
#ifdef _WIN32
#include <Windows.h>
#endif


extern void RKErrorDisplay_Platform(int errType, const char *cond, const char *file, const char *fn, int line, const char *bufferMessage);


bool gRKAssertWantBreakpoint = true;

	//+++	if (MLCore::RunFromDebugger())

#include "../RKLog_Code/RKLog.h"

#ifndef NULL
#define NULL 0
#endif


namespace RKUtils
{
	//************************************************************************************************************************************
	//* from MLA.h ...
	//************************************************************************************************************************************
	int MLA_decode_argN(char *dest, int len, RKArgumentList *arg, int n, const char *format, int formatType);


	enum
	{
		PERC_NONE,
		PERC_s,
		PERC_d,
		PERC_f,
		PERC_XMLString,
		PERC_X,
		PERC_x = PERC_X,
	};


	//----------------------------------------------------------------------------------------------------------------------
	int MLA_argToText(char *dest, int len, RKArgumentList *args)
		//----------------------------------------------------------------------------------------------------------------------
	{
		*dest = 0;
		if (args == NULL)
			return 0;

		int n;
		for (n = 0; n < args->nbArguments; n++)
		{
			MLA_decode_argN(dest, len, args, n, NULL, (int)PERC_NONE);
		}
		return strlen(dest);	//mm;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool TestFormat(char *format, char**ppEnd, int *pType, int type, const char *toTest)
		//----------------------------------------------------------------------------------------------------------------------
	{
		int len = strlen(toTest);
		if (strncmp(format, toTest, len) == 0)
		{
			*ppEnd = (format + len);
			if (pType)
				*pType = type;
			return true;
		}
		return false;
	}


	static char *grabFormatType(char *format, int *pType);
	//----------------------------------------------------------------------------------------------------------------------
	char *grabFormatType(char *format, int *pType)
		//----------------------------------------------------------------------------------------------------------------------
	{
		char *pEnd;
		*pType = PERC_NONE;
		if (TestFormat(format, &pEnd, pType, PERC_s, "%s"))	return pEnd;
		if (TestFormat(format, &pEnd, pType, PERC_d, "%d"))	return pEnd;
		if (TestFormat(format, &pEnd, pType, PERC_XMLString, "%XMLString"))	return pEnd;
		if (TestFormat(format, &pEnd, pType, PERC_X, "%X"))	return pEnd;
		if (TestFormat(format, &pEnd, pType, PERC_x, "%x"))	return pEnd;
		if (TestFormat(format, &pEnd, pType, PERC_f, "%f"))	return pEnd;

		if (format[1] >= '0' && format[1] <= '9')
		{
			int i = 2;
			while (format[i] != 0 && format[i] != 'f' && format[i] != 'x' && format[i] != 'X' && format[i] != 'd')
				i++;
			pEnd = &format[i];
			switch (*pEnd)
			{
			case 'f': *pType = PERC_f; break;
			case 'x': *pType = PERC_x; break;
			case 'X': *pType = PERC_X; break;
			case 'd': *pType = PERC_d; break;
			}
			if (format[i] != 0)
				pEnd++;
			return pEnd;	//mm
		}


		RKLOGt_WARN("ASSERT", "unknown format : %s", format);

		return NULL;
	}


	//----------------------------------------------------------------------------------------------------------------------
	int MLA_snprintf(char *destBuffer, int destLen, RKArgumentList *args)
		//----------------------------------------------------------------------------------------------------------------------
	{
		*destBuffer = 0;

		if (args == NULL)
			return 0;

		//supposing that the 1st arg is the format string ...
		char format[512];
		format[0] = 0;

		MLA_decode_argN(format, sizeof(format), args, 0, "%s", (int)PERC_s);

		//	char *textStart = format;

		//	int lenToCopy;
		//	char *pPercent; = strstr(textStart, "%");
		/*
			if (pPercent)
			lenToCopy = pPercent-textStart;
			else
			lenToCopy = strlen(textStart);

			if (lenToCopy)
			strncpy_SAFE(dest, len, format, lenToCopy);
			*/

		char *src = format;
		char *dst = destBuffer;
		int remaining = destLen;

		int formatType;
		int n;
		for (n = 1; n < args->nbArguments; n++)
		{
			//-- copy normal text up to the next %
			while (src != NULL && *src != 0 && *src != '%' && remaining > 0)
			{
				*dst++ = *src++;
				remaining--;
			}
			if (remaining > 0)
			{
				*dst = 0;
			}

			char thisArgFormatBuffer[128] = "";
			char *thisArgFormat = NULL;
			if (src != NULL && *src == '%')
			{
				thisArgFormat = src;
				char *endOfPercent = grabFormatType(src, &formatType);

				src = endOfPercent;
				int lenArg = 0;	//thisArgFormat
				if (endOfPercent)
				{
					lenArg = endOfPercent - thisArgFormat;
					if (lenArg >= (int)sizeof(thisArgFormatBuffer))
						lenArg = sizeof(thisArgFormatBuffer) - 1;
					strncpy_AUTO(thisArgFormatBuffer, thisArgFormat, lenArg);
					thisArgFormatBuffer[lenArg] = 0;
				}
			}

			char buffer_one_arg[512];
			MLA_decode_argN(buffer_one_arg, sizeof(buffer_one_arg), args, n, thisArgFormatBuffer, formatType);
			strcat_SAFE(dst, remaining, buffer_one_arg);
			int argLen = strlen(dst);
			dst += argLen;
			remaining -= argLen;
		}


		//--copy the end ----

		while (src != NULL && *src != 0 && *src != '%' && remaining > 0)
		{
			*dst++ = *src++;
			remaining--;
		}
		if (remaining > 0)
			*dst = 0;
		else
			destBuffer[destLen - 1] = 0;

		return destLen - remaining;
	}


	/*
	//----------------------------------------------------------------------------------------------------------------------
	bool CheckFormat(char *formatString, char *formatToCheck)
	//----------------------------------------------------------------------------------------------------------------------
	{
	int len = strlen(formatToCheck);
	if (strncmp(formatString, formatToCheck, len)==0)
	return true;
	return false;
	}
	*/
	//----------------------------------------------------------------------------------------------------------------------
	int MLA_decode_argN(char *dest, int len, RKArgumentList *arg, int n, const char *format, int formatType)
		//----------------------------------------------------------------------------------------------------------------------
	{
		*dest = 0;
		if (arg == NULL)
			return 0;

		//	char buffer[128];

		//--- for the compiler to get the types : some unused variables ...
		char *_charP = NULL;
		const char *_const_charP = NULL;
		rku8 _u8 = 0;
		rks8 _s8 = 0;
		rku8 *_u8P = NULL;
		rks8 *_s8P = NULL;
		rku16 _u16 = 0;
		rks16 _s16 = 0;
		rku16 *_u16P = NULL;
		rks16* _s16P = NULL;
		int _int = 0;
		unsigned int _uint = 0;
		long _long = 0;
		unsigned long _ulong = 0;

		RKUnused(_int); RKUnused(_uint);
		RKUnused(_charP); RKUnused(_const_charP);
		RKUnused(_u8P); RKUnused(_s8P);
		RKUnused(_u8); RKUnused(_s8);
		RKUnused(_u16); RKUnused(_s16);
		RKUnused(_long);  RKUnused(_ulong);

		float _float = 0;	RKUnused(_float);
		double _double = 0; RKUnused(_double);

		void *_voidP = NULL;	RKUnused(_voidP);

#if ML_USE_STRING==1
		MLString *_MLStringP=NULL;	RKUnused(_MLStringP);
		MLString _MLString;		RKUnused(_MLString);

#endif

		//-----
		int wasFound = true;

		size_t value = arg->argument[n].value;
		int type_id = arg->argument[n].typeId;
		switch (type_id)
		{
			//----------------------------------------- char pointer ----------------------
		case RKSA_typeID(_const_charP): //RKSA_typeID = MLA_typeID
			_charP = (char*)value;
			if (format)
			{
				if (formatType == PERC_X)	//CheckFormat(format, "%X")==true || CheckFormat(format, "%x")==true)
				{
					sprintf_SAFE(dest, len, format, value);
				}
				else
				{
					if (_charP == NULL)
					{
						RKWarning(0, "BAD : text argument (%s) is NULL", format);
						strcpy_SAFE(dest, len, "<%s NULL!>");
						return false;
					}
					RKWarning(formatType == PERC_s, "BAD : text argument without s !format=%s", format);
					//can't use strcpy_SAFE if error : recuse !
					int lenSrc = strlen(_charP);
					if (lenSrc < len - 1)
						strcpy_SAFE(dest, len, _charP);
					else
						strncpy_REALLY(dest, _charP, len - 1);
				}
			}
			else
			{
				//todo : check if _charP is valid /readable!
				if (_charP == NULL)
					strcpy_SAFE(dest, len, "NULL");
				else
					strcpy_SAFE(dest, len, _charP);
			}
			break;

			//------------------------------------ int, etc -------------
		case RKSA_typeID(_int):
			if (format)
			{
				if (formatType == PERC_X)
					sprintf_SAFE(dest, len, "%X ", (int)value);
				else
				{
					RKWarning(formatType == PERC_d, "BAD : int argument without d !");
					sprintf_SAFE(dest, len, "%d", (int)value);
				}
			}
			else
			{
				sprintf_SAFE(dest, len, " (int)=%d ", (int)value);
			}
			break;

		case RKSA_typeID(_uint):
			sprintf_SAFE(dest, len, " (uint)=%d ", (int)value);
			break;

		case RKSA_typeID(_long):
		case RKSA_typeID(_ulong):
			if (format)
			{
				if (formatType == PERC_d)
					sprintf_SAFE(dest, len, "%d ", (int)value);
				else
					sprintf_SAFE(dest, len, "%X ", (int)value);
			}
			else
				sprintf_SAFE(dest, len, "%d ", (int)value);
			break;

#if ML_USE_STRING==1
			//------------------------------------- MLString ------------------
		case RKSA_typeID(_MLString) :
		{
			_charP = (char*)value;
			strcpy_SAFE(dest, len, "WARNING:MLA_decode_argN() : use MLString* !");
			strcat_SAFE(dest, len, _charP);
		}
		break;

		//------------------------------------- MLString pointer -------------------------
		case RKSA_typeID(_MLStringP) :
		{
			MLString *string = (MLString*)value;
			if (string->strIsUnicode()==false)
			{
				if (format)
				{
					if (formatType==PERC_XMLString)	//CheckFormat(format, "%XMLString")==false)
						strcpy_SAFE(dest, len, string->strBuffer8());		//ok, %XMLString
					else
					{
						if (formatType == PERC_X) //CheckFormat(format, "%X")==true)
							sprintf_SAFE(dest, len, format, string) ;		//well, want to display adress of a string
						else
						{
							RKWarning(0, ER("BAD : MLString argument without XMLString / X !"));
							strcpy_SAFE(dest, len, string->strBuffer8());
						}
					}
				}
				else	//no format mmm
				{
					strcpy_SAFE(dest, len, string->strBuffer8());
				}
			}
			else	//unicode !
			{
				//strcpy_SAFE(dest, len, "TODO:convert UNICODE string ?");
				string->strCopyToAnsi(dest, len);
			}
		}
		break;
#endif //ML_USE_STRING==1

		//----------------------------------------- float / double -------------------------
		case RKSA_typeID(_float):
		{
			float *pFloat = (float*)(&value);
			_float = *pFloat;	//(float)value;
			sprintf_SAFE(dest, len, format, _float);
		}
		break;

		case RKSA_typeID(_double):
			_double = (double)value;
			sprintf_SAFE(dest, len, format, _double);
			break;

			//------------------------------------------- others ...
		case RKSA_typeID(_u8):
			sprintf_SAFE(dest, len, " (u8)=%d ", value);	//mmm		was *( (u8*)value ) );
			break;

		case RKSA_typeID(_s8):
			sprintf_SAFE(dest, len, " (s8)=%d ", value);	//mmm was  *( (u8*)value ) );
			break;

		case RKSA_typeID(_u16):
			sprintf_SAFE(dest, len, " (u16)=%d ", value);	//mmm		was *( (u8*)value ) );
			break;

		case RKSA_typeID(_s16):
			sprintf_SAFE(dest, len, " (s16)=%d ", value);	//mmm was  *( (u8*)value ) );
			break;

		case RKSA_typeID(_u8P):
			sprintf_SAFE(dest, len, " (u8*)=%X ", value);		//mmm sould it be  *( (u8*)value ) );  ??
			break;

			/*mmm	error C2196: case value '34' already used
				case RKSA_typeID(_s8P) :
				sprintf_SAFE(dest, len, " (s8*)=%X ", value );
				break;
				*/
			/*mmm	error C2196: case value '34' already used
					case RKSA_typeID(_voidP):
					if (formatType==PERC_X)	//CheckFormat(format, "%X")==true || CheckFormat(format, "%x")==true)
					sprintf_SAFE(dest, len, format, value ) ;
					else
					sprintf_SAFE(dest, len, "%X", value ) ;
					break;
					*/
		default:
			sprintf_SAFE(dest, len, " MLA_decode_argN():TODO:type_id=%d ", type_id);
			wasFound = 0;
			break;
		}

		return wasFound;
	}

	//************************************************************************************************************************************
	//************************************************************************************************************************************















	//----------------------------------------------------------------------------------------------------------------------
	void RKErrorDisplay(int errType, const char *cond, RKArgumentList *args, const char *file, const char *fn, int line)
		//----------------------------------------------------------------------------------------------------------------------
	{
		char bufferMessage[1024];
		MLA_snprintf(bufferMessage, sizeof(bufferMessage), args);

#ifdef WINAPI_FAMILY
#ifdef _DEBUG
		OutputDebugStringA(bufferMessage);
#endif
#else
		RKErrorDisplay_Platform(errType, cond, file, fn, line, bufferMessage);
#endif
	}

	//****************************************************************************************************************************************************************************
	//*
	//-------------------------------------------------------------------------------------
	RKASError& RKASError::errType(int t)
		//-------------------------------------------------------------------------------------
	{
		m_type = t;

		//-- set oFlag from type ---
		m_oFlags = gRKErrConf.ecGetTypeOutFlag(t);

		return *this;
	}  //ex 		RKERRT_ASSERT




	//-------------------------------------------------------------------------------------
	RKASError& RKASError::add_info_context(const char *file, int line)
		//-------------------------------------------------------------------------------------
	{
		m_filename = file;
		m_line = line;
		return *this;
	}

	//helper
	//void errSafeCat(char *dest, int maxLen, char *src);
	//-------------------------------------------------------------------------------------
	static void errSafeCat(char *dest, int maxLen, const char *src)
		//-------------------------------------------------------------------------------------
	{
		int iNow = (int)strlen(dest);
		while (iNow < maxLen - 1 && *src)
		{
			dest[iNow] = *src++;
			iNow++;
			dest[iNow] = 0;
		}

		if (iNow >= maxLen - 1)
			RKLOGt_WARN("ASSERT", "LIMIT REACHED");
	}



	//-------------------------------------------------------------------------------------
	RKASError::RKASError() //: SMART_ERROR_A (*this), SMART_ERROR_B (*this)	//, SMART_ERROR_RMV_A (*this), SMART_ERROR_RMV_B (*this)
		//-------------------------------------------------------------------------------------
	{
		m_filename = NULL;
		m_functionName = NULL;
		m_line = 0;
		m_expression = NULL;
		m_msg = "";
		m_type = RKERRT_NONE;	//RKERRT_REMOVED;	//RKERRT_NONE;
		m_oFlags = rkoALL;
		///mmm??	MLError_Buffer[0] = 0;
	}



	//****************************************************************************************************************************************************************************
	//*





	const char *gRKTblErrorTypeName[RKERRT_LAST_ + 1] =		//make sure to match the enum ... see RKERRT_LAST_
	{
		"None",
		"WARNING",
		//"VERIFY",
		"ASSERT",
		"FATAL",

		"UNKNOWN"	//RKERRT_LAST_
	};


	//****************************************************************************************************************************************************************************
	//*
	//* RKErrTypeConf Class
	//*
	//*  to store the default behavour depending of error type
	//*
	//*
	//****************************************************************************************************************************************************************************

	//---------------------------------------------------------------------------------------
	RKErrTypeConf::RKErrTypeConf()
		//---------------------------------------------------------------------------------------
	{
#define MLErrTypeConf_CTORHERE
		// default config for all error types
		m_allowIgnoreBut = true;
		m_allowQuitBut = true;
		m_defaultOutputFlags = rkoALL;
		//m_maxLoggedOccurence		= 0;			//never stop logging 
		//m_ignoreThisType				= 0;
		m_ErrorCount = 0;
	}


	//---------------------------------------------------------------------------------------
	void RKErrTypeConf::etcInitDefaultSettings(void)
		//---------------------------------------------------------------------------------------
	{
		//called by ctor and by clearList
	}


	//---------------------------------------------------------------------------------------
	void RKErrTypeConf::etcSetoFlags(int wantLOG, int wantIDE, int wantDISP)
		//---------------------------------------------------------------------------------------
	{
		m_defaultOutputFlags = 0;

		if (wantLOG)		m_defaultOutputFlags |= rkoLOG;
		if (wantIDE)		m_defaultOutputFlags |= rkoIDE;
		if (wantDISP)		m_defaultOutputFlags |= rkoDISP;
	}


	//*****************************************************************************************************************************
	//*
	//*   RKErrIgnoreInfo class    - Individual error data
	//*
	//*  currently based on __SOURCE__ and __FILE__
	//*
	//* TODO : ? check parameters to diferenciate errors : eg a File not found ("yop.bin")  / File not found ("toto.txt") --> different errors ?
	//* TODO : add ; IgnoreLogAfterNOccurence[]
	//* TODO : add a Persistance option (save / load gErrorArray[] )
	//*****************************************************************************************************************************
	//see RKErrIgnoreInfo

	//--------------------------------------------------------------------------------------------------------------------------------
	RKErrIgnoreInfo::RKErrIgnoreInfo()
		//--------------------------------------------------------------------------------------------------------------------------------
	{
#define MLErrIgnoreInfo_CTORHERE

		iiClear();
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	void RKErrIgnoreInfo::iiClear(void)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		m_filename = NULL;
		m_line = -1;
		m_oFlag = rkoLOG;
		m_nbOccurence = 0;
		m_type = RKERRT_NONE;
	}




	//****************************************************************************************************************************************************************************
	//*
	//* RKErrConf Class
	//*
	//*  
	//****************************************************************************************************************************************************************************
	// default error flags, depending on type (can be overrideen by gRKErrConf.ecSetTypeOutFlag()
#define MLQA_ERR_DISP_ASSERT							1		//display dialog box for error type ASSERT
#define MLQA_ERR_DISP_WARNING							1

#define MLQA_ERR_LOG_ASSERT								1		//log in file for error type ASSERT
#define MLQA_ERR_LOG_WARNING							1

#define MLQA_ERR_IDE_ASSERT								1		//display in VisualC IDE for error type ASSERT
#define MLQA_ERR_IDE_WARNING							1


	RKErrConf gRKErrConf;		//<---- The only instance	


	//---------------------------------------------------------------------------------------
	RKErrConf::RKErrConf()
		//---------------------------------------------------------------------------------------
	{
#define MLErrConf_CTORHERE

		m_totalCount = 0;
		m_maxLoggedOccurence = 50;		//avoid filling the log file with the same error many times
		m_quitRequestedForError = false;

		//specific settings depending on types

		//#undef MLQA_ERR_DISP_WARNING
		//#define MLQA_ERR_DISP_WARNING 0	//test

		//first init : using QA flags
		m_typeConf[RKERRT_WARNING].etcSetoFlags(MLQA_ERR_LOG_WARNING, MLQA_ERR_IDE_WARNING, MLQA_ERR_DISP_WARNING);
		m_typeConf[RKERRT_ASSERT].etcSetoFlags(MLQA_ERR_LOG_ASSERT, MLQA_ERR_IDE_ASSERT, MLQA_ERR_DISP_ASSERT);
		//	m_typeConf[RKERRT_VERIFY].etcSetoFlags(MLQA_ERR_LOG_VERIFY,  MLQA_ERR_IDE_VERIFY,  MLQA_ERR_DISP_VERIFY);

		//-- save it to show in err dialog info ---
		int n;
		for (n = 0; n < RKERRT_LAST_; n++)
			m_typeConf[n].m_QASettingsOutputFlags = m_typeConf[n].m_defaultOutputFlags;

		ecInitDefaultSettings();

	}



	//---------------------------------------------------------------------------------------
	void RKErrConf::ecInitDefaultSettings(void)
		//---------------------------------------------------------------------------------------
	{
		// init or re-init
		int n;
		for (n = 0; n < RKERRT_LAST_; n++)
		{
			m_typeConf[n].etcInitDefaultSettings();
		}



		m_typeConf[RKERRT_FATAL].m_allowIgnoreBut = true;			//no ignore for FATALs !

		m_typeConf[RKERRT_WARNING].m_allowQuitBut = false;			//warning are supposed to handle the error

		//--- restore output settings from QA flags
		//	int n;
		for (n = 0; n < RKERRT_LAST_; n++)
			m_typeConf[n].m_defaultOutputFlags = m_typeConf[n].m_QASettingsOutputFlags;

	}


	//---------------------------------------------------------------------------------------
	void RKErrConf::ecSetTypeOutFlag(int type, int flags)
		//---------------------------------------------------------------------------------------
	{
		//to manually override default settings
		m_typeConf[type].m_defaultOutputFlags = flags;
	}

	//---------------------------------------------------------------------------------------
	int RKErrConf::ecGetTypeOutFlag(int type)
		//---------------------------------------------------------------------------------------
	{
		//to init new error depending on type..
		return m_typeConf[type].m_defaultOutputFlags;
	}


	//---------------------------------------------------------------------------------------
	bool RKErrConf::ecAllowIgnoreBut(int type)
		//---------------------------------------------------------------------------------------
	{
		if (type < RKERRT_NONE || type >= RKERRT_LAST_)
			return false;

		return m_typeConf[type].m_allowIgnoreBut;
	}

	//---------------------------------------------------------------------------------------
	bool RKErrConf::ecAllowQuitBut(int type)
		//---------------------------------------------------------------------------------------
	{
		if (type < RKERRT_NONE || type >= RKERRT_LAST_)
			return true;

		return m_typeConf[type].m_allowQuitBut;
	}

	//---------------------------------------------------------------------------------------
	RKErrTypeConf* RKErrConf::ecGetTypeConfiguration(int type)
		//---------------------------------------------------------------------------------------
	{
		if (type < RKERRT_NONE || type >= RKERRT_LAST_)
			type = RKERRT_NONE;

		return &m_typeConf[type];
	}




	//--------------------------------------------------------------------------------------------------------------------------------
	void RKErrConf::ecClearIgnoreList(void)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		int ne = MLERR_IGNORE_MAX;
		int n;

		for (n = 0; n < ne; n++)
		{
			m_ignoreList[n].iiClear();
		}

		//---- also reset default Type flags
		ecInitDefaultSettings();

	}


	//--------------------------------------------------------------------------------------------------------------------------------
	RKErrIgnoreInfo *RKErrConf::ecGetErrorInfo(int n)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		if (n >= 0 && n < MLERR_IGNORE_MAX)
			return &m_ignoreList[n];
		return NULL;
	}


	//--------------------------------------------------------------------------------------------------------------------------------
	void RKErrConf::ecIgnoreError(RKASError *pErr, int oNewFlags)
		//---------------------------------------------------------------------------------------------------------------------------------
	{
		// internal : to keep a list of user-selected output levels

		//--- check if aleady in list ---
		int ne = MLERR_IGNORE_MAX;

		RKErrIgnoreInfo *freeSlot = NULL;

		int n;
		for (n = 0; n < ne; n++)
		{
			RKErrIgnoreInfo *lst = &m_ignoreList[n];

			if (freeSlot == NULL && lst->m_filename == NULL)
				freeSlot = lst;

			if (pErr->m_line == lst->m_line && lst->m_filename && strcmp(lst->m_filename, pErr->m_filename) == 0)
			{
				//found !
				//change flags...
				lst->m_oFlag = oNewFlags;
				return;
			}
		}

		//-- not found : create a new one
		if (freeSlot)
		{
			freeSlot->m_filename = pErr->m_filename;
			freeSlot->m_line = pErr->m_line;
			freeSlot->m_oFlag = oNewFlags;					//not pErr->m_oFlags;
			freeSlot->m_type = pErr->m_type;
			freeSlot->m_nbOccurence = 1;
		}
		else
		{
			//ho, list is full : can't remember settings for this error
			RKLOGt_WARN("ASSERT", "list is full : can't remember settings for this error : %s, %d", pErr->m_filename, pErr->m_line);
		}
	}




	//--------------------------------------------------------------------------------------------------------------------------------
	void RKErrConf::ecIgnoreError(const char *filename, int line, int oNewFlags)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		//to "manually" ignore some errors in your project

		//--- check if aleady in list ---
		int ne = MLERR_IGNORE_MAX;

		RKErrIgnoreInfo *freeSlot = NULL;

		int n;
		for (n = 0; n < ne; n++)
		{
			RKErrIgnoreInfo *lst = &m_ignoreList[n];

			if (freeSlot == NULL && lst->m_filename == NULL)
				freeSlot = lst;

			if (line == lst->m_line && lst->m_filename && strcmp(lst->m_filename, filename) == 0)
			{
				//found !
				//change flags...
				lst->m_oFlag = oNewFlags;
				return;
			}
		}

		//-- not found : create a new one
		if (freeSlot)
		{
			freeSlot->m_filename = filename;
			freeSlot->m_line = line;
			freeSlot->m_oFlag = oNewFlags;
			freeSlot->m_nbOccurence = 1;
		}
		else
		{
			//ho, list is full : can't remember settings for this error
			RKLOGt_WARN("ASSERT", "list is full : can't remember settings for this error : %s, %d", filename, line);
		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	void RKErrConf::ecIgnoreSource(const char *filename, int oNewFlags)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		RKLOGt_WARN("ASSERT", "ecIgnoreSource(%s, %X)  !!  next errors may not be logged !", filename, oNewFlags);

		ecIgnoreError(filename, 0, oNewFlags);		//line number 0  = any line
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	int RKErrConf::ecIgnoreType(int errorType, int oNewFlags)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		//m_ignoreThisType OR m_defaultOutputFlags ??
		//errorType : see RKERRT_LAST_
		//	MLLog("!! WARNING : ecIgnoreType(%d, %X)  !!  next errors may not be logged !\n", errorType, oNewFlags);

		int prev = m_typeConf[errorType].m_defaultOutputFlags;
		m_typeConf[errorType].m_defaultOutputFlags = oNewFlags;
		return prev;
	}

	//--------------------------------------------------------------------------------------------------------------------------------
	void RKErrConf::ecIgnoreAll(int oNewFlags)
		//--------------------------------------------------------------------------------------------------------------------------------
	{
		for (int n = 0; n < RKERRT_LAST_; n++)
		{
			if (n != RKERRT_FATAL)		//exept FATALS !
				ecIgnoreType(n, oNewFlags);
		}
	}



	//---------------------------------------------------------------------------------------------------------------------------------
	void RKErrConf::ecIncrementErrorCount(RKASError *pErr)
		//---------------------------------------------------------------------------------------------------------------------------------
	{
		//an error jest occured : add to the count for final check
		m_totalCount++;

		if (pErr)
		{
			if (pErr->m_type >= RKERRT_NONE && pErr->m_type < RKERRT_LAST_)
			{
				m_typeConf[pErr->m_type].m_ErrorCount++;
			}
		}
	}

	//---------------------------------------------------------------------------------------------------------------------------------
	int RKErrConf::ecGetOutputFlags(RKASError *pErr, int *pNbOccurences)
		//---------------------------------------------------------------------------------------------------------------------------------
	{
		// internal : we keep a list of user-selected output levels
		//--- check if aleady in list ---
		int ne = MLERR_IGNORE_MAX;

		int n;
		for (n = 0; n < ne; n++)
		{
			RKErrIgnoreInfo *lst = &m_ignoreList[n];

			if ((lst->m_line == 0 || lst->m_line == pErr->m_line) && lst->m_filename && strcmp(lst->m_filename, pErr->m_filename) == 0)
			{
				//found ! this error already occured, and was ignored
				lst->m_nbOccurence++;

				if (pNbOccurences)
					*pNbOccurences = lst->m_nbOccurence;

				return lst->m_oFlag;
			}
		}

		//this error didn't occured : first time now

#if ML_USE_MEMORY==1
		//#if MLMEMORY_GLOBAL_CONSTRUCTOR_ENABLED
		if (MLMemoryHeap::mainHeap() == NULL)  //not yet initialized ... 
			return oALL; //oDEFAULT;
#endif


		return pErr->m_oFlags;	 //set in contructor / by type() / by force_oFlag()
		//return oDEFAULT;	//use the global setting dependinf of type of error   	//(oIDE | oLOG | oDISP);		//all
	}


}

