#include "RKString.h"
#include "RKMacros.h"

#include <sstream>
#include <map>
namespace RKUtils
{
class _RKStringTestClass
{
public:

static RKString longString() { return "abcdefghijklmnopqrstuvwxyz0123456789"; }

static void _CheckString(const RKString& str)
{
	str._SanityCheck();
}

static void _TestConstructors()
{
	RKString s;
	_CheckString(s);
	RKASSERT(s == "", "");

	RKString s1("abc");
	_CheckString(s1);
	RKASSERT(s1 == "abc", "");

	RKString s11(RKString("abc"));
	_CheckString(s11);
	RKASSERT(s11 == "abc", "");

	RKString s2(longString().GetString());
	_CheckString(s2);
	RKASSERT(s2 == longString(), "");

	RKString s22(longString());
	_CheckString(s22);
	RKASSERT(s22 == longString().GetString(), "");

	RKString s3(std::string("abc"));
	_CheckString(s3);
	RKASSERT(s3 == "abc", "");

	RKString s4 = RKString::MakeFormatted("%s %d", "abc", 5);
	_CheckString(s4);
	RKASSERT(s4 == "abc 5", "");

	RKString s44 = RKString::MakeFormatted("abc");
	_CheckString(s44);
	RKASSERT(s44 == "abc", "");

	RKString s444 = RKString::MakeFormatted("");
	_CheckString(s444);
	RKASSERT(s444 == "", "");
}

static void _TestAssignAppendPrepend()
{
	RKString s;
	RKASSERT(s.Length() == 0, "");
	_CheckString(s);

	RKString s2 = "abc";
	RKASSERT(s2.Length() == 3, "");
	_CheckString(s2);

	RKString s3 = "def";
	s3 = "";
	RKASSERT(s3.Length() == 0, "");
	_CheckString(s3);

	RKString s4;
	RKASSERT(s4.Length() == 0, "");
	_CheckString(s4);
	s4 = "abc";
	RKASSERT(s4.Length() == 3, "");
	_CheckString(s4);
	s4 += "def";
	RKASSERT(s4.Length() == 6, "");
	_CheckString(s4);
	s4 += RKString("jhi");
	RKASSERT(s4.Length() == 9, "");
	_CheckString(s4);

	RKString s5 = longString();
	RKASSERT(s5.Length() == longString().Length(), "");
	_CheckString(s5);
	s5 += RKString("abc");
	RKASSERT(s5.Length() == longString().Length() + 3, "");
	_CheckString(s5);

	RKString s6 = "abc";
	s6.Append("defj", 1, 2);
	_CheckString(s6);
	RKASSERT(s6 == "abcef", "");

	RKString s66 = "abc";
	s66.Append("defj", 3);
	_CheckString(s66);
	RKASSERT(s66 == "abcdef", "");

	RKString s666 = "abc";
	s666.Append("defj", 1, 0);
	_CheckString(s666);
	RKASSERT(s666 == "abc", "");

	RKString s7 = "abc";
	s7.Assign("defj", 1, 2);
	_CheckString(s7);
	RKASSERT(s7 == "ef", "");

	RKString s77 = "abc";
	s77.Assign("defj", 3);
	_CheckString(s77);
	RKASSERT(s77 == "def", "");

	RKString s777 = "abc";
	s777.Assign("defj", 1, 0);
	_CheckString(s777);
	RKASSERT(s777 == "", "");

	{ // ranged assignments
		{
			RKString s = "abc";
			s.Assign("defj", -100, 10);
			_CheckString(s);
			RKASSERT(s == "", "");
		}

		{
			RKString s = "abc";
			s.Assign("defj", 100, 10);
			_CheckString(s);
			RKASSERT(s == "", "");
		}

		{
			RKString s = "abc";
			s.Assign("defj", -9, 10);
			_CheckString(s);
			RKASSERT(s == "d", "");
		}

		{
			RKString s = "abc";
			s.Assign("defj", 3, 10);
			_CheckString(s);
			RKASSERT(s == "j", "");
		}

		{
			RKString s = "abc";
			s.Assign("defj", -100, 200);
			_CheckString(s);
			RKASSERT(s == "defj", "");
		}
	}
	
}

static void _TestConcatenataion()
{
	RKString s = RKString("abc") + "def" + "hij";
	_CheckString(s);
	RKASSERT(s.Length() == 9, "");

	RKString s2 = longString() + "def" + "hij";
	_CheckString(s2);
	RKASSERT(s2.Length() == longString().Length() + 6, "");

	RKString s22 = "def" + longString() + "hij";
	_CheckString(s22);
	RKASSERT(s22.Length() == longString().Length() + 6, "");

	RKString s3 = RKString("") + "abc";
	_CheckString(s3);
	RKASSERT(s3.Length() == 3, "");

	RKString s4 = RKString("") + "";
	_CheckString(s4);
	RKASSERT(s4.Length() == 0, "");

	RKString s5 = longString() + "";
	_CheckString(s5);
	RKASSERT(s5.Length() == longString().Length(), "");

	RKString s6 = "" + longString();
	_CheckString(s6);
	RKASSERT(s6.Length() == longString().Length(), "");
}

static void _TestToLowerToUpper()
{
	RKString s("aBcD");
	s.ToUpperCase();
	_CheckString(s);
	RKASSERT(s == "ABCD", "");

	RKString s1("aBcD");
	s1.ToLowerCase();
	_CheckString(s1);
	RKASSERT(s1 == "abcd", "");

	RKString s3("");
	s3.ToLowerCase();
	_CheckString(s3);
	RKASSERT(s3 == "", "");
	s3.ToUpperCase();
	_CheckString(s3);
	RKASSERT(s3 == "", "");
}

static void _TestStrip()
{
	RKString s("  abc    ");
	s.StripLeft();
	_CheckString(s);
	RKASSERT(s == "abc    ", "");

	RKString s2("  abc    ");
	s2.StripRight();
	_CheckString(s2);
	RKASSERT(s2 == "  abc", "");

	RKString s3("  abc    ");
	s3.Strip();
	_CheckString(s3);
	RKASSERT(s3 == "abc", "");

	RKString s4;
	s4.StripLeft();
	_CheckString(s4);
	RKASSERT(s4 == "", "");
	s4.StripRight();
	_CheckString(s4);
	RKASSERT(s4 == "", "");
	s4.Strip();
	_CheckString(s4);
	RKASSERT(s4 == "", "");
	s4 = "         ";
	s4.Strip();
	_CheckString(s4);
	RKASSERT(s4 == "", "");

	RKString s5 = "   " + longString() + " ";
	s5.Strip();
	_CheckString(s5);
	RKASSERT(s5 == longString(), "");

	RKString s6 = " qq  " + longString() + " q";
	s6.Strip(" q");
	_CheckString(s6);
	RKASSERT(s6 == longString(), "");

	RKString s7 = "   ";
	s7.Strip("");
	_CheckString(s7);
	RKASSERT(s7 == "   ", "");

	RKString s8 = "                             a                                    ";
	s8.Strip();
	_CheckString(s8);
	RKASSERT(s8 == "a", "");
}

static void _TestCompare()
{
	const RKString a("ss_abc");
	const RKString b("ss_bcd");
	const RKString A("ss_ABC");
	const RKString B("ss_BCD");
	const RKString s("s");

	RKASSERT(a.Compare(a) == 0, "");
	RKASSERT(a.ICompare(a) == 0, "");
	RKASSERT(a.Compare(A) == 1, "");
	RKASSERT(A.ICompare(A) == 0, "");

	RKASSERT(a.Compare(b) == -1, "");
	RKASSERT(b.Compare(a) == 1, "");
	RKASSERT(a.ICompare(b) == -1, "");
	RKASSERT(b.ICompare(a) == 1, "");

	RKASSERT(a.Compare(B) == 1, "");
	RKASSERT(a.ICompare(B) == -1, "");

	RKASSERT(b.Compare(s) == 1, "");
	RKASSERT(a.Compare(s) == 1, "");
	RKASSERT(b.ICompare(s) == 1, "");
	RKASSERT(a.ICompare(s) == 1, "");

	RKASSERT(a.Compare("") == 1, "");
	RKASSERT(a.ICompare("") == 1, "");

	RKASSERT(a == "ss_abc", "");
	RKASSERT("ss_abc" == a, "");
	RKASSERT(a != "!ss_abc", "");
	RKASSERT("!ss_abc" != a, "");
}

static void _TestSubstring()
{
	const RKString s("abcdef");

	RKString s1 = s.Substring(2, 3);
	_CheckString(s1);
	RKASSERT(s1 == "cde", "");

	s1 = s.Substring(2, 1);
	_CheckString(s1);
	RKASSERT(s1 == "c", "");

	s1 = s.Substring(2, -1);
	_CheckString(s1);
	RKASSERT(s1 == "", "");

	s1 = s.Substring(-1, 2);
	_CheckString(s1);
	RKASSERT(s1 == "a", "");

	s1 = s.Substring(3, 200);
	_CheckString(s1);
	RKASSERT(s1 == "def", "");

	s1 = s.Substring(-100, 200);
	_CheckString(s1);
	RKASSERT(s1 == "abcdef", "");

	s1 = s.Left(2);
	_CheckString(s1);
	RKASSERT(s1 == "ab", "");

	s1 = s.Left(-2);
	_CheckString(s1);
	RKASSERT(s1 == "", "");

	s1 = s.Left(0);
	_CheckString(s1);
	RKASSERT(s1 == "", "");

	s1 = s.Right(2);
	_CheckString(s1);
	RKASSERT(s1 == "ef", "");

	s1 = s.Right(-2);
	_CheckString(s1);
	RKASSERT(s1 == "", "");

	s1 = s.Right(0);
	_CheckString(s1);
	RKASSERT(s1 == "", "");

	s1 = longString().Substring(10, 2);
	_CheckString(s1);
	RKASSERT(s1 == "kl", "");

	s1 = longString().Left(3);
	_CheckString(s1);
	RKASSERT(s1 == "abc", "");

	s1 = longString().Right(3);
	_CheckString(s1);
	RKASSERT(s1 == "789", "");
}

static void _TestStartsEndsWith()
{
	const RKString s = "abcdef";
	
	RKASSERT(s.StartsWith(""), "");
	RKASSERT(s.StartsWith("abc"), "");
	RKASSERT(s.StartsWith("abcdef"), "");
	RKASSERT(!s.StartsWith("abcdefg"), "");
	RKASSERT(!s.StartsWith("qabc"), "");

	RKASSERT(s.EndsWith(""), "");
	RKASSERT(s.EndsWith("def"), "");
	RKASSERT(s.EndsWith("abcdef"), "");
	RKASSERT(!s.EndsWith("qabcdef"), "");
	RKASSERT(!s.EndsWith("qabc"), "");
}

static void _TestFind()
{
	const RKString s("abcdef_edcb");

	RKASSERT(s.FindFirst("") == 0, "");
	RKASSERT(s.FindFirst("q") == -1, "");
	RKASSERT(s.FindFirst("dc") == 8, "");
	RKASSERT(s.FindFirst("def_ed") == 3, "");

	RKASSERT(s.FindFirst("b") == 1, "");
	RKASSERT(s.FindFirst("b", 2) == 10, "");
	RKASSERT(s.FindFirst("b", 2, 10) == -1, "");
	RKASSERT(s.FindFirst("ef", 10, 1) == 4, "");
	RKASSERT(s.FindFirst("c", -3, -1) == 9, "");

	RKASSERT(s.FindLast("") == 11, ""); // ????
	RKASSERT(s.FindLast("q") == -1, "");
	RKASSERT(s.FindLast("b") == 10, "");
	RKASSERT(s.FindLast("cd") == 2, "");
	RKASSERT(s.FindLast("def_ed") == 3, "");

	RKASSERT(s.FindLast("b") == 10, "");
	RKASSERT(s.FindLast("b", 9) == 1, "");
	RKASSERT(s.FindLast("b", 9, 1) == 1, "");
	RKASSERT(s.FindLast("b", 9, 2) == -1, "");
	RKASSERT(s.FindLast("ef", 1, 10) == 4, "");
	RKASSERT(s.FindLast("c", -10, -6) == 2, "");

	const RKString s2("1234321");

	RKASSERT(s2.FindFirst("343") == 2, "");
	RKASSERT(s2.FindFirst("343", 0, 7) == 2, "");
	RKASSERT(s2.FindFirst("343", 1, 6) == 2, "");
	RKASSERT(s2.FindFirst("343", 2, 5) == 2, "");
	RKASSERT(s2.FindFirst("343", 0, 4) == -1, "");
	RKASSERT(s2.FindFirst("343", 3, -1) == -1, "");

	RKASSERT(s2.FindLast("343") == 2, "");
	RKASSERT(s2.FindLast("343", 0, 7) == 2, "");
	RKASSERT(s2.FindLast("343", 1, 6) == 2, "");
	RKASSERT(s2.FindLast("343", 2, 5) == 2, "");
	RKASSERT(s2.FindLast("343", 2, 4) == -1, "");
	RKASSERT(s2.FindLast("343", 0, 4) == -1, "");
	RKASSERT(s2.FindLast("343", 3, -1) == -1, "");
	
}

static void _TestReplace()
{
	{
		RKString s("_abababa_");
		RKASSERT(s.ReplaceAll("aba", "C"), "");
		_CheckString(s);
		RKASSERT(s == "_CbC_", "");
	}

	{
		RKString s("_abababa_");
		RKASSERT(s.ReplaceFirst("aba", "C"), "");
		_CheckString(s);
		RKASSERT(s == "_Cbaba_", "");
	}

	{
		RKString s("_abababa_");
		RKASSERT(s.ReplaceLast("aba", "C"), "");
		_CheckString(s);
		RKASSERT(s == "_ababC_", "");
	}

	{
		RKString s("_abababa_");
		RKASSERT(!s.ReplaceLast("q", "d"), "");
		_CheckString(s);
		RKASSERT(s == "_abababa_", "");
	}

	{
		RKString s("_abababa_");
		RKASSERT(!s.ReplaceLast("ada", "awa"), "");
		_CheckString(s);
		RKASSERT(s == "_abababa_", "");
	}

	{
		RKString s("_abababa_");
		RKASSERT(s.ReplaceAll("baba", ""), "");
		_CheckString(s);
		RKASSERT(s == "_aba_", "");
	}

	{
		RKString s("_abababa_");
		RKASSERT(s.ReplaceAll("b", "a"), "");
		_CheckString(s);
		RKASSERT(s == "_aaaaaaa_", "");
	}

	{
		RKString s("aaaa");
		RKASSERT(s.ReplaceAll("a", "aa"), "");
		_CheckString(s);
		RKASSERT(s == "aaaaaaaa", "");
	}

	{
		RKString s(longString());
		RKASSERT(s.ReplaceAll("bcdefghijklmnopqrstuvwxyz012345678", "!"), "");
		_CheckString(s);
		RKASSERT(s == "a!9", "");
	}

	{
		RKString s(longString());
		RKASSERT(s.ReplaceAll("xyz", "!"), "");
		_CheckString(s);
		RKASSERT(s == "abcdefghijklmnopqrstuvw!0123456789", "");
	}

	{
		RKString s(longString());
		RKASSERT(s.ReplaceAll("xyz", "!xyz!"), "");
		_CheckString(s);
		RKASSERT(s == "abcdefghijklmnopqrstuvw!xyz!0123456789", "");
	}
	 
	{ // prefix shifting optimization test
		RKString s(longString());
		RKASSERT(s.ReplaceAll("cde", "!"), "");
		_CheckString(s);
		RKASSERT(s == "ab!fghijklmnopqrstuvwxyz0123456789", "");
	}

	{
		RKString s("abababa");
		RKASSERT(s.ReplaceFirst("a", "!", 2), "");
		_CheckString(s);
		RKASSERT(s == "!b!baba", "");
	}

	{
		RKString s("abababa");
		RKASSERT(s.ReplaceLast("a", "!", 2), "");
		_CheckString(s);
		RKASSERT(s == "abab!b!", "");
	}

	{
		RKString s("abababa!baba");
		RKASSERT(s.ReplaceFirst("aba", "a", 2), "");
		_CheckString(s);
		RKASSERT(s == "aba!baba", "");
	}

	{
		RKString s("abab!ababa");
		RKASSERT(s.ReplaceLast("aba", "a", 2), "");
		_CheckString(s);
		RKASSERT(s == "ab!aba", "");
	}

	{
		RKString s("aaa!aaa");
		RKASSERT(s.ReplaceLast("aa", "a", 2), "");
		_CheckString(s);
		RKASSERT(s == "aa!aa", "");
	}

	{
		RKString s("aaa!aaa");
		RKASSERT(s.ReplaceFirst("aa", "a", 2), "");
		_CheckString(s);
		RKASSERT(s == "aa!aa", "");
	}

	{
		RKString s("aaa!aaa");
		s.ReplaceRegion(3, 1, "?$");
		RKASSERT(s == "aaa?$aaa", "");
	}

	{
		RKString s("aaa!aaa");
		s.ReplaceRegion(3, 0, "?$");
		RKASSERT(s == "aaa?$!aaa", "");
	}

	{
		RKString s("aaa!aaa");
		s.ReplaceRegion(0, 0, "?$");
		RKASSERT(s == "?$aaa!aaa", "");
	}

	{
		RKString s("aaa!aaa");
		s.ReplaceRegion(7, 0, "?$");
		RKASSERT(s == "aaa!aaa?$", "");
	}

	{ // efficient replace, no reallocation
		RKString s = RKString::MakeRepeated("0123456789", 3);
		RKASSERT(s.Length() == 30, ""); RKASSERT(s._GetDynamicBufferSize() == 31, "");
		const char* buffer = s.GetString();
		s.ReplaceFirst("0", ""); // should be no reallocations
		RKASSERT(s == "12345678901234567890123456789", "");
		RKASSERT(s.Length() == 29, ""); RKASSERT(s._GetDynamicBufferSize() == 31, "");
		s.Prepend("!");
		RKASSERT(s == "!12345678901234567890123456789", "");
		RKASSERT(s.GetString() == buffer, "");
	}
}

static void _TestSplitJoin()
{
	{
		const RKString src("a bb ccc");
		RKList<RKString> parts = src.Split();
		RKASSERT(parts.Size() == 3, "");
		RKASSERT(parts[0] == "a", "");
		RKASSERT(parts[1] == "bb", "");
		RKASSERT(parts[2] == "ccc", "");
		RKASSERT(RKString::MakeJoined(parts, " ") == src, "");
		RKASSERT(RKString::MakeJoined(parts, "") == "abbccc", "");
		RKASSERT(RKString::MakeJoined(parts, "_!_") == "a_!_bb_!_ccc", "");

		parts = RKString("").Split();
		RKASSERT(parts.Size() == 0, "");
		
		parts = RKString("").Split(" ", true);
		RKASSERT(parts.Size() == 1, "");
		RKASSERT(parts[0] == "", "");

		parts = RKString("").Split(" ", false, true);
		RKASSERT(parts.Size() == 0, "");

		parts = RKString("").Split(" ", true, true);
		RKASSERT(parts.Size() == 1, "");
		RKASSERT(parts[0] == "", "");
	}

	{
		const RKString src("/|/|/|a/|bb/|/|ccc/|/|/|/|/|dddd");
		RKList<RKString> parts;

		parts = src.Split("/|");
		RKASSERT(parts.Size() == 4, "");
		RKASSERT(parts[0] == "a", "");
		RKASSERT(parts[1] == "bb", "");
		RKASSERT(parts[2] == "ccc", "");
		RKASSERT(parts[3] == "dddd", "");

		parts = src.Split("/|", true);
		RKASSERT(parts.Size() == 12, "");

		parts = src.Split("/|", false, true);
		RKASSERT(parts.Size() == 4, "");
		RKASSERT(parts[0] == "a/|", "");
		RKASSERT(parts[1] == "bb/|", "");
		RKASSERT(parts[2] == "ccc/|", "");
		RKASSERT(parts[3] == "dddd", "");

		parts = src.Split("/|", true, true);
		RKASSERT(parts.Size() == 12, "");
		RKASSERT(parts[0] == "/|", "");
		RKASSERT(parts[1] == "/|", "");
		RKASSERT(parts[2] == "/|", "");
		RKASSERT(parts[3] == "a/|", "");
		RKASSERT(parts[10] == "/|", "");
		RKASSERT(parts[11] == "dddd", "");
	}

	{
		const RKString src("/|/|/|a/|bb/|/|ccc/|/|/|/|/|dddd");
		RKList<RKString> parts;
		
		{ // zero splits never change the string
			parts = src.SplitFirst("/|", 0);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == src, "");

			parts = RKString("").SplitFirst(" ", 0);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");

			parts = RKString("").SplitFirst(" ", 0, true);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");

			parts = RKString("").SplitFirst(" ", 0, false, true);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");

			parts = RKString("").SplitFirst(" ", 0, true, true);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");
		}

		parts = src.SplitFirst("/|", 2);
		RKASSERT(parts.Size() == 3, "");
		RKASSERT(parts[0] == "a", "");
		RKASSERT(parts[1] == "bb", "");
		RKASSERT(parts[2] == "ccc/|/|/|/|/|dddd", "");

		parts = src.SplitFirst("/|", 5, true);
		RKASSERT(parts.Size() == 6, "");
		RKASSERT(parts[0] == "", "");
		RKASSERT(parts[1] == "", "");
		RKASSERT(parts[2] == "", "");
		RKASSERT(parts[3] == "a", "");
		RKASSERT(parts[4] == "bb", "");
		RKASSERT(parts[5] == "/|ccc/|/|/|/|/|dddd", "");

		parts = src.SplitFirst("/|", 2, false, true);
		RKASSERT(parts.Size() == 3, "");
		RKASSERT(parts[0] == "a/|", "");
		RKASSERT(parts[1] == "bb/|", "");
		RKASSERT(parts[2] == "ccc/|/|/|/|/|dddd", "");

		parts = src.SplitFirst("/|", 5, true, true);
		RKASSERT(parts.Size() == 6, "");
		RKASSERT(parts[0] == "/|", "");
		RKASSERT(parts[1] == "/|", "");
		RKASSERT(parts[2] == "/|", "");
		RKASSERT(parts[3] == "a/|", "");
		RKASSERT(parts[4] == "bb/|", "");
		RKASSERT(parts[5] == "/|ccc/|/|/|/|/|dddd", "");

		{
			parts = src.SplitFirst("/|", 1000, true, true);
			RKString dst = RKString::MakeJoined(parts, "");
			RKASSERT(dst == src, "");
		}
		
		{
			parts = src.SplitFirst("/|", 1000, true, false);
			RKString dst = RKString::MakeJoined(parts, "/|");
			RKASSERT(dst == src, "");
		}
	}

	{
		const RKString src("/|/|/|a/|/|bb/|ccc/|/|/|dddd/|");
		RKList<RKString> parts;

		{ // zero splits never change the string
			parts = src.SplitLast("/|", 0);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == src, "");

			parts = RKString("").SplitLast(" ", 0);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");

			parts = RKString("").SplitLast(" ", 0, true);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");

			parts = RKString("").SplitLast(" ", 0, false, true);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");

			parts = RKString("").SplitLast(" ", 0, true, true);
			RKASSERT(parts.Size() == 1, "");
			RKASSERT(parts[0] == "", "");
		}

		parts = src.SplitLast("/|", 2);
		RKASSERT(parts.Size() == 3, "");
		RKASSERT(parts[0] == "/|/|/|a/|/|bb", "");
		RKASSERT(parts[1] == "ccc", "");
		RKASSERT(parts[2] == "dddd", "");

		parts = src.SplitLast("/|", 6, true);
		RKASSERT(parts.Size() == 7, "");
		RKASSERT(parts[0] == "/|/|/|a/|", "");
		RKASSERT(parts[1] == "bb", "");
		RKASSERT(parts[2] == "ccc", "");
		RKASSERT(parts[3] == "", "");
		RKASSERT(parts[4] == "", "");
		RKASSERT(parts[5] == "dddd", "");
		RKASSERT(parts[6] == "", "");

		parts = src.SplitLast("/|", 3, false, true);
		RKASSERT(parts.Size() == 4, "");
		RKASSERT(parts[0] == "/|/|/|a", "");
		RKASSERT(parts[1] == "/|bb", "");
		RKASSERT(parts[2] == "/|ccc", "");
		RKASSERT(parts[3] == "/|dddd", "");

		parts = src.SplitLast("/|", 6, true, true);
		RKASSERT(parts.Size() == 7, "");
		RKASSERT(parts[0] == "/|/|/|a/|", "");
		RKASSERT(parts[1] == "/|bb", "");
		RKASSERT(parts[2] == "/|ccc", "");
		RKASSERT(parts[3] == "/|", "");
		RKASSERT(parts[4] == "/|", "");
		RKASSERT(parts[5] == "/|dddd", "");
		RKASSERT(parts[6] == "/|", "");

		{
			parts = src.SplitLast("/|", 1000, true, true);
			RKString dst = RKString::MakeJoined(parts, "");
			RKASSERT(dst == src, "");
		}

		{
			parts = src.SplitLast("/|", 1000, true, false);
			RKString dst = RKString::MakeJoined(parts, "/|");
			RKASSERT(dst == src, "");
		}
	}
}

static void _TestReverse()
{
	{
		RKString s;
		s.Reverse();
		RKASSERT(s == "", "");
	}

	{
		RKString s("a");
		s.Reverse();
		RKASSERT(s == "a", "");
	}

	{
		RKString s("abc");
		s.Reverse();
		RKASSERT(s == "cba", "");
	}

	{
		RKString s("abcd");
		s.Reverse();
		RKASSERT(s == "dcba", "");
	}
	
}

static void _TestCasts()
{
	{
		int v = 0;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "0", "");
		RKASSERT(s.ToInt() == v, "");
	}

	{
		int v = 5;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "5", "");
		RKASSERT(s.ToInt() == v, "");
	}

	{
		int v = -5;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "-5", "");
		RKASSERT(s.ToInt() == v, "");
	}

	{
		uint32 v = 0;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "0", "");
		RKASSERT(s.ToUInt() == v, "");
	}

	{
		uint32 v = 2549;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "2549", "");
		RKASSERT(s.ToUInt() == v, "");
	}

	{
		uint32 v = -1;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "4294967295", "");
		RKASSERT(s.ToUInt() == v, "");
	}

	{
		float v = 0.f;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "0", "");
		RKASSERT(s.ToFloat() == v, "");
	}

	{
		float v = -1.5f;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "-1.5", "");
		RKASSERT(s.ToFloat() == v, "");
	}

	{
		float v = 0.5f;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "0.5", "");
		RKASSERT(s.ToFloat() == v, "");
	}

	{
		float v = 0.005f;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "0.005", "");
		RKASSERT(s.ToFloat() == v, "");
	}

	{
		float v = 5000000000.f;
		RKString s = RKString::MakeWithValue(v);
		RKASSERT(s == "5e+009" || s == "5e+09", "");
		RKASSERT(s.ToFloat() == v, "");
	}
	
	// casts with base
	{
		int v = 255;
		RKString s = RKString::MakeWithValue(v, 16);
		RKASSERT(s == "0xff", "");
		RKASSERT(s.ToInt() == v, "");
	}

	{
		int v = 255;
		RKString s = RKString::MakeWithValue(v, 8);
		RKASSERT(s == "0377", "");
		RKASSERT(s.ToInt() == v, "");
	}

	{
		int v = 255;
		RKString s = RKString::MakeWithValue(v, 8, false);
		RKASSERT(s == "377", "");
		RKASSERT(s.ToInt(8) == v, "");
	}

	{
		uint32 v = -1;
		RKString s = RKString::MakeWithValue(v, 16);
		RKASSERT(s == "0xffffffff", "");
		RKASSERT(s.ToUInt() == v, "");
	}

	// strip casts
	{
		RKString s = "\t\n 5 ";
		RKASSERT(s.ToInt() == 5, "");
	}

	{
		RKString s = "  4294967295\t\n ";
		RKASSERT(s.ToUInt() == uint32(-1), "");
	}

	{
		RKString s = "  \t1.5\n ";
		RKASSERT(s.ToFloat() == 1.5f, "");
	}

	// failed casts
#if 0
	{
		RKString s = "abc";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(!s.ParseValue(vf), "");
	}

	{
		RKString s = "";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(!s.ParseValue(vf), "");
	}

	{
		RKString s = "        ";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(!s.ParseValue(vf), "");
	}

	{
		RKString s = "abc5";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(!s.ParseValue(vf), "");
	}

	{
		RKString s = "5abc";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(!s.ParseValue(vf), "");
	}

	{
		RKString s = "1 2";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(!s.ParseValue(vf), "");
	}

	{
		RKString s = "1.5";
		int vi; unsigned int vu; float vf;
		RKASSERT(!s.ParseValue(vi), "");
		RKASSERT(!s.ParseValue(vu), "");
		RKASSERT(s.ParseValue(vf), "");
	}

	{
		RKString s = "-10";
		int vi; unsigned int vu; float vf;
		RKASSERT(s.ParseValue(vi), "");
		RKASSERT(s.ParseValue(vu), ""); // questionable, but that's how "strtoul" function works
		RKASSERT(s.ParseValue(vf), "");
	}

	{
		RKString s = "0xff";
		int vi; unsigned int vu;// float vf;
		RKASSERT(s.ParseValue(vi), "");
		RKASSERT(s.ParseValue(vu), "");
		//RKASSERT(!s.ParseValue(vf), ""); // works differently on different platforms, maybe should be unified manually
		RKASSERT(vi == 255, "");
		RKASSERT(vu == 255, "");
	}

	{
		RKString s = "0377";
		int vi; unsigned int vu; float vf;
		RKASSERT(s.ParseValue(vi), "");
		RKASSERT(s.ParseValue(vu), "");
		RKASSERT(s.ParseValue(vf), "");
		RKASSERT(vi == 255, "");
		RKASSERT(vu == 255, "");
		RKASSERT(vf == 377.f, "");
	}
#endif
}

static void _StreamsTest()
{
	std::ostringstream out;
	out << RKString("test1") << RKString("test2");
	RKASSERT(out.str() == "test1test2", "");

	{
		static const uint32 kosmeUtf8Size = 11;
		uint8 kosmeUtf8[kosmeUtf8Size] = {0xce, 0xba, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, 0xce, 0xbc, 0xce, 0xb5};

		static const uint32 kosmeWCharSize = 5;
		uint32 kosmeWChar[kosmeWCharSize] = {0x03ba, 0x1f79, 0x03c3, 0x03bc, 0x03b5};

		RKString s;
		s.Assign((char*)kosmeUtf8, kosmeUtf8Size);

		std::wostringstream out;
		out << s << s;

		std::wstring res = out.str();
		RKASSERT(res.size() == 10, "");

		for (uint32 i = 0; i < res.size(); ++i)
		{
			RKASSERT(res[i] == kosmeWChar[i % kosmeWCharSize], "");
		}
	}
}

static void _AllocationsTest()
{
	{ // expand
		RKString s = RKString::MakeRepeated("a", 19);
		RKASSERT(s._GetDynamicBufferSize() == 20, ""); RKASSERT(s.Length() == 19, "");
		s.Append("0123456789");
		RKASSERT(s._GetDynamicBufferSize() == 40, ""); RKASSERT(s.Length() == 29, "");
		s.Append("0123456789");
		RKASSERT(s._GetDynamicBufferSize() == 40, ""); RKASSERT(s.Length() == 39, "");
		s.Append("0123456789");
		RKASSERT(s._GetDynamicBufferSize() == 80, ""); RKASSERT(s.Length() == 49, "");
	}

	// relocating string back without reallocation
	{
		RKString s = "abcdefghijklmnopqrstuvwxyz";
		RKASSERT(s._GetDynamicBufferSize() == 27, ""); RKASSERT(s.Length() == 26, "");
		s.Strip(2, 2);
		RKASSERT(s._GetDynamicBufferSize() == 27, ""); RKASSERT(s.Length() == 22, "");
		s.Append("1234");
		RKASSERT(s._GetDynamicBufferSize() == 27, ""); RKASSERT(s.Length() == 26, "");
		RKASSERT(s == "cdefghijklmnopqrstuvwx1234", "");
	}

	// strip shrink
	{
		{ // to SSO
			RKString s = "abcdefghijklmnopqrstuvwxyz";
			RKASSERT(s._GetDynamicBufferSize() == 27, ""); RKASSERT(s.Length() == 26, "");
			s.Strip(10, 10);
			RKASSERT(s._GetDynamicBufferSize() == 0, ""); RKASSERT(s.Length() == 6, "");
		}
		
		{ // to smaller buffer
			RKString s = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvw";
			RKASSERT(s._GetDynamicBufferSize() == 50, ""); RKASSERT(s.Length() == 49, "");
			s.Append("_");
			RKASSERT(s._GetDynamicBufferSize() == 100, ""); RKASSERT(s.Length() == 50, "");
			s.Strip(14, 16);
			RKASSERT(s._GetDynamicBufferSize() == 50, ""); RKASSERT(s.Length() == 20, "");
		}
	}

	// replace shrink
	{
		{ // to SSO
			RKString s = "abcdefghijklmnopabcdefghij";
			RKASSERT(s._GetDynamicBufferSize() == 27, ""); RKASSERT(s.Length() == 26, "");
			s.ReplaceAll("abcdefghij", "");
			RKASSERT(s._GetDynamicBufferSize() == 0, ""); RKASSERT(s.Length() == 6, "");
		}
		
		{ // to smaller buffer
			RKString s = "abcdefghij0123_01234567890123456789_abcdefghij0123";
			RKASSERT(s._GetDynamicBufferSize() == 51, ""); RKASSERT(s.Length() == 50, "");
			s.Append("_");
			RKASSERT(s._GetDynamicBufferSize() == 51*2, ""); RKASSERT(s.Length() == 51, "");
			s.ReplaceAll("abcdefghij0123", "");
			RKASSERT(s._GetDynamicBufferSize() == 51, ""); RKASSERT(s.Length() == 23, "");
		}
	}

	{
		RKString s;
		s._Reserve(21);
		RKASSERT(s._GetDynamicBufferSize() == 21, ""); RKASSERT(s.Length() == 0, "");
		s.Append("0123456789");
		RKASSERT(s._GetDynamicBufferSize() == 21, ""); RKASSERT(s.Length() == 10, "");
		s.Append("0123456789");
		RKASSERT(s._GetDynamicBufferSize() == 21, ""); RKASSERT(s.Length() == 20, "");
		s.Append("_");
		RKASSERT(s._GetDynamicBufferSize() == 42, ""); RKASSERT(s.Length() == 21, "");
		s.ReplaceAll("_", "");
		RKASSERT(s._GetDynamicBufferSize() == 42, ""); RKASSERT(s.Length() == 20, "");
		s.Append("0123456789");
		s.Append("0123456789");
		s.Append("0123456789");
		RKASSERT(s._GetDynamicBufferSize() == 84, ""); RKASSERT(s.Length() == 50, "");
		s.ReplaceFirst("0123456789", "", 4);
		RKASSERT(s._GetDynamicBufferSize() == 42, ""); RKASSERT(s.Length() == 10, "");
		s._Reserve(5);
		RKASSERT(s._GetDynamicBufferSize() == 0, ""); RKASSERT(s.Length() == 10, "");
		s._Reserve(0);
		RKASSERT(s._GetDynamicBufferSize() == 0, ""); RKASSERT(s.Length() == 10, "");
	}

	{
		RKString s = "01234567890123456789";
		RKASSERT(s._GetDynamicBufferSize() == 21, ""); RKASSERT(s.Length() == 20, "");
		s.Append("_");
		RKASSERT(s._GetDynamicBufferSize() == 42, ""); RKASSERT(s.Length() == 21, "");
		s.ReplaceAll("_", "");
		RKASSERT(s._GetDynamicBufferSize() == 42, ""); RKASSERT(s.Length() == 20, "");
	}

	{
		int baseCapacity = 128;
		RKString s = RKString::MakeRepeated("a", baseCapacity - 1);
		RKASSERT(s._GetDynamicBufferSize() == baseCapacity, ""); RKASSERT(s.Length() == baseCapacity-1, "");
		s.Append("a");
		RKASSERT(s._GetDynamicBufferSize() == baseCapacity * 2, ""); RKASSERT(s.Length() == baseCapacity, "");
		s.StripRight(1); // no reallocation should happen here
		RKASSERT(s._GetDynamicBufferSize() == baseCapacity * 2, ""); RKASSERT(s.Length() == baseCapacity - 1, "");
		s.StripRight(baseCapacity / 2 - 1); // no reallocation should happen here
		RKASSERT(s._GetDynamicBufferSize() == baseCapacity * 2, ""); RKASSERT(s.Length() == baseCapacity / 2, "");
		s.StripRight(1); // finally, reallocation
		RKASSERT(s._GetDynamicBufferSize() == baseCapacity, ""); RKASSERT(s.Length() == baseCapacity / 2 - 1, "");
	}
}

static void _RepeatedTest()
{
	{
		RKString r = RKString::MakeRepeated("123", 3);
		RKASSERT(r == "123123123", "");
	}
	{
		RKString r = RKString::MakeRepeated("123", -1);
		RKASSERT(r == "", "");
	}
	{
		RKString r = RKString::MakeRepeated("123", -0);
		RKASSERT(r == "", "");
	}
	{
		RKString r = RKString::MakeRepeated("123", 1);
		RKASSERT(r == "123", "");
	}

	{
		RKString r = RKString::MakeRepeated("123", 0, "sep");
		RKASSERT(r == "", "");
	}
	{
		RKString r = RKString::MakeRepeated("123", 1, "sep");
		RKASSERT(r == "123", "");
	}
	{
		RKString r = RKString::MakeRepeated("123", 2, "sep");
		RKASSERT(r == "123sep123", "");
	}
	{
		RKString r = RKString::MakeRepeated("123", 3, "sep");
		RKASSERT(r == "123sep123sep123", "");
	}
}

static void _FillTest()
{
	{
		RKString str = "abcdef";
		str.Fill('!');
		RKASSERT(str == "!!!!!!", "");
	}

	{
		RKString str = "abcdef";
		str.Fill('!', 3);
		RKASSERT(str == "!!!", "");
	}

	{
		RKString str = "abcdef";
		str.Fill('!', 10);
		RKASSERT(str == "!!!!!!!!!!", "");
	}
	
}

static void _CountTest()
{
	//RKASSERT(RKString("aaaaa").Count("") == 0, "");
	RKASSERT(RKString("aaaaa").Count("a") == 5, "");
	RKASSERT(RKString("aaaaa").Count("b") == 0, "");
	RKASSERT(RKString("aaaaa").Count("aa") == 2, "");
	RKASSERT(RKString("").Count("a") == 0, "");
	//RKASSERT(RKString("").Count("") == 0, "");
	RKASSERT(RKString("abababa").Count("a") == 4, "");
	RKASSERT(RKString("abababa").Count("b") == 3, "");
	RKASSERT(RKString("abababa").Count("ab") == 3, "");
	RKASSERT(RKString("abababa").Count("aba") == 2, "");
}

static void _InsertPrependTest()
{
	{
		RKString s("aaa");
		s.Prepend("bbb");
		RKASSERT(s == "bbbaaa", "");
	}

	{
		RKString s("aaa");
		s.Prepend("");
		RKASSERT(s == "aaa", "");
	}

	{
		RKString s("");
		s.Prepend("bbb");
		RKASSERT(s == "bbb", "");
	}

	{
		RKString s("aaa");
		s.Insert(1, "bbb");
		RKASSERT(s == "abbbaa", "");
	}

	{
		RKString s("aaa");
		s.Insert(0, "bbb");
		RKASSERT(s == "bbbaaa", "");
	}

	{
		RKString s("aaa");
		s.Insert(3, "bbb");
		RKASSERT(s == "aaabbb", "");
	}

	{
		RKString s("");
		s.Insert(0, "bbb");
		RKASSERT(s == "bbb", "");
	}

	{
		RKString s("aaa");
		s.Insert(1, "");
		RKASSERT(s == "aaa", "");
	}

	// efficient prepends
	{
		RKString s = RKString::MakeRepeated("0123456789", 3);
		const char* buffer = s.GetString();
		RKASSERT(s.Length() == 30, ""); RKASSERT(s._GetDynamicBufferSize() == 31, "");
		s.StripLeft(5); // no reallocation
		RKASSERT(s.Length() == 25, ""); RKASSERT(s._GetDynamicBufferSize() == 31, "");
		s.Insert(1, "!!!!!"); // no reallocation !!
		RKASSERT(s == "5!!!!!678901234567890123456789", "");
		RKASSERT(s.Length() == 30, ""); RKASSERT(s._GetDynamicBufferSize() == 31, "");
		RKASSERT(s.GetString() == buffer, "");
	}
}

static void _SelfModificationTest()
{
	// assign
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.Assign(s);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 10), "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.Assign(s.GetString() + 92, 6);
		RKASSERT(s == "234567", "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.Assign(s.GetString() + 2);
		RKASSERT(s == "23456789" + RKString::MakeRepeated("0123456789", 9), "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10) + "%d";
		s.AssignFormatted(s.GetString() + 10, 999);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 9) + "999", "");
	}
	{
		{
			RKList<RKString> parts;
			parts.Append(RKString::MakeRepeated("0123456789", 10));
			parts.Append(RKString::MakeRepeated("0123456789", 10));
			parts[0].AssignJoined(parts, "!");
			RKASSERT(parts[0] == 
				RKString::MakeRepeated("0123456789", 10) + "!" + RKString::MakeRepeated("0123456789", 10), 
				"");
		}
		{
			RKList<RKString> parts;
			parts.Append(RKString::MakeRepeated("0123456789", 10));
			parts.Append(RKString::MakeRepeated("0123456789", 10));
			parts[0].AssignJoined(parts, parts[0]);
			RKASSERT(parts[0] == RKString::MakeRepeated("0123456789", 30), "");
		}
		{
			RKList<RKString> parts;
			parts.Append(RKString::MakeRepeated("0123456789", 10));
			parts.Append(RKString::MakeRepeated("0123456789", 10));
			RKString sep = "!";
			sep.AssignJoined(parts, sep);
			RKASSERT(sep == 
				RKString::MakeRepeated("0123456789", 10) + "!" + RKString::MakeRepeated("0123456789", 10), 
				"");
		}
		
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.AssignRepeated(s, 2);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 20), "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.AssignRepeated(s, 2, s);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 30), "");
	}

	// append / prepend / insert
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.Append(s);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 20), "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.Prepend(s);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 20), "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.Insert(10, s);
		RKASSERT(s == RKString::MakeRepeated("0123456789", 20), "");
	}

	// strip / replace
	{
		RKString s = RKString::MakeRepeated(" ", 10) + RKString::MakeRepeated("a", 10) + RKString::MakeRepeated(" ", 10);
		s.Strip(s.GetString() + 25);
		RKASSERT(s == RKString::MakeRepeated("a", 10), "");
	}
	{
		RKString s = RKString::MakeRepeated("0123456789", 10);
		s.ReplaceAll(s.GetString() + 92, "");
		RKASSERT(s == RKString::MakeRepeated("01", 10), "");
	}
}

static void _EqualsUTFTest()
{
	{
		static const uint32 kosmeUtf8Size = 11;
		uint8 kosmeUtf8[kosmeUtf8Size] = {0xce, 0xba, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, 0xce, 0xbc, 0xce, 0xb5};

		static const uint32 kosmeWCharSize = 5;
		uint32 kosmeWChar[kosmeWCharSize] = {0x03ba, 0x1f79, 0x03c3, 0x03bc, 0x03b5};

		std::wstring kosmeWString;
		for (uint32 i = 0; i < kosmeWCharSize; ++i) kosmeWString.push_back(kosmeWChar[i]);

		RKString kosmeRKString = RKString::MakeSubstring((char*)kosmeUtf8, kosmeUtf8Size);
		RKASSERT(kosmeRKString == kosmeWString, "");

		std::wstring kosmeWString2 = kosmeWString;
		kosmeWString2[1] = 2;
		RKASSERT(kosmeRKString != kosmeWString2, "");

		std::wstring kosmeWString3 = kosmeWString;
		kosmeWString3.push_back('a');
		RKASSERT(kosmeRKString != kosmeWString3, "");

		std::wstring kosmeWString4 = kosmeWString;
		kosmeWString4.erase(2);
		RKASSERT(kosmeRKString != kosmeWString4, "");
	}

	{
		// wrong "kosme", latin k, broken m
		static const uint32 kosmeUtf8Size = 9;
		uint8 kosmeUtf8[kosmeUtf8Size] = {/* latin k */ 0x6b, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, /* broken */ 0x80, 0xce, 0xb5};

		static const uint32 kosmeWCharSize = 5;
		uint32 kosmeWChar[kosmeWCharSize] = {'k', 0x1f79, 0x03c3, '?', 0x03b5};

		std::wstring kosmeWString;
		for (uint32 i = 0; i < kosmeWCharSize; ++i) kosmeWString.push_back(kosmeWChar[i]);

		RKString kosmeRKString = RKString::MakeSubstring((char*)kosmeUtf8, kosmeUtf8Size);

		RKASSERT(kosmeRKString != kosmeWString, "");
		wchar_t* buffer = kosmeRKString.CreateDecodedBufferWCHAR();
		RKASSERT(buffer == kosmeWString, "");
		RKDELETEARRAY(buffer);
	}
}

static void _UtilsTest()
{
	{
		RKString a = "c:\\path\\file.ext";
		RKString_SetExtension(a, "2");
		RKASSERT(a == "c:\\path\\file.2", "")
	}

	{
		RKString a = "c:\\path\\file.ext";
		RKString_SetExtension(a, NULL);
		RKASSERT(a == "c:\\path\\file", "")
	}

	{
		RKString a = "c:\\path\\file.ext.txe";
		RKString_SetExtension(a, "2");
		RKASSERT(a == "c:\\path\\file.ext.2", "")
	}

	{
		RKString a = "c:\\path\\file.ext.txe";
		RKString_SetExtension(a, NULL);
		RKASSERT(a == "c:\\path\\file.ext", "")
	}

	{
		RKString a = "c:\\path\\file";
		RKString_SetExtension(a, "2");
		RKASSERT(a == "c:\\path\\file.2", "")
	}

	{
		RKString a = "c:\\path\\file";
		RKString_SetExtension(a, NULL);
		RKASSERT(a == "c:\\path\\file", "")
	}

	{
		RKString a = "c:\\path.dot\\file";
		RKString_SetExtension(a, "2");
		RKASSERT(a == "c:\\path.dot\\file.2", "")
	}

	{
		RKString a = "c:/path.dot/file";
		RKString_SetExtension(a, "2");
		RKASSERT(a == "c:/path.dot/file.2", "")
	}

	{
		RKString a = "c:\\path.dot\\file";
		RKString_SetExtension(a, NULL);
		RKASSERT(a == "c:\\path.dot\\file", "")
	}

	{
		RKString a = "c:\\path.dot\\file.ext";
		RKString_SetExtension(a, "2");
		RKASSERT(a == "c:\\path.dot\\file.2", "")
	}

	{
		RKString a = "c:\\path.dot\\file.ext";
		RKString_SetExtension(a, NULL);
		RKASSERT(a == "c:\\path.dot\\file", "")
	}
	
}

static void _TestStdMap()
{
	std::map<RKString, RKString> testMap;

	for (uint32 i = 0; i < 30; ++i)
	{
		RKString key = RKString::MakeWithValue(i);
		RKString value = RKString::MakeWithValue(100 + i);
		testMap[key] = value;
	}

	for (uint32 i = 0; i < 30; ++i)
	{
		RKString key = RKString::MakeWithValue(i);
		RKString value = RKString::MakeWithValue(100 + i);
		RKASSERT(testMap[key] == value, "");
		RKASSERT(testMap.find(key)->second == value, "");
	}

	RKASSERT(testMap.find("FUUUU") == testMap.end(), "");
}

static void _TestRKString()
{
	RKLOG_TRACE("Autotesting RKString");
	RKLOG_TRACE("sizeof(RKString) == %d", sizeof(RKString));
	RKLOG_TRACE("RKString::SSO_BUFFER_SIZE == %d", RKString::SSO_BUFFER_SIZE);
	RKLOG_TRACE("sizeof(RKString::_HeapStorage) == %d", sizeof(RKString::_HeapStorage));
	
	_CheckString(longString());
	RKASSERT(longString().Length() > RKString::SSO_BUFFER_SIZE, "");
	RKASSERT(RKString::EmptyString == "", "");
   
    #if !defined(__arm64__)
     //potential fix 1 : #pragma pack 1 : saves 4 bytes per RKString, but get unaligned pointer access
     //potential fix 2 : set SSO_BUFFER_SIZE to use extra bytes (16->24) but will result in different behaviour of games ..
	RKSTATICASSERT(sizeof(RKString::_HeapStorage) <= RKString::SSO_BUFFER_SIZE, "Wasted memory detected");
	RKSTATICASSERT(sizeof(RKString) == (RKString::SSO_BUFFER_SIZE + 4), "Bad padding on this platform, wasted memory");
    #endif
	
	_TestConstructors();

	_TestAssignAppendPrepend();	

	_TestConcatenataion();

	_TestToLowerToUpper();

	_TestStrip();

	_TestCompare();

	_TestSubstring();

	_TestStartsEndsWith();

	_TestFind();

	_TestReplace();

	_TestSplitJoin();

	_TestReverse();

	_TestCasts();

	_StreamsTest();

	_AllocationsTest();

	_RepeatedTest();

	_FillTest();

	_CountTest();

	_InsertPrependTest();

	_SelfModificationTest();

	_EqualsUTFTest();

	_UtilsTest();

	_TestStdMap();

	RKLOG_TRACE("RKString autotest was successfull");
}

}; // _RKStringTestClass

void _TestRKString()
{
	_RKStringTestClass::_TestRKString();
}

}