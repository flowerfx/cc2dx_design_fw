//**************************************************************************************************************************************
//*
//* RKAssertPrivate.h
//*
//* [Christophe Le Bouil]
//**************************************************************************************************************************************
#ifndef _RK_ASSERT_PRIVATE_H_
#define _RK_ASSERT_PRIVATE_H_
namespace RKUtils
{
	//------ types of error ----------
	enum RKERRT										//make sure to match the error type names : see gRKTblErrorTypeName[]
	{
		RKERRT_NONE,
		RKERRT_WARNING,
		RKERRT_ASSERT,
		RKERRT_FATAL,

		RKERRT_LAST_,		//"Unknown error"
		RKERRT_REMOVED

	};



	//-------------------------------------------------------------
#ifndef RKTARGET    //should be in RKTarget.h [TODO]
#define RKTARGET_NONE   0
#define RKTARGET_WIN32  9876
#define RKTARGET_ANDRO  9875
#define RKTARGET_IOS    9874

#ifdef GAME_WIN32
#define RKTARGET RKTARGET_WIN32
#endif
#ifdef GAME_ANDROID
#define RKTARGET RKTARGET_ANDRO
#endif
#ifdef GAME_IOS
#define RKTARGET RKTARGET_IOS
#endif

#ifndef RKTARGET
#ifdef _WIN32
#define RKTARGET RKTARGET_WIN32
#else
#include "OOPPSS  RKTARGET not defined!"
#endif
#endif

#endif //RKTARGET
	//-------------------------------------

#if (RKTARGET==RKTARGET_WIN32)
	typedef unsigned char 				rku8;				///<unsigned 8 bit value
	typedef signed char 				rks8;				///<signed 8 bit value
	typedef unsigned short 				rku16;			///<unsigned 16 bit value
	typedef signed short 				rks16;			///<signed 8 bit value
	typedef float               		rkf32;			///<float value
	typedef unsigned short 				rkf16;			///< half float unsigned 16 bit value
	typedef unsigned int 				rku32;
	typedef signed int 					rks32;
	typedef unsigned __int64 			rku64;
	typedef signed __int64 				rks64;
	typedef unsigned __int64 			rknptr;
#else
	//.. to check / customize ..
	typedef unsigned char 				rku8;				///<unsigned 8 bit value
	typedef signed char 					rks8;				///<signed 8 bit value
	typedef unsigned short 				rku16;			///<unsigned 16 bit value
	typedef signed short 					rks16;			///<signed 8 bit value
	typedef float               	rkf32;			///<float value
	typedef unsigned short 				rkf16;			///< half float unsigned 16 bit value
	typedef unsigned int 				rku32;
	typedef signed int 					rks32;
	typedef unsigned long long 		rku64;    //not on android ..
	typedef signed long long  			rks64;
	typedef unsigned long long  		rknptr;
#endif




	//***********************************************************************************************************************************
	//*
	//*   MLA_typeID()  and  MLA_typeof()
	//*   RKSA_
	//*
	//***********************************************************************************************************************************


	template<int N> struct RKSA_typeof_class; // no deffn, only specializations
	template<class T> struct RKSA_WrapType { typedef T U; };

#define RKSA_REGISTER_TYPE(N,T) \
    template<> struct RKSA_typeof_class<N> { typedef RKSA_WrapType<T>::U V; }; \
    char (*RKSA_typeof_fct(const RKSA_WrapType<T>::U &))[N];

#define RKSA_typeof(x) RKSA_typeof_class<sizeof(*RKSA_typeof_fct(x))>::V

#define RKSA_typeID(x) sizeof(*RKSA_typeof_fct(x))

	//======== Registration of types to be used with "RKSA_typeID" ========

	RKSA_REGISTER_TYPE(1, char)
		RKSA_REGISTER_TYPE(2, signed char)
		RKSA_REGISTER_TYPE(3, unsigned char)
		RKSA_REGISTER_TYPE(4, short)
		RKSA_REGISTER_TYPE(5, unsigned short)
		RKSA_REGISTER_TYPE(6, int)
		RKSA_REGISTER_TYPE(7, unsigned int)
		RKSA_REGISTER_TYPE(8, long)
		RKSA_REGISTER_TYPE(9, unsigned long)
		// RKSA_REGISTER_TYPE( 10, long long ) - may be unknown type 
		// RKSA_REGISTER_TYPE( 11, unsigned long long ) - may be unknown type 
		RKSA_REGISTER_TYPE(12, float)
		RKSA_REGISTER_TYPE(13, double)
		// RKSA_REGISTER_TYPE( 14, wchar_t ) - may be unknown type 
		RKSA_REGISTER_TYPE(15, int(*)())

		//******* ModuLib types and classes **********
#ifdef _MLDATATYPE_H_									// see <MLDataType.h>
		//RKSA_REGISTER_TYPE( 17, rku8 )
		//RKSA_REGISTER_TYPE( 17, rks8 )
		//RKSA_REGISTER_TYPE( 22, rku16 )
		//RKSA_REGISTER_TYPE( 20, rku16 )
		//RKSA_REGISTER_TYPE( 20, rku32 )
		//RKSA_REGISTER_TYPE( 20, rks32 )
		//RKSA_REGISTER_TYPE( 20, rkf32 )
		//RKSA_REGISTER_TYPE( 20, rku64 )		//   if needed, must change RKArgument::value to be 64 bits !
		//RKSA_REGISTER_TYPE( 20, rks64 )

		RKSA_REGISTER_TYPE( 17, rku8 *)
		RKSA_REGISTER_TYPE( 18, rks8 *)
		RKSA_REGISTER_TYPE( 19, rk u16 *)
		RKSA_REGISTER_TYPE( 20, rks16 *)
		RKSA_REGISTER_TYPE( 21, rku32 *)
		RKSA_REGISTER_TYPE( 22, rks32 *)

		//#if (MLTARGET!=MLTARGET_DS)		//f32 is unsigned int
		RKSA_REGISTER_TYPE( 23, rkf32 *)
		//#endif

		RKSA_REGISTER_TYPE( 24, rgba)
		RKSA_REGISTER_TYPE( 25, rgba *)
#endif

#if ML_USE_STRING==1
	class MLString;	//#include <core/MLString.h>
	RKSA_REGISTER_TYPE( 26, MLString *)
		RKSA_REGISTER_TYPE( 27, MLString)						//bad : the value is the string buffer !  -> use MLString *

#endif

#if ML_USE_FONT==1
	class MLFont;
	RKSA_REGISTER_TYPE( 28, MLFont *)
#endif

#if ML_USE_MEMORY==1
	class MLMemoryBlock;
	RKSA_REGISTER_TYPE( 29, MLMemoryBlock *)
#endif

#if ML_USE_WINDOW==1
	class MLWindow;
	RKSA_REGISTER_TYPE( 30,  MLWindow )	
		RKSA_REGISTER_TYPE( 31,  MLWindow* )	
		RKSA_REGISTER_TYPE( 32,  void (*)(MLWindow*) )			//MLWindowCallback
#endif

		RKSA_REGISTER_TYPE(33, const char *)

		RKSA_REGISTER_TYPE(34, void *)


		//***********************************************************************************************************************************
		//*
		//*   MakeRKAL()  MLA()
		//*
		//***********************************************************************************************************************************

		typedef size_t RKAVal;

	typedef struct
	{
		int						typeId;												// get by RKSA_typeID()
		RKAVal				value;												// to be casted to any type
	}RKArgument;

#define RKSA_MAX_ARG 16
	typedef struct
	{
		int						nbArguments;
		RKArgument		argument[RKSA_MAX_ARG];
	}RKArgumentList;


#define RKSA_MAX_LIST 10
	template <class T1>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *getRKArgumentList(T1 n)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		static RKArgumentList sTheArgumentListArray[RKSA_MAX_LIST];
		static int currentList = 0;

		RKArgumentList *ret = 0;  //NULL;

		if (n >= 0 && n < RKSA_MAX_LIST)
		{
			ret = &sTheArgumentListArray[currentList];
		}
		else
		{
			if (n == -1)
			{
				currentList = 0;
				ret = &sTheArgumentListArray[currentList];
			}
			else if (n == -2)
			{
				ret = &sTheArgumentListArray[currentList];
				currentList++;		//turning buffer of RKSA_MAX_LIST lists of arguments
				if (currentList >= RKSA_MAX_LIST)
					currentList = 0;
			}
		}

		if (ret)
			ret->nbArguments = 0;

		return ret;
	}





	//*** 1 to 16 arguments template
#define ADD_RK_ARG(n) lst->argument[ n].typeId = RKSA_typeID( arg##n );  T##n *pArg##n = &arg##n;  lst->argument[ n].value = (RKAVal) (*((RKAVal*)(pArg##n))) ;  lst->nbArguments++;

	template <class T0>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);

		//lst->argument[ 0].typeId = RKSA_typeID( arg0 );			lst->argument[ 0].value = (u32) arg0 ;		//too simple cast : convert float to u32 : loose decimal part !

		lst->argument[0].typeId = RKSA_typeID(arg0);

		T0 *pArg0 = &arg0;

		lst->argument[0].value = (RKAVal)(*((RKAVal*)(pArg0)));		//read the 4/8 bytes (size_t)

		lst->nbArguments++;

		//NOTE : for the other templates, we use the macro ADD_RK_ARG

		return lst;
	}


	template <class T0, class T1>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0)
			ADD_RK_ARG(1)
			return lst;
	}

	template <class T0, class T1, class T2>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0)
			ADD_RK_ARG(1)
			ADD_RK_ARG(2)
			return lst;
	}


	template <class T0, class T1, class T2, class T3>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0)
			ADD_RK_ARG(1)
			ADD_RK_ARG(2)
			ADD_RK_ARG(3)
			return lst;
	}

	template <class T0, class T1, class T2, class T3, class T4>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		return lst;
	}

	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		return lst;
	}

	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		return lst;
	}

	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		ADD_RK_ARG(10);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		ADD_RK_ARG(10);
		ADD_RK_ARG(11);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		ADD_RK_ARG(10);
		ADD_RK_ARG(11);
		ADD_RK_ARG(12);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		ADD_RK_ARG(10);
		ADD_RK_ARG(11);
		ADD_RK_ARG(12);
		ADD_RK_ARG(13);
		return lst;
	}


	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13, T14 arg14)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		ADD_RK_ARG(10);
		ADD_RK_ARG(11);
		ADD_RK_ARG(12);
		ADD_RK_ARG(13);
		ADD_RK_ARG(14);
		return lst;
	}

	template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	RKArgumentList *MakeRKAL(T0 arg0, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12, T13 arg13, T14 arg14, T15 arg15)
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	{
		RKArgumentList *lst = getRKArgumentList(-2);
		ADD_RK_ARG(0);
		ADD_RK_ARG(1);
		ADD_RK_ARG(2);
		ADD_RK_ARG(3);
		ADD_RK_ARG(4);
		ADD_RK_ARG(5);
		ADD_RK_ARG(6);
		ADD_RK_ARG(7);
		ADD_RK_ARG(8);
		ADD_RK_ARG(9);
		ADD_RK_ARG(10);
		ADD_RK_ARG(11);
		ADD_RK_ARG(12);
		ADD_RK_ARG(13);
		ADD_RK_ARG(14);
		ADD_RK_ARG(15);
		return lst;
	}





#ifndef RKUnused  //to avoid compile warnings ..  MLUnused
#define RKUnused(a) {a;}
#endif





	//****************************************************************************************************************************************************
	//from MLError.h

	//------- error output flags --------
	enum
	{
		rkoNONE = 0,
		rkoLOG = 1 << 0,
		rkoIDE = 1 << 1,
		rkoDISP = 1 << 2,
		rkoDEFAULT = 1 << 3,			//use the global setting depending of type of error
		rkoALL = 0xFFFFFFFF
	};




	//****************************************************************************************************************************************************************************
	//*
	//* RKASError Class (RK Assert Error)
	//* [partially unused .. TODO: cleanup..]
	//****************************************************************************************************************************************************************************

	class RKASError
	{
	public:

		static void errMainInit(void);
		static int errMainQuit(void);
		static void errMainInit_PC(void);
		static void errMainQuit_PC(void);
		static void errMainInit_IPAD(void);

		static void errEnableWriteErrorFile(bool enable);		//iOS : enable write "BlueScreenError.txt" in application's "Documents" folder - default is false


		RKASError();

		const char *m_filename;													// Source file, where the error macro is.
		const char *m_functionName;											// NULL or _FUNCTION_ if compiler has it
		int			    m_line;															// Line number in the source file, where the error macro is.
		const char *m_expression;												// Text version of the tested expression (eg "a>0")
		const char *m_msg;															// static text added by .msg("hello")
		int			    m_type;															// the error type (RKERRT_ASSERT, RKERRT_VERIFY, RKERRT_FATAL)
		int			    m_oFlags;														// see (oIDE | oLOG | oDISP);


		// internal : sets the expression
		RKASError & set_expr(const char * strExpr) { m_expression = strExpr; return *this; }

		// internal : sets the context from __FILE__, __LINE__
		RKASError& add_info_context(const char *file, int line);

		// internal : set the type and outFlag
		RKASError& errType(int t);// { m_type = t; return *this; }  //ex 		RKERRT_ASSERT

		RKASError& force_oFlag(int oflag) { m_oFlags = oflag; return *this; }

		//internal : the display
		///void errDisplay(void);

		//--- simple filename ptr ...
		//static void errSetCurrentLoadedFilename(char *name);
		//static char *errGetCurrentLoadedFilename(void);

	};





	//****************************************************************************************************************************************************************************
	//*
	//*  RKErrConf class 
	//*
	//*   RKErrTypeConf class				: to store the default behavior depending of error type
	//*   RKErrIgnoreInfo class			: to store individual error infos
	//*  
	//*
	//****************************************************************************************************************************************************************************
	//---------------------------------------------------------------------------------------
	class RKErrTypeConf		//default setting for each type
	{
	public:

		bool m_allowIgnoreBut;								// show the "Ignore xxx" Buttons										(typically false for RKERRT_FATAL)
		bool m_allowQuitBut;									// show the "Quit" button for warnings							(typically false for RKERRT_WARNING)
		int m_defaultOutputFlags;							// combination of oXXX flags,  see oLOG, oDISP, etc
		int m_QASettingsOutputFlags;					// combination of oXXX flags,  see oLOG, oDISP, etc
		int m_ErrorCount;											// nb of time error of this type occurred

		RKErrTypeConf();											// see MLErrTypeConf_CTORHERE

		void etcInitDefaultSettings(void);		//init or re-init    : called by ctor and by clearList

		void etcSetoFlags(int wantLOG, int wantIDE, int wantDISP);

	};


	//---------------------------------------------------------------------------------------
	class RKErrIgnoreInfo		//specific setting for each error that occurred
	{
	public:
		const char *m_filename;
		int m_line;
		int m_oFlag;					//see oLOG etc
		int m_nbOccurence;
		int m_type;

		void iiClear(void);

		RKErrIgnoreInfo();				//see MLErrIgnoreInfo_CTORHERE
	};



#define MLERR_IGNORE_MAX 100		//nb errors in the "ignore" list
	//---------------------------------------------------------------------------------------
	class RKErrConf //MLErrConf
	{
	public:
		RKErrConf();

	private:
		RKErrTypeConf			m_typeConf[RKERRT_LAST_];					//default setting for the 4 types : RKERRT_WARNING etc..
		RKErrIgnoreInfo		m_ignoreList[MLERR_IGNORE_MAX];		//specific setting for each error that occurred
		int m_maxLoggedOccurence;														//0 or the limit of the same error to log
		bool m_quitRequestedForError;
		int m_totalCount;

	public:

		//--- types properties ----
		void ecSetTypeOutFlag(int type, int flags);					//to manually override default settings
		int ecGetTypeOutFlag(int type);

		bool ecAllowIgnoreBut(int type);
		bool ecAllowQuitBut(int type);

		RKErrTypeConf* ecGetTypeConfiguration(int type);

		//--- individual error properties ----
		void ecIgnoreError(RKASError *pErr, int oNewFlags);			//add error to the ignore list, and set this output flags
		int ecGetOutputFlags(RKASError *pErr, int *pNbOccurences);				//using 1)ignore list 2)error settings 3)default type settings 
		void ecClearIgnoreList(void);

		//--other ignore ----
		void ecIgnoreError(const char *filename, int line, int oNewFlags);		//to "manually" ignore some errors in your project
		void ecIgnoreSource(const char *filename, int oNewFlags);
		int ecIgnoreType(int errorType, int oNewFlags);									//oNewFlags : see rkoNONE
		void ecIgnoreAll(int oNewFlags);																//exept FATALS !

		//--- general properties
		void ecIncrementErrorCount(RKASError *pErr);
		int ecGetTotalCount(void)	{ return m_totalCount; }

		int ecGetMaxLogOccurence(void) { return m_maxLoggedOccurence; }

		void ecQuittRequestedForError(void) { m_quitRequestedForError = true; }
		bool ecIsQuitRequestedForError(void) { return m_quitRequestedForError; }

		RKErrIgnoreInfo *ecGetErrorInfo(int n);

		void ecInitDefaultSettings(void);						//init or re-init    :  called by ctor and ecClearIgnoreList() 

	};

	extern RKErrConf gRKErrConf;		//<---- The only instance	

	void RKErrorDisplay(int errType, const char *cond, RKArgumentList *arg, const char *file, const char *fn, int line);

}

#endif //_RK_ASSERT_PRIVATE_H_

