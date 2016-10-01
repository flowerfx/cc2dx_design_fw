#pragma once


///////////////////////////////////////////////////////////////////////////////
// Safe enum implementation

template<typename def, typename storageType = typename def::type>
class safe_enum : public def
{
	storageType val;
public:
	typedef typename def::type type;

	safe_enum() {}
	safe_enum(type v) : val((storageType)v) {}
	explicit safe_enum(int v) : val((storageType)v) {}
	storageType value() const { return val; }

	const char* name() const { return def::name((type)val); }

	bool operator==(const safe_enum & s) const { return this->val == s.val; }
	bool operator!=(const safe_enum & s) const { return this->val != s.val; }
	bool operator< (const safe_enum & s) const { return this->val <  s.val; }
	bool operator<=(const safe_enum & s) const { return this->val <= s.val; }
	bool operator> (const safe_enum & s) const { return this->val >  s.val; }
	bool operator>=(const safe_enum & s) const { return this->val >= s.val; }
};

//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// helpers to convert enums to string 
// add a name function in your struct definition

#define ENUM_START(t) switch(t) {
#define ENUM_CASE(type) case type: return #type; 
#define ENUM_END() } return "";						

#define JET_H_INCLUDED_CORRECTLY

#if defined(_DEBUG)
#	define JET_STRINGIZE(X) JET_DO_STRINGIZE(X)
#	define JET_DO_STRINGIZE(X) #X
#	define PTODO_MESSAGE_STRING(msg) __FILE__ "(" JET_STRINGIZE(__LINE__)") : TODO - " msg " - [ "__FUNCTION__ " ]"
#	if defined(_MSC_VER) // Pragma messages are only supported on MSVC...
#		define PTODO(msg) __pragma( message( PTODO_MESSAGE_STRING(msg) ) )
#		define PDEPRECATED(msg) PTODO(msg)
#	elif defined(__GNUC__) && defined(GCC_VERSION) && GCC_VERSION >= 40400  // And GCC > 4.4.x
#		define PTODO_MESSAGE_GNUC(msg) _Pragma(#msg)
#		define PTODO(msg) PTODO_MESSAGE_GNUC( message( PTODO_MESSAGE_STRING(msg) ) )
#		define PDEPRECATED(msg) PTODO(msg)
#	else
#		define PTODO(msg)
#		define PDEPRECATED(msg)
#	endif
#else
#	define PTODO(msg)
#	define PDEPRECATED(msg)
#endif

#pragma warning(disable:4530) // c++ exception handler used , but unwind semantic are not enabled


///////////////////////////////////////////////////////////////////////////////
// example
//struct CullStatusDef
//{
//	enum type
//	{
//		INSIDE = 0,
//		OUTSIDE,
//		INTERSECTING
//	};
//	
//	inline const char* name(type t)	const
//	{
//		ENUM_START(t)
//		   ENUM_CASE(INSIDE)
//		   ENUM_CASE(OUTSIDE)
//		   ENUM_CASE(INTERSECTING)
//		ENUM_END()		
//	}
//};
//typedef safe_enum<CullStatusDef> CullStatus;
//
///////////////////////////////////////////////////////////////////////////////
