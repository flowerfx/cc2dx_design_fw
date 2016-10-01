#include "RKStringUTF.h"
namespace RKUtils
{
// see also http://www.columbia.edu/~fdc/utf8/

// random data generated using following python script:
// you'll need a wide build of python to run this code
// or you can use http://ideone.com online interpreter

/*
import random

def randomCodeWithSpecificUtf8Lenght(desiredLength):
  minUtfCode = 0
  maxUtfCode = 0x10FFFF

  if desiredLength == 1:
    minUtfCode = 1
    maxUtfCode = 0x7F
  elif desiredLength == 2:
    minUtfCode = 0x7F + 1
    maxUtfCode = 0x07FF
  elif desiredLength == 3:
    minUtfCode = 0x07FF + 1
    maxUtfCode = 0xFFFF
  elif desiredLength == 4:
    minUtfCode = 0xFFFF + 1
    #maxUtfCode = 0x1FFFFF
    maxUtfCode = 0x110000
  else:
    assert(False)

  res = random.randint(minUtfCode, maxUtfCode)

	# don't return banned UTF codes
  while desiredLength == 3 and ((0xD800 <= res <= 0xDFFF) or (0xFFFE <= res <= 0xFFFF)):
    res = random.randint(minUtfCode, maxUtfCode)

  return res

def generateTestData(count):
  print 'const uint32 utfSamplesCount = %s;' % count
  print 'uint32 utf32Codes[%s];' % count
  print 'uint8* utf8Codes[%s];' % count
  print 'int    utf8CodeLengths[%s];' % count
  print
  for i in range(count):
    utf32Code = randomCodeWithSpecificUtf8Lenght(random.randint(1, 4))
    utf8String = unichr(utf32Code).encode('utf8')
    utf8Codes = map(ord, utf8String)
    utf8Codes = map(hex, utf8Codes)

    print 'utf32Codes[%s]      = %s;' % (i, utf32Code)
    print 'uint8 utf8Code%s[]  = {%s};' % (i, ', '.join(utf8Codes))
    print 'utf8Codes[%s] = utf8Code%s;' % (i, i)
    print 'utf8CodeLengths[%s] = %s;' % (i, len(utf8String))
    
generateTestData(256)
*/

static void _TestEndodeDecode(uint8* utf8, uint32 utf32, uint32 expectedOffset, RKStringUTF_RC expectedDecodingRC)
{
	{
		uint32 decoded;
		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_DecodeUTF8((char*)(utf8), -1, decoded, offset);
		RKASSERT(rc == expectedDecodingRC, "");
		RKASSERT(offset == expectedOffset, "");
		RKASSERT(RKStringUTF_NextCodeLength((char*)utf8, -1) == expectedOffset, "");

		if (rc == RKStringUTF_SUCCESS || rc == RKStringUTF_OVERLONG_CODE)
		{
			RKASSERT(decoded == utf32, "");
		}
	}

	if (expectedDecodingRC == RKStringUTF_SUCCESS || expectedDecodingRC == RKStringUTF_OVERLONG_CODE || expectedDecodingRC == RKStringUTF_ZERO_CODE)
	{
		uint8 encoded[4];
		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_EncodeUTF8(utf32, (char*)encoded, 4, offset);
		RKASSERT(RKStringUTF_EncodedLength(utf32) == offset, "");
		if (expectedDecodingRC == RKStringUTF_SUCCESS || expectedDecodingRC == RKStringUTF_ZERO_CODE)
		{
			RKASSERT(rc == expectedDecodingRC, "");
			RKASSERT(offset == expectedOffset, "");
			for (uint32 ci = 0; ci < offset; ++ci)
			{
				RKASSERT(encoded[ci] == utf8[ci], "");
			}
		}

		if (expectedDecodingRC == RKStringUTF_OVERLONG_CODE)
		{
			RKASSERT(rc == RKStringUTF_SUCCESS, "");
			RKASSERT(offset < expectedOffset, "");
		}
	}
}


static void _TestOnRandomData()
{
	const uint32 utfSamplesCount = 256;
	uint32 utf32Codes[256];
	uint8* utf8Codes[256];
	int    utf8CodeLengths[256];

	utf32Codes[0]      = 46727;
	uint8 utf8Code0[]  = {0xeb, 0x9a, 0x87};
	utf8Codes[0] = utf8Code0;
	utf8CodeLengths[0] = 3;
	utf32Codes[1]      = 847;
	uint8 utf8Code1[]  = {0xcd, 0x8f};
	utf8Codes[1] = utf8Code1;
	utf8CodeLengths[1] = 2;
	utf32Codes[2]      = 26;
	uint8 utf8Code2[]  = {0x1a};
	utf8Codes[2] = utf8Code2;
	utf8CodeLengths[2] = 1;
	utf32Codes[3]      = 652;
	uint8 utf8Code3[]  = {0xca, 0x8c};
	utf8Codes[3] = utf8Code3;
	utf8CodeLengths[3] = 2;
	utf32Codes[4]      = 14;
	uint8 utf8Code4[]  = {0xe};
	utf8Codes[4] = utf8Code4;
	utf8CodeLengths[4] = 1;
	utf32Codes[5]      = 102;
	uint8 utf8Code5[]  = {0x66};
	utf8Codes[5] = utf8Code5;
	utf8CodeLengths[5] = 1;
	utf32Codes[6]      = 77;
	uint8 utf8Code6[]  = {0x4d};
	utf8Codes[6] = utf8Code6;
	utf8CodeLengths[6] = 1;
	utf32Codes[7]      = 740946;
	uint8 utf8Code7[]  = {0xf2, 0xb4, 0xb9, 0x92};
	utf8Codes[7] = utf8Code7;
	utf8CodeLengths[7] = 4;
	utf32Codes[8]      = 20;
	uint8 utf8Code8[]  = {0x14};
	utf8Codes[8] = utf8Code8;
	utf8CodeLengths[8] = 1;
	utf32Codes[9]      = 249;
	uint8 utf8Code9[]  = {0xc3, 0xb9};
	utf8Codes[9] = utf8Code9;
	utf8CodeLengths[9] = 2;
	utf32Codes[10]      = 445338;
	uint8 utf8Code10[]  = {0xf1, 0xac, 0xae, 0x9a};
	utf8Codes[10] = utf8Code10;
	utf8CodeLengths[10] = 4;
	utf32Codes[11]      = 372;
	uint8 utf8Code11[]  = {0xc5, 0xb4};
	utf8Codes[11] = utf8Code11;
	utf8CodeLengths[11] = 2;
	utf32Codes[12]      = 1111;
	uint8 utf8Code12[]  = {0xd1, 0x97};
	utf8Codes[12] = utf8Code12;
	utf8CodeLengths[12] = 2;
	utf32Codes[13]      = 99;
	uint8 utf8Code13[]  = {0x63};
	utf8Codes[13] = utf8Code13;
	utf8CodeLengths[13] = 1;
	utf32Codes[14]      = 627676;
	uint8 utf8Code14[]  = {0xf2, 0x99, 0x8f, 0x9c};
	utf8Codes[14] = utf8Code14;
	utf8CodeLengths[14] = 4;
	utf32Codes[15]      = 73;
	uint8 utf8Code15[]  = {0x49};
	utf8Codes[15] = utf8Code15;
	utf8CodeLengths[15] = 1;
	utf32Codes[16]      = 923850;
	uint8 utf8Code16[]  = {0xf3, 0xa1, 0xa3, 0x8a};
	utf8Codes[16] = utf8Code16;
	utf8CodeLengths[16] = 4;
	utf32Codes[17]      = 82;
	uint8 utf8Code17[]  = {0x52};
	utf8Codes[17] = utf8Code17;
	utf8CodeLengths[17] = 1;
	utf32Codes[18]      = 596511;
	uint8 utf8Code18[]  = {0xf2, 0x91, 0xa8, 0x9f};
	utf8Codes[18] = utf8Code18;
	utf8CodeLengths[18] = 4;
	utf32Codes[19]      = 195886;
	uint8 utf8Code19[]  = {0xf0, 0xaf, 0xb4, 0xae};
	utf8Codes[19] = utf8Code19;
	utf8CodeLengths[19] = 4;
	utf32Codes[20]      = 100;
	uint8 utf8Code20[]  = {0x64};
	utf8Codes[20] = utf8Code20;
	utf8CodeLengths[20] = 1;
	utf32Codes[21]      = 2012;
	uint8 utf8Code21[]  = {0xdf, 0x9c};
	utf8Codes[21] = utf8Code21;
	utf8CodeLengths[21] = 2;
	utf32Codes[22]      = 1461;
	uint8 utf8Code22[]  = {0xd6, 0xb5};
	utf8Codes[22] = utf8Code22;
	utf8CodeLengths[22] = 2;
	utf32Codes[23]      = 25363;
	uint8 utf8Code23[]  = {0xe6, 0x8c, 0x93};
	utf8Codes[23] = utf8Code23;
	utf8CodeLengths[23] = 3;
	utf32Codes[24]      = 21872;
	uint8 utf8Code24[]  = {0xe5, 0x95, 0xb0};
	utf8Codes[24] = utf8Code24;
	utf8CodeLengths[24] = 3;
	utf32Codes[25]      = 93;
	uint8 utf8Code25[]  = {0x5d};
	utf8Codes[25] = utf8Code25;
	utf8CodeLengths[25] = 1;
	utf32Codes[26]      = 210;
	uint8 utf8Code26[]  = {0xc3, 0x92};
	utf8Codes[26] = utf8Code26;
	utf8CodeLengths[26] = 2;
	utf32Codes[27]      = 45265;
	uint8 utf8Code27[]  = {0xeb, 0x83, 0x91};
	utf8Codes[27] = utf8Code27;
	utf8CodeLengths[27] = 3;
	utf32Codes[28]      = 4675;
	uint8 utf8Code28[]  = {0xe1, 0x89, 0x83};
	utf8Codes[28] = utf8Code28;
	utf8CodeLengths[28] = 3;
	utf32Codes[29]      = 1108;
	uint8 utf8Code29[]  = {0xd1, 0x94};
	utf8Codes[29] = utf8Code29;
	utf8CodeLengths[29] = 2;
	utf32Codes[30]      = 348;
	uint8 utf8Code30[]  = {0xc5, 0x9c};
	utf8Codes[30] = utf8Code30;
	utf8CodeLengths[30] = 2;
	utf32Codes[31]      = 18;
	uint8 utf8Code31[]  = {0x12};
	utf8Codes[31] = utf8Code31;
	utf8CodeLengths[31] = 1;
	utf32Codes[32]      = 14382;
	uint8 utf8Code32[]  = {0xe3, 0xa0, 0xae};
	utf8Codes[32] = utf8Code32;
	utf8CodeLengths[32] = 3;
	utf32Codes[33]      = 43;
	uint8 utf8Code33[]  = {0x2b};
	utf8Codes[33] = utf8Code33;
	utf8CodeLengths[33] = 1;
	utf32Codes[34]      = 36;
	uint8 utf8Code34[]  = {0x24};
	utf8Codes[34] = utf8Code34;
	utf8CodeLengths[34] = 1;
	utf32Codes[35]      = 584214;
	uint8 utf8Code35[]  = {0xf2, 0x8e, 0xa8, 0x96};
	utf8Codes[35] = utf8Code35;
	utf8CodeLengths[35] = 4;
	utf32Codes[36]      = 108;
	uint8 utf8Code36[]  = {0x6c};
	utf8Codes[36] = utf8Code36;
	utf8CodeLengths[36] = 1;
	utf32Codes[37]      = 50716;
	uint8 utf8Code37[]  = {0xec, 0x98, 0x9c};
	utf8Codes[37] = utf8Code37;
	utf8CodeLengths[37] = 3;
	utf32Codes[38]      = 877941;
	uint8 utf8Code38[]  = {0xf3, 0x96, 0x95, 0xb5};
	utf8Codes[38] = utf8Code38;
	utf8CodeLengths[38] = 4;
	utf32Codes[39]      = 674;
	uint8 utf8Code39[]  = {0xca, 0xa2};
	utf8Codes[39] = utf8Code39;
	utf8CodeLengths[39] = 2;
	utf32Codes[40]      = 919113;
	uint8 utf8Code40[]  = {0xf3, 0xa0, 0x99, 0x89};
	utf8Codes[40] = utf8Code40;
	utf8CodeLengths[40] = 4;
	utf32Codes[41]      = 110;
	uint8 utf8Code41[]  = {0x6e};
	utf8Codes[41] = utf8Code41;
	utf8CodeLengths[41] = 1;
	utf32Codes[42]      = 1127;
	uint8 utf8Code42[]  = {0xd1, 0xa7};
	utf8Codes[42] = utf8Code42;
	utf8CodeLengths[42] = 2;
	utf32Codes[43]      = 5;
	uint8 utf8Code43[]  = {0x5};
	utf8Codes[43] = utf8Code43;
	utf8CodeLengths[43] = 1;
	utf32Codes[44]      = 444297;
	uint8 utf8Code44[]  = {0xf1, 0xac, 0x9e, 0x89};
	utf8Codes[44] = utf8Code44;
	utf8CodeLengths[44] = 4;
	utf32Codes[45]      = 124;
	uint8 utf8Code45[]  = {0x7c};
	utf8Codes[45] = utf8Code45;
	utf8CodeLengths[45] = 1;
	utf32Codes[46]      = 1375;
	uint8 utf8Code46[]  = {0xd5, 0x9f};
	utf8Codes[46] = utf8Code46;
	utf8CodeLengths[46] = 2;
	utf32Codes[47]      = 30912;
	uint8 utf8Code47[]  = {0xe7, 0xa3, 0x80};
	utf8Codes[47] = utf8Code47;
	utf8CodeLengths[47] = 3;
	utf32Codes[48]      = 23143;
	uint8 utf8Code48[]  = {0xe5, 0xa9, 0xa7};
	utf8Codes[48] = utf8Code48;
	utf8CodeLengths[48] = 3;
	utf32Codes[49]      = 2028;
	uint8 utf8Code49[]  = {0xdf, 0xac};
	utf8Codes[49] = utf8Code49;
	utf8CodeLengths[49] = 2;
	utf32Codes[50]      = 317110;
	uint8 utf8Code50[]  = {0xf1, 0x8d, 0x9a, 0xb6};
	utf8Codes[50] = utf8Code50;
	utf8CodeLengths[50] = 4;
	utf32Codes[51]      = 545681;
	uint8 utf8Code51[]  = {0xf2, 0x85, 0x8e, 0x91};
	utf8Codes[51] = utf8Code51;
	utf8CodeLengths[51] = 4;
	utf32Codes[52]      = 551586;
	uint8 utf8Code52[]  = {0xf2, 0x86, 0xaa, 0xa2};
	utf8Codes[52] = utf8Code52;
	utf8CodeLengths[52] = 4;
	utf32Codes[53]      = 96;
	uint8 utf8Code53[]  = {0x60};
	utf8Codes[53] = utf8Code53;
	utf8CodeLengths[53] = 1;
	utf32Codes[54]      = 38435;
	uint8 utf8Code54[]  = {0xe9, 0x98, 0xa3};
	utf8Codes[54] = utf8Code54;
	utf8CodeLengths[54] = 3;
	utf32Codes[55]      = 80;
	uint8 utf8Code55[]  = {0x50};
	utf8Codes[55] = utf8Code55;
	utf8CodeLengths[55] = 1;
	utf32Codes[56]      = 1064;
	uint8 utf8Code56[]  = {0xd0, 0xa8};
	utf8Codes[56] = utf8Code56;
	utf8CodeLengths[56] = 2;
	utf32Codes[57]      = 246;
	uint8 utf8Code57[]  = {0xc3, 0xb6};
	utf8Codes[57] = utf8Code57;
	utf8CodeLengths[57] = 2;
	utf32Codes[58]      = 26;
	uint8 utf8Code58[]  = {0x1a};
	utf8Codes[58] = utf8Code58;
	utf8CodeLengths[58] = 1;
	utf32Codes[59]      = 58;
	uint8 utf8Code59[]  = {0x3a};
	utf8Codes[59] = utf8Code59;
	utf8CodeLengths[59] = 1;
	utf32Codes[60]      = 60728;
	uint8 utf8Code60[]  = {0xee, 0xb4, 0xb8};
	utf8Codes[60] = utf8Code60;
	utf8CodeLengths[60] = 3;
	utf32Codes[61]      = 2044;
	uint8 utf8Code61[]  = {0xdf, 0xbc};
	utf8Codes[61] = utf8Code61;
	utf8CodeLengths[61] = 2;
	utf32Codes[62]      = 295419;
	uint8 utf8Code62[]  = {0xf1, 0x88, 0x87, 0xbb};
	utf8Codes[62] = utf8Code62;
	utf8CodeLengths[62] = 4;
	utf32Codes[63]      = 18040;
	uint8 utf8Code63[]  = {0xe4, 0x99, 0xb8};
	utf8Codes[63] = utf8Code63;
	utf8CodeLengths[63] = 3;
	utf32Codes[64]      = 61505;
	uint8 utf8Code64[]  = {0xef, 0x81, 0x81};
	utf8Codes[64] = utf8Code64;
	utf8CodeLengths[64] = 3;
	utf32Codes[65]      = 43239;
	uint8 utf8Code65[]  = {0xea, 0xa3, 0xa7};
	utf8Codes[65] = utf8Code65;
	utf8CodeLengths[65] = 3;
	utf32Codes[66]      = 12;
	uint8 utf8Code66[]  = {0xc};
	utf8Codes[66] = utf8Code66;
	utf8CodeLengths[66] = 1;
	utf32Codes[67]      = 7541;
	uint8 utf8Code67[]  = {0xe1, 0xb5, 0xb5};
	utf8Codes[67] = utf8Code67;
	utf8CodeLengths[67] = 3;
	utf32Codes[68]      = 42509;
	uint8 utf8Code68[]  = {0xea, 0x98, 0x8d};
	utf8Codes[68] = utf8Code68;
	utf8CodeLengths[68] = 3;
	utf32Codes[69]      = 192009;
	uint8 utf8Code69[]  = {0xf0, 0xae, 0xb8, 0x89};
	utf8Codes[69] = utf8Code69;
	utf8CodeLengths[69] = 4;
	utf32Codes[70]      = 1761;
	uint8 utf8Code70[]  = {0xdb, 0xa1};
	utf8Codes[70] = utf8Code70;
	utf8CodeLengths[70] = 2;
	utf32Codes[71]      = 40264;
	uint8 utf8Code71[]  = {0xe9, 0xb5, 0x88};
	utf8Codes[71] = utf8Code71;
	utf8CodeLengths[71] = 3;
	utf32Codes[72]      = 522;
	uint8 utf8Code72[]  = {0xc8, 0x8a};
	utf8Codes[72] = utf8Code72;
	utf8CodeLengths[72] = 2;
	utf32Codes[73]      = 30709;
	uint8 utf8Code73[]  = {0xe7, 0x9f, 0xb5};
	utf8Codes[73] = utf8Code73;
	utf8CodeLengths[73] = 3;
	utf32Codes[74]      = 219696;
	uint8 utf8Code74[]  = {0xf0, 0xb5, 0xa8, 0xb0};
	utf8Codes[74] = utf8Code74;
	utf8CodeLengths[74] = 4;
	utf32Codes[75]      = 1682;
	uint8 utf8Code75[]  = {0xda, 0x92};
	utf8Codes[75] = utf8Code75;
	utf8CodeLengths[75] = 2;
	utf32Codes[76]      = 192411;
	uint8 utf8Code76[]  = {0xf0, 0xae, 0xbe, 0x9b};
	utf8Codes[76] = utf8Code76;
	utf8CodeLengths[76] = 4;
	utf32Codes[77]      = 36;
	uint8 utf8Code77[]  = {0x24};
	utf8Codes[77] = utf8Code77;
	utf8CodeLengths[77] = 1;
	utf32Codes[78]      = 11910;
	uint8 utf8Code78[]  = {0xe2, 0xba, 0x86};
	utf8Codes[78] = utf8Code78;
	utf8CodeLengths[78] = 3;
	utf32Codes[79]      = 285828;
	uint8 utf8Code79[]  = {0xf1, 0x85, 0xb2, 0x84};
	utf8Codes[79] = utf8Code79;
	utf8CodeLengths[79] = 4;
	utf32Codes[80]      = 699377;
	uint8 utf8Code80[]  = {0xf2, 0xaa, 0xaf, 0xb1};
	utf8Codes[80] = utf8Code80;
	utf8CodeLengths[80] = 4;
	utf32Codes[81]      = 14551;
	uint8 utf8Code81[]  = {0xe3, 0xa3, 0x97};
	utf8Codes[81] = utf8Code81;
	utf8CodeLengths[81] = 3;
	utf32Codes[82]      = 50;
	uint8 utf8Code82[]  = {0x32};
	utf8Codes[82] = utf8Code82;
	utf8CodeLengths[82] = 1;
	utf32Codes[83]      = 100;
	uint8 utf8Code83[]  = {0x64};
	utf8Codes[83] = utf8Code83;
	utf8CodeLengths[83] = 1;
	utf32Codes[84]      = 91;
	uint8 utf8Code84[]  = {0x5b};
	utf8Codes[84] = utf8Code84;
	utf8CodeLengths[84] = 1;
	utf32Codes[85]      = 53;
	uint8 utf8Code85[]  = {0x35};
	utf8Codes[85] = utf8Code85;
	utf8CodeLengths[85] = 1;
	utf32Codes[86]      = 1002;
	uint8 utf8Code86[]  = {0xcf, 0xaa};
	utf8Codes[86] = utf8Code86;
	utf8CodeLengths[86] = 2;
	utf32Codes[87]      = 933391;
	uint8 utf8Code87[]  = {0xf3, 0xa3, 0xb8, 0x8f};
	utf8Codes[87] = utf8Code87;
	utf8CodeLengths[87] = 4;
	utf32Codes[88]      = 54;
	uint8 utf8Code88[]  = {0x36};
	utf8Codes[88] = utf8Code88;
	utf8CodeLengths[88] = 1;
	utf32Codes[89]      = 833107;
	uint8 utf8Code89[]  = {0xf3, 0x8b, 0x99, 0x93};
	utf8Codes[89] = utf8Code89;
	utf8CodeLengths[89] = 4;
	utf32Codes[90]      = 37070;
	uint8 utf8Code90[]  = {0xe9, 0x83, 0x8e};
	utf8Codes[90] = utf8Code90;
	utf8CodeLengths[90] = 3;
	utf32Codes[91]      = 1589;
	uint8 utf8Code91[]  = {0xd8, 0xb5};
	utf8Codes[91] = utf8Code91;
	utf8CodeLengths[91] = 2;
	utf32Codes[92]      = 1469;
	uint8 utf8Code92[]  = {0xd6, 0xbd};
	utf8Codes[92] = utf8Code92;
	utf8CodeLengths[92] = 2;
	utf32Codes[93]      = 27100;
	uint8 utf8Code93[]  = {0xe6, 0xa7, 0x9c};
	utf8Codes[93] = utf8Code93;
	utf8CodeLengths[93] = 3;
	utf32Codes[94]      = 485267;
	uint8 utf8Code94[]  = {0xf1, 0xb6, 0x9e, 0x93};
	utf8Codes[94] = utf8Code94;
	utf8CodeLengths[94] = 4;
	utf32Codes[95]      = 1993;
	uint8 utf8Code95[]  = {0xdf, 0x89};
	utf8Codes[95] = utf8Code95;
	utf8CodeLengths[95] = 2;
	utf32Codes[96]      = 1225;
	uint8 utf8Code96[]  = {0xd3, 0x89};
	utf8Codes[96] = utf8Code96;
	utf8CodeLengths[96] = 2;
	utf32Codes[97]      = 594112;
	uint8 utf8Code97[]  = {0xf2, 0x91, 0x83, 0x80};
	utf8Codes[97] = utf8Code97;
	utf8CodeLengths[97] = 4;
	utf32Codes[98]      = 45633;
	uint8 utf8Code98[]  = {0xeb, 0x89, 0x81};
	utf8Codes[98] = utf8Code98;
	utf8CodeLengths[98] = 3;
	utf32Codes[99]      = 1088216;
	uint8 utf8Code99[]  = {0xf4, 0x89, 0xab, 0x98};
	utf8Codes[99] = utf8Code99;
	utf8CodeLengths[99] = 4;
	utf32Codes[100]      = 822;
	uint8 utf8Code100[]  = {0xcc, 0xb6};
	utf8Codes[100] = utf8Code100;
	utf8CodeLengths[100] = 2;
	utf32Codes[101]      = 28838;
	uint8 utf8Code101[]  = {0xe7, 0x82, 0xa6};
	utf8Codes[101] = utf8Code101;
	utf8CodeLengths[101] = 3;
	utf32Codes[102]      = 1303;
	uint8 utf8Code102[]  = {0xd4, 0x97};
	utf8Codes[102] = utf8Code102;
	utf8CodeLengths[102] = 2;
	utf32Codes[103]      = 44561;
	uint8 utf8Code103[]  = {0xea, 0xb8, 0x91};
	utf8Codes[103] = utf8Code103;
	utf8CodeLengths[103] = 3;
	utf32Codes[104]      = 24707;
	uint8 utf8Code104[]  = {0xe6, 0x82, 0x83};
	utf8Codes[104] = utf8Code104;
	utf8CodeLengths[104] = 3;
	utf32Codes[105]      = 203495;
	uint8 utf8Code105[]  = {0xf0, 0xb1, 0xab, 0xa7};
	utf8Codes[105] = utf8Code105;
	utf8CodeLengths[105] = 4;
	utf32Codes[106]      = 37306;
	uint8 utf8Code106[]  = {0xe9, 0x86, 0xba};
	utf8Codes[106] = utf8Code106;
	utf8CodeLengths[106] = 3;
	utf32Codes[107]      = 587956;
	uint8 utf8Code107[]  = {0xf2, 0x8f, 0xa2, 0xb4};
	utf8Codes[107] = utf8Code107;
	utf8CodeLengths[107] = 4;
	utf32Codes[108]      = 911274;
	uint8 utf8Code108[]  = {0xf3, 0x9e, 0x9e, 0xaa};
	utf8Codes[108] = utf8Code108;
	utf8CodeLengths[108] = 4;
	utf32Codes[109]      = 9370;
	uint8 utf8Code109[]  = {0xe2, 0x92, 0x9a};
	utf8Codes[109] = utf8Code109;
	utf8CodeLengths[109] = 3;
	utf32Codes[110]      = 563640;
	uint8 utf8Code110[]  = {0xf2, 0x89, 0xa6, 0xb8};
	utf8Codes[110] = utf8Code110;
	utf8CodeLengths[110] = 4;
	utf32Codes[111]      = 5;
	uint8 utf8Code111[]  = {0x5};
	utf8Codes[111] = utf8Code111;
	utf8CodeLengths[111] = 1;
	utf32Codes[112]      = 24;
	uint8 utf8Code112[]  = {0x18};
	utf8Codes[112] = utf8Code112;
	utf8CodeLengths[112] = 1;
	utf32Codes[113]      = 4804;
	uint8 utf8Code113[]  = {0xe1, 0x8b, 0x84};
	utf8Codes[113] = utf8Code113;
	utf8CodeLengths[113] = 3;
	utf32Codes[114]      = 3168;
	uint8 utf8Code114[]  = {0xe0, 0xb1, 0xa0};
	utf8Codes[114] = utf8Code114;
	utf8CodeLengths[114] = 3;
	utf32Codes[115]      = 410;
	uint8 utf8Code115[]  = {0xc6, 0x9a};
	utf8Codes[115] = utf8Code115;
	utf8CodeLengths[115] = 2;
	utf32Codes[116]      = 109;
	uint8 utf8Code116[]  = {0x6d};
	utf8Codes[116] = utf8Code116;
	utf8CodeLengths[116] = 1;
	utf32Codes[117]      = 335795;
	uint8 utf8Code117[]  = {0xf1, 0x91, 0xbe, 0xb3};
	utf8Codes[117] = utf8Code117;
	utf8CodeLengths[117] = 4;
	utf32Codes[118]      = 16688;
	uint8 utf8Code118[]  = {0xe4, 0x84, 0xb0};
	utf8Codes[118] = utf8Code118;
	utf8CodeLengths[118] = 3;
	utf32Codes[119]      = 837;
	uint8 utf8Code119[]  = {0xcd, 0x85};
	utf8Codes[119] = utf8Code119;
	utf8CodeLengths[119] = 2;
	utf32Codes[120]      = 744828;
	uint8 utf8Code120[]  = {0xf2, 0xb5, 0xb5, 0xbc};
	utf8Codes[120] = utf8Code120;
	utf8CodeLengths[120] = 4;
	utf32Codes[121]      = 757993;
	uint8 utf8Code121[]  = {0xf2, 0xb9, 0x83, 0xa9};
	utf8Codes[121] = utf8Code121;
	utf8CodeLengths[121] = 4;
	utf32Codes[122]      = 757;
	uint8 utf8Code122[]  = {0xcb, 0xb5};
	utf8Codes[122] = utf8Code122;
	utf8CodeLengths[122] = 2;
	utf32Codes[123]      = 167194;
	uint8 utf8Code123[]  = {0xf0, 0xa8, 0xb4, 0x9a};
	utf8Codes[123] = utf8Code123;
	utf8CodeLengths[123] = 4;
	utf32Codes[124]      = 281901;
	uint8 utf8Code124[]  = {0xf1, 0x84, 0xb4, 0xad};
	utf8Codes[124] = utf8Code124;
	utf8CodeLengths[124] = 4;
	utf32Codes[125]      = 29028;
	uint8 utf8Code125[]  = {0xe7, 0x85, 0xa4};
	utf8Codes[125] = utf8Code125;
	utf8CodeLengths[125] = 3;
	utf32Codes[126]      = 504;
	uint8 utf8Code126[]  = {0xc7, 0xb8};
	utf8Codes[126] = utf8Code126;
	utf8CodeLengths[126] = 2;
	utf32Codes[127]      = 1243;
	uint8 utf8Code127[]  = {0xd3, 0x9b};
	utf8Codes[127] = utf8Code127;
	utf8CodeLengths[127] = 2;
	utf32Codes[128]      = 26614;
	uint8 utf8Code128[]  = {0xe6, 0x9f, 0xb6};
	utf8Codes[128] = utf8Code128;
	utf8CodeLengths[128] = 3;
	utf32Codes[129]      = 386783;
	uint8 utf8Code129[]  = {0xf1, 0x9e, 0x9b, 0x9f};
	utf8Codes[129] = utf8Code129;
	utf8CodeLengths[129] = 4;
	utf32Codes[130]      = 1557;
	uint8 utf8Code130[]  = {0xd8, 0x95};
	utf8Codes[130] = utf8Code130;
	utf8CodeLengths[130] = 2;
	utf32Codes[131]      = 43878;
	uint8 utf8Code131[]  = {0xea, 0xad, 0xa6};
	utf8Codes[131] = utf8Code131;
	utf8CodeLengths[131] = 3;
	utf32Codes[132]      = 1329;
	uint8 utf8Code132[]  = {0xd4, 0xb1};
	utf8Codes[132] = utf8Code132;
	utf8CodeLengths[132] = 2;
	utf32Codes[133]      = 8266;
	uint8 utf8Code133[]  = {0xe2, 0x81, 0x8a};
	utf8Codes[133] = utf8Code133;
	utf8CodeLengths[133] = 3;
	utf32Codes[134]      = 1213;
	uint8 utf8Code134[]  = {0xd2, 0xbd};
	utf8Codes[134] = utf8Code134;
	utf8CodeLengths[134] = 2;
	utf32Codes[135]      = 654382;
	uint8 utf8Code135[]  = {0xf2, 0x9f, 0xb0, 0xae};
	utf8Codes[135] = utf8Code135;
	utf8CodeLengths[135] = 4;
	utf32Codes[136]      = 36;
	uint8 utf8Code136[]  = {0x24};
	utf8Codes[136] = utf8Code136;
	utf8CodeLengths[136] = 1;
	utf32Codes[137]      = 1521;
	uint8 utf8Code137[]  = {0xd7, 0xb1};
	utf8Codes[137] = utf8Code137;
	utf8CodeLengths[137] = 2;
	utf32Codes[138]      = 49631;
	uint8 utf8Code138[]  = {0xec, 0x87, 0x9f};
	utf8Codes[138] = utf8Code138;
	utf8CodeLengths[138] = 3;
	utf32Codes[139]      = 944;
	uint8 utf8Code139[]  = {0xce, 0xb0};
	utf8Codes[139] = utf8Code139;
	utf8CodeLengths[139] = 2;
	utf32Codes[140]      = 576;
	uint8 utf8Code140[]  = {0xc9, 0x80};
	utf8Codes[140] = utf8Code140;
	utf8CodeLengths[140] = 2;
	utf32Codes[141]      = 30261;
	uint8 utf8Code141[]  = {0xe7, 0x98, 0xb5};
	utf8Codes[141] = utf8Code141;
	utf8CodeLengths[141] = 3;
	utf32Codes[142]      = 1252;
	uint8 utf8Code142[]  = {0xd3, 0xa4};
	utf8Codes[142] = utf8Code142;
	utf8CodeLengths[142] = 2;
	utf32Codes[143]      = 37851;
	uint8 utf8Code143[]  = {0xe9, 0x8f, 0x9b};
	utf8Codes[143] = utf8Code143;
	utf8CodeLengths[143] = 3;
	utf32Codes[144]      = 68;
	uint8 utf8Code144[]  = {0x44};
	utf8Codes[144] = utf8Code144;
	utf8CodeLengths[144] = 1;
	utf32Codes[145]      = 1654;
	uint8 utf8Code145[]  = {0xd9, 0xb6};
	utf8Codes[145] = utf8Code145;
	utf8CodeLengths[145] = 2;
	utf32Codes[146]      = 984;
	uint8 utf8Code146[]  = {0xcf, 0x98};
	utf8Codes[146] = utf8Code146;
	utf8CodeLengths[146] = 2;
	utf32Codes[147]      = 1181;
	uint8 utf8Code147[]  = {0xd2, 0x9d};
	utf8Codes[147] = utf8Code147;
	utf8CodeLengths[147] = 2;
	utf32Codes[148]      = 71;
	uint8 utf8Code148[]  = {0x47};
	utf8Codes[148] = utf8Code148;
	utf8CodeLengths[148] = 1;
	utf32Codes[149]      = 13041;
	uint8 utf8Code149[]  = {0xe3, 0x8b, 0xb1};
	utf8Codes[149] = utf8Code149;
	utf8CodeLengths[149] = 3;
	utf32Codes[150]      = 102;
	uint8 utf8Code150[]  = {0x66};
	utf8Codes[150] = utf8Code150;
	utf8CodeLengths[150] = 1;
	utf32Codes[151]      = 670157;
	uint8 utf8Code151[]  = {0xf2, 0xa3, 0xa7, 0x8d};
	utf8Codes[151] = utf8Code151;
	utf8CodeLengths[151] = 4;
	utf32Codes[152]      = 23678;
	uint8 utf8Code152[]  = {0xe5, 0xb1, 0xbe};
	utf8Codes[152] = utf8Code152;
	utf8CodeLengths[152] = 3;
	utf32Codes[153]      = 26326;
	uint8 utf8Code153[]  = {0xe6, 0x9b, 0x96};
	utf8Codes[153] = utf8Code153;
	utf8CodeLengths[153] = 3;
	utf32Codes[154]      = 566035;
	uint8 utf8Code154[]  = {0xf2, 0x8a, 0x8c, 0x93};
	utf8Codes[154] = utf8Code154;
	utf8CodeLengths[154] = 4;
	utf32Codes[155]      = 808;
	uint8 utf8Code155[]  = {0xcc, 0xa8};
	utf8Codes[155] = utf8Code155;
	utf8CodeLengths[155] = 2;
	utf32Codes[156]      = 3151;
	uint8 utf8Code156[]  = {0xe0, 0xb1, 0x8f};
	utf8Codes[156] = utf8Code156;
	utf8CodeLengths[156] = 3;
	utf32Codes[157]      = 47;
	uint8 utf8Code157[]  = {0x2f};
	utf8Codes[157] = utf8Code157;
	utf8CodeLengths[157] = 1;
	utf32Codes[158]      = 97;
	uint8 utf8Code158[]  = {0x61};
	utf8Codes[158] = utf8Code158;
	utf8CodeLengths[158] = 1;
	utf32Codes[159]      = 58886;
	uint8 utf8Code159[]  = {0xee, 0x98, 0x86};
	utf8Codes[159] = utf8Code159;
	utf8CodeLengths[159] = 3;
	utf32Codes[160]      = 35882;
	uint8 utf8Code160[]  = {0xe8, 0xb0, 0xaa};
	utf8Codes[160] = utf8Code160;
	utf8CodeLengths[160] = 3;
	utf32Codes[161]      = 321501;
	uint8 utf8Code161[]  = {0xf1, 0x8e, 0x9f, 0x9d};
	utf8Codes[161] = utf8Code161;
	utf8CodeLengths[161] = 4;
	utf32Codes[162]      = 700;
	uint8 utf8Code162[]  = {0xca, 0xbc};
	utf8Codes[162] = utf8Code162;
	utf8CodeLengths[162] = 2;
	utf32Codes[163]      = 71526;
	uint8 utf8Code163[]  = {0xf0, 0x91, 0x9d, 0xa6};
	utf8Codes[163] = utf8Code163;
	utf8CodeLengths[163] = 4;
	utf32Codes[164]      = 95;
	uint8 utf8Code164[]  = {0x5f};
	utf8Codes[164] = utf8Code164;
	utf8CodeLengths[164] = 1;
	utf32Codes[165]      = 8472;
	uint8 utf8Code165[]  = {0xe2, 0x84, 0x98};
	utf8Codes[165] = utf8Code165;
	utf8CodeLengths[165] = 3;
	utf32Codes[166]      = 1520;
	uint8 utf8Code166[]  = {0xd7, 0xb0};
	utf8Codes[166] = utf8Code166;
	utf8CodeLengths[166] = 2;
	utf32Codes[167]      = 116;
	uint8 utf8Code167[]  = {0x74};
	utf8Codes[167] = utf8Code167;
	utf8CodeLengths[167] = 1;
	utf32Codes[168]      = 6898;
	uint8 utf8Code168[]  = {0xe1, 0xab, 0xb2};
	utf8Codes[168] = utf8Code168;
	utf8CodeLengths[168] = 3;
	utf32Codes[169]      = 105;
	uint8 utf8Code169[]  = {0x69};
	utf8Codes[169] = utf8Code169;
	utf8CodeLengths[169] = 1;
	utf32Codes[170]      = 727486;
	uint8 utf8Code170[]  = {0xf2, 0xb1, 0xa6, 0xbe};
	utf8Codes[170] = utf8Code170;
	utf8CodeLengths[170] = 4;
	utf32Codes[171]      = 49615;
	uint8 utf8Code171[]  = {0xec, 0x87, 0x8f};
	utf8Codes[171] = utf8Code171;
	utf8CodeLengths[171] = 3;
	utf32Codes[172]      = 650149;
	uint8 utf8Code172[]  = {0xf2, 0x9e, 0xae, 0xa5};
	utf8Codes[172] = utf8Code172;
	utf8CodeLengths[172] = 4;
	utf32Codes[173]      = 43632;
	uint8 utf8Code173[]  = {0xea, 0xa9, 0xb0};
	utf8Codes[173] = utf8Code173;
	utf8CodeLengths[173] = 3;
	utf32Codes[174]      = 756503;
	uint8 utf8Code174[]  = {0xf2, 0xb8, 0xac, 0x97};
	utf8Codes[174] = utf8Code174;
	utf8CodeLengths[174] = 4;
	utf32Codes[175]      = 1430;
	uint8 utf8Code175[]  = {0xd6, 0x96};
	utf8Codes[175] = utf8Code175;
	utf8CodeLengths[175] = 2;
	utf32Codes[176]      = 627244;
	uint8 utf8Code176[]  = {0xf2, 0x99, 0x88, 0xac};
	utf8Codes[176] = utf8Code176;
	utf8CodeLengths[176] = 4;
	utf32Codes[177]      = 46677;
	uint8 utf8Code177[]  = {0xeb, 0x99, 0x95};
	utf8Codes[177] = utf8Code177;
	utf8CodeLengths[177] = 3;
	utf32Codes[178]      = 140226;
	uint8 utf8Code178[]  = {0xf0, 0xa2, 0x8f, 0x82};
	utf8Codes[178] = utf8Code178;
	utf8CodeLengths[178] = 4;
	utf32Codes[179]      = 27188;
	uint8 utf8Code179[]  = {0xe6, 0xa8, 0xb4};
	utf8Codes[179] = utf8Code179;
	utf8CodeLengths[179] = 3;
	utf32Codes[180]      = 396823;
	uint8 utf8Code180[]  = {0xf1, 0xa0, 0xb8, 0x97};
	utf8Codes[180] = utf8Code180;
	utf8CodeLengths[180] = 4;
	utf32Codes[181]      = 1029;
	uint8 utf8Code181[]  = {0xd0, 0x85};
	utf8Codes[181] = utf8Code181;
	utf8CodeLengths[181] = 2;
	utf32Codes[182]      = 116704;
	uint8 utf8Code182[]  = {0xf0, 0x9c, 0x9f, 0xa0};
	utf8Codes[182] = utf8Code182;
	utf8CodeLengths[182] = 4;
	utf32Codes[183]      = 1693;
	uint8 utf8Code183[]  = {0xda, 0x9d};
	utf8Codes[183] = utf8Code183;
	utf8CodeLengths[183] = 2;
	utf32Codes[184]      = 153;
	uint8 utf8Code184[]  = {0xc2, 0x99};
	utf8Codes[184] = utf8Code184;
	utf8CodeLengths[184] = 2;
	utf32Codes[185]      = 579344;
	uint8 utf8Code185[]  = {0xf2, 0x8d, 0x9c, 0x90};
	utf8Codes[185] = utf8Code185;
	utf8CodeLengths[185] = 4;
	utf32Codes[186]      = 42;
	uint8 utf8Code186[]  = {0x2a};
	utf8Codes[186] = utf8Code186;
	utf8CodeLengths[186] = 1;
	utf32Codes[187]      = 47762;
	uint8 utf8Code187[]  = {0xeb, 0xaa, 0x92};
	utf8Codes[187] = utf8Code187;
	utf8CodeLengths[187] = 3;
	utf32Codes[188]      = 52516;
	uint8 utf8Code188[]  = {0xec, 0xb4, 0xa4};
	utf8Codes[188] = utf8Code188;
	utf8CodeLengths[188] = 3;
	utf32Codes[189]      = 1105842;
	uint8 utf8Code189[]  = {0xf4, 0x8d, 0xbe, 0xb2};
	utf8Codes[189] = utf8Code189;
	utf8CodeLengths[189] = 4;
	utf32Codes[190]      = 327;
	uint8 utf8Code190[]  = {0xc5, 0x87};
	utf8Codes[190] = utf8Code190;
	utf8CodeLengths[190] = 2;
	utf32Codes[191]      = 1353;
	uint8 utf8Code191[]  = {0xd5, 0x89};
	utf8Codes[191] = utf8Code191;
	utf8CodeLengths[191] = 2;
	utf32Codes[192]      = 60;
	uint8 utf8Code192[]  = {0x3c};
	utf8Codes[192] = utf8Code192;
	utf8CodeLengths[192] = 1;
	utf32Codes[193]      = 38;
	uint8 utf8Code193[]  = {0x26};
	utf8Codes[193] = utf8Code193;
	utf8CodeLengths[193] = 1;
	utf32Codes[194]      = 58574;
	uint8 utf8Code194[]  = {0xee, 0x93, 0x8e};
	utf8Codes[194] = utf8Code194;
	utf8CodeLengths[194] = 3;
	utf32Codes[195]      = 497970;
	uint8 utf8Code195[]  = {0xf1, 0xb9, 0xa4, 0xb2};
	utf8Codes[195] = utf8Code195;
	utf8CodeLengths[195] = 4;
	utf32Codes[196]      = 1313;
	uint8 utf8Code196[]  = {0xd4, 0xa1};
	utf8Codes[196] = utf8Code196;
	utf8CodeLengths[196] = 2;
	utf32Codes[197]      = 1337;
	uint8 utf8Code197[]  = {0xd4, 0xb9};
	utf8Codes[197] = utf8Code197;
	utf8CodeLengths[197] = 2;
	utf32Codes[198]      = 26;
	uint8 utf8Code198[]  = {0x1a};
	utf8Codes[198] = utf8Code198;
	utf8CodeLengths[198] = 1;
	utf32Codes[199]      = 1382;
	uint8 utf8Code199[]  = {0xd5, 0xa6};
	utf8Codes[199] = utf8Code199;
	utf8CodeLengths[199] = 2;
	utf32Codes[200]      = 64351;
	uint8 utf8Code200[]  = {0xef, 0xad, 0x9f};
	utf8Codes[200] = utf8Code200;
	utf8CodeLengths[200] = 3;
	utf32Codes[201]      = 1176;
	uint8 utf8Code201[]  = {0xd2, 0x98};
	utf8Codes[201] = utf8Code201;
	utf8CodeLengths[201] = 2;
	utf32Codes[202]      = 1056;
	uint8 utf8Code202[]  = {0xd0, 0xa0};
	utf8Codes[202] = utf8Code202;
	utf8CodeLengths[202] = 2;
	utf32Codes[203]      = 11;
	uint8 utf8Code203[]  = {0xb};
	utf8Codes[203] = utf8Code203;
	utf8CodeLengths[203] = 1;
	utf32Codes[204]      = 703146;
	uint8 utf8Code204[]  = {0xf2, 0xab, 0xaa, 0xaa};
	utf8Codes[204] = utf8Code204;
	utf8CodeLengths[204] = 4;
	utf32Codes[205]      = 1282;
	uint8 utf8Code205[]  = {0xd4, 0x82};
	utf8Codes[205] = utf8Code205;
	utf8CodeLengths[205] = 2;
	utf32Codes[206]      = 32;
	uint8 utf8Code206[]  = {0x20};
	utf8Codes[206] = utf8Code206;
	utf8CodeLengths[206] = 1;
	utf32Codes[207]      = 758509;
	uint8 utf8Code207[]  = {0xf2, 0xb9, 0x8b, 0xad};
	utf8Codes[207] = utf8Code207;
	utf8CodeLengths[207] = 4;
	utf32Codes[208]      = 753;
	uint8 utf8Code208[]  = {0xcb, 0xb1};
	utf8Codes[208] = utf8Code208;
	utf8CodeLengths[208] = 2;
	utf32Codes[209]      = 13572;
	uint8 utf8Code209[]  = {0xe3, 0x94, 0x84};
	utf8Codes[209] = utf8Code209;
	utf8CodeLengths[209] = 3;
	utf32Codes[210]      = 1043;
	uint8 utf8Code210[]  = {0xd0, 0x93};
	utf8Codes[210] = utf8Code210;
	utf8CodeLengths[210] = 2;
	utf32Codes[211]      = 1550;
	uint8 utf8Code211[]  = {0xd8, 0x8e};
	utf8Codes[211] = utf8Code211;
	utf8CodeLengths[211] = 2;
	utf32Codes[212]      = 1052327;
	uint8 utf8Code212[]  = {0xf4, 0x80, 0xba, 0xa7};
	utf8Codes[212] = utf8Code212;
	utf8CodeLengths[212] = 4;
	utf32Codes[213]      = 1834;
	uint8 utf8Code213[]  = {0xdc, 0xaa};
	utf8Codes[213] = utf8Code213;
	utf8CodeLengths[213] = 2;
	utf32Codes[214]      = 1084286;
	uint8 utf8Code214[]  = {0xf4, 0x88, 0xad, 0xbe};
	utf8Codes[214] = utf8Code214;
	utf8CodeLengths[214] = 4;
	utf32Codes[215]      = 53;
	uint8 utf8Code215[]  = {0x35};
	utf8Codes[215] = utf8Code215;
	utf8CodeLengths[215] = 1;
	utf32Codes[216]      = 1573;
	uint8 utf8Code216[]  = {0xd8, 0xa5};
	utf8Codes[216] = utf8Code216;
	utf8CodeLengths[216] = 2;
	utf32Codes[217]      = 125;
	uint8 utf8Code217[]  = {0x7d};
	utf8Codes[217] = utf8Code217;
	utf8CodeLengths[217] = 1;
	utf32Codes[218]      = 374;
	uint8 utf8Code218[]  = {0xc5, 0xb6};
	utf8Codes[218] = utf8Code218;
	utf8CodeLengths[218] = 2;
	utf32Codes[219]      = 52926;
	uint8 utf8Code219[]  = {0xec, 0xba, 0xbe};
	utf8Codes[219] = utf8Code219;
	utf8CodeLengths[219] = 3;
	utf32Codes[220]      = 34547;
	uint8 utf8Code220[]  = {0xe8, 0x9b, 0xb3};
	utf8Codes[220] = utf8Code220;
	utf8CodeLengths[220] = 3;
	utf32Codes[221]      = 116;
	uint8 utf8Code221[]  = {0x74};
	utf8Codes[221] = utf8Code221;
	utf8CodeLengths[221] = 1;
	utf32Codes[222]      = 64540;
	uint8 utf8Code222[]  = {0xef, 0xb0, 0x9c};
	utf8Codes[222] = utf8Code222;
	utf8CodeLengths[222] = 3;
	utf32Codes[223]      = 540469;
	uint8 utf8Code223[]  = {0xf2, 0x83, 0xbc, 0xb5};
	utf8Codes[223] = utf8Code223;
	utf8CodeLengths[223] = 4;
	utf32Codes[224]      = 1101296;
	uint8 utf8Code224[]  = {0xf4, 0x8c, 0xb7, 0xb0};
	utf8Codes[224] = utf8Code224;
	utf8CodeLengths[224] = 4;
	utf32Codes[225]      = 78773;
	uint8 utf8Code225[]  = {0xf0, 0x93, 0x8e, 0xb5};
	utf8Codes[225] = utf8Code225;
	utf8CodeLengths[225] = 4;
	utf32Codes[226]      = 881022;
	uint8 utf8Code226[]  = {0xf3, 0x97, 0x85, 0xbe};
	utf8Codes[226] = utf8Code226;
	utf8CodeLengths[226] = 4;
	utf32Codes[227]      = 1427;
	uint8 utf8Code227[]  = {0xd6, 0x93};
	utf8Codes[227] = utf8Code227;
	utf8CodeLengths[227] = 2;
	utf32Codes[228]      = 69;
	uint8 utf8Code228[]  = {0x45};
	utf8Codes[228] = utf8Code228;
	utf8CodeLengths[228] = 1;
	utf32Codes[229]      = 21290;
	uint8 utf8Code229[]  = {0xe5, 0x8c, 0xaa};
	utf8Codes[229] = utf8Code229;
	utf8CodeLengths[229] = 3;
	utf32Codes[230]      = 108;
	uint8 utf8Code230[]  = {0x6c};
	utf8Codes[230] = utf8Code230;
	utf8CodeLengths[230] = 1;
	utf32Codes[231]      = 1237;
	uint8 utf8Code231[]  = {0xd3, 0x95};
	utf8Codes[231] = utf8Code231;
	utf8CodeLengths[231] = 2;
	utf32Codes[232]      = 1023777;
	uint8 utf8Code232[]  = {0xf3, 0xb9, 0xbc, 0xa1};
	utf8Codes[232] = utf8Code232;
	utf8CodeLengths[232] = 4;
	utf32Codes[233]      = 656273;
	uint8 utf8Code233[]  = {0xf2, 0xa0, 0x8e, 0x91};
	utf8Codes[233] = utf8Code233;
	utf8CodeLengths[233] = 4;
	utf32Codes[234]      = 105;
	uint8 utf8Code234[]  = {0x69};
	utf8Codes[234] = utf8Code234;
	utf8CodeLengths[234] = 1;
	utf32Codes[235]      = 64197;
	uint8 utf8Code235[]  = {0xef, 0xab, 0x85};
	utf8Codes[235] = utf8Code235;
	utf8CodeLengths[235] = 3;
	utf32Codes[236]      = 58;
	uint8 utf8Code236[]  = {0x3a};
	utf8Codes[236] = utf8Code236;
	utf8CodeLengths[236] = 1;
	utf32Codes[237]      = 49208;
	uint8 utf8Code237[]  = {0xec, 0x80, 0xb8};
	utf8Codes[237] = utf8Code237;
	utf8CodeLengths[237] = 3;
	utf32Codes[238]      = 20681;
	uint8 utf8Code238[]  = {0xe5, 0x83, 0x89};
	utf8Codes[238] = utf8Code238;
	utf8CodeLengths[238] = 3;
	utf32Codes[239]      = 1904;
	uint8 utf8Code239[]  = {0xdd, 0xb0};
	utf8Codes[239] = utf8Code239;
	utf8CodeLengths[239] = 2;
	utf32Codes[240]      = 1087445;
	uint8 utf8Code240[]  = {0xf4, 0x89, 0x9f, 0x95};
	utf8Codes[240] = utf8Code240;
	utf8CodeLengths[240] = 4;
	utf32Codes[241]      = 54778;
	uint8 utf8Code241[]  = {0xed, 0x97, 0xba};
	utf8Codes[241] = utf8Code241;
	utf8CodeLengths[241] = 3;
	utf32Codes[242]      = 1013427;
	uint8 utf8Code242[]  = {0xf3, 0xb7, 0x9a, 0xb3};
	utf8Codes[242] = utf8Code242;
	utf8CodeLengths[242] = 4;
	utf32Codes[243]      = 58;
	uint8 utf8Code243[]  = {0x3a};
	utf8Codes[243] = utf8Code243;
	utf8CodeLengths[243] = 1;
	utf32Codes[244]      = 99915;
	uint8 utf8Code244[]  = {0xf0, 0x98, 0x99, 0x8b};
	utf8Codes[244] = utf8Code244;
	utf8CodeLengths[244] = 4;
	utf32Codes[245]      = 805;
	uint8 utf8Code245[]  = {0xcc, 0xa5};
	utf8Codes[245] = utf8Code245;
	utf8CodeLengths[245] = 2;
	utf32Codes[246]      = 13523;
	uint8 utf8Code246[]  = {0xe3, 0x93, 0x93};
	utf8Codes[246] = utf8Code246;
	utf8CodeLengths[246] = 3;
	utf32Codes[247]      = 52;
	uint8 utf8Code247[]  = {0x34};
	utf8Codes[247] = utf8Code247;
	utf8CodeLengths[247] = 1;
	utf32Codes[248]      = 1304;
	uint8 utf8Code248[]  = {0xd4, 0x98};
	utf8Codes[248] = utf8Code248;
	utf8CodeLengths[248] = 2;
	utf32Codes[249]      = 1746;
	uint8 utf8Code249[]  = {0xdb, 0x92};
	utf8Codes[249] = utf8Code249;
	utf8CodeLengths[249] = 2;
	utf32Codes[250]      = 100;
	uint8 utf8Code250[]  = {0x64};
	utf8Codes[250] = utf8Code250;
	utf8CodeLengths[250] = 1;
	utf32Codes[251]      = 51;
	uint8 utf8Code251[]  = {0x33};
	utf8Codes[251] = utf8Code251;
	utf8CodeLengths[251] = 1;
	utf32Codes[252]      = 1718;
	uint8 utf8Code252[]  = {0xda, 0xb6};
	utf8Codes[252] = utf8Code252;
	utf8CodeLengths[252] = 2;
	utf32Codes[253]      = 86;
	uint8 utf8Code253[]  = {0x56};
	utf8Codes[253] = utf8Code253;
	utf8CodeLengths[253] = 1;
	utf32Codes[254]      = 34;
	uint8 utf8Code254[]  = {0x22};
	utf8Codes[254] = utf8Code254;
	utf8CodeLengths[254] = 1;
	utf32Codes[255]      = 56;
	uint8 utf8Code255[]  = {0x38};
	utf8Codes[255] = utf8Code255;
	utf8CodeLengths[255] = 1;

	for (uint32 i = 0; i < utfSamplesCount; ++i)
	{
		_TestEndodeDecode(utf8Codes[i], utf32Codes[i], utf8CodeLengths[i], RKStringUTF_SUCCESS);
	}
}

void _TestEdges()
{
	uint32 zeroUTF32 = 0;
	uint8 zeroUTF8[] = {0};
	_TestEndodeDecode(zeroUTF8, zeroUTF32, 1, RKStringUTF_ZERO_CODE);

	uint32 minUTF32_1Byte  = 1;
	uint8  minUTF8_1Byte[] = {1};
	uint32 maxUTF32_1Byte  = 0x7f;
	uint8  maxUTF8_1Byte[] = {0x7f};
	_TestEndodeDecode(minUTF8_1Byte, minUTF32_1Byte, 1, RKStringUTF_SUCCESS);
	_TestEndodeDecode(maxUTF8_1Byte, maxUTF32_1Byte, 1, RKStringUTF_SUCCESS);

	uint32 minUTF32_2Byte  = 0x80;
	uint8  minUTF8_2Byte[] = {0xC2, 0x80};
	uint32 maxUTF32_2Byte  = 0x07FF;
	uint8  maxUTF8_2Byte[] = {0xDF, 0xBF};
	_TestEndodeDecode(minUTF8_2Byte, minUTF32_2Byte, 2, RKStringUTF_SUCCESS);
	_TestEndodeDecode(maxUTF8_2Byte, maxUTF32_2Byte, 2, RKStringUTF_SUCCESS);

	uint32 minUTF32_3Byte  = 0x0800;
	uint8  minUTF8_3Byte[] = {0xE0, 0xA0, 0x80};
	uint32 maxUTF32_3Byte  = 0xFFFD;
	uint8  maxUTF8_3Byte[] = {0xEF, 0xBF, 0xBD};
	_TestEndodeDecode(minUTF8_3Byte, minUTF32_3Byte, 3, RKStringUTF_SUCCESS);
	_TestEndodeDecode(maxUTF8_3Byte, maxUTF32_3Byte, 3, RKStringUTF_SUCCESS);

	uint32 minUTF32_4Byte  = 0x010000;
	uint8  minUTF8_4Byte[] = {0xF0, 0x90, 0x80, 0x80};
	uint32 maxUTF32_4Byte  = 0x1FFFFF;
	uint8  maxUTF8_4Byte[] = {0xF7, 0xBF, 0xBF, 0xBF};
	_TestEndodeDecode(minUTF8_4Byte, minUTF32_4Byte, 4, RKStringUTF_SUCCESS);
	_TestEndodeDecode(maxUTF8_4Byte, maxUTF32_4Byte, 4, RKStringUTF_SUCCESS);
}

static void _EnsureInvalid(uint8* utf8, uint32 expectedOffset = 1)
{
	uint32 decodedBefore = rand();
	uint32 decoded = decodedBefore;
	uint32 offset;
	RKStringUTF_RC rc = RKStringUTF_DecodeUTF8((char*)utf8, -1, decoded, offset);
	RKASSERT(rc == RKStringUTF_INVALID_CODE, "");
	RKASSERT(offset == expectedOffset, "");
	RKASSERT(decoded == decodedBefore, "");
	RKASSERT(RKStringUTF_NextCodeLength((char*)utf8, -1) == offset, "");
}

static void _TestBannedCode(uint32 utf32Code)
{
	RKASSERT(utf32Code > 0x07FF && utf32Code <= 0xFFFF, "");
	uint8 utf8[3];
	uint32 offset;
	RKStringUTF_RC rc = RKStringUTF_EncodeUTF8(utf32Code, (char*)utf8, -1, offset);
	RKASSERT(rc == RKStringUTF_INVALID_CODE, "");
	RKASSERT(offset == 0, "");
	RKASSERT(RKStringUTF_EncodedLength(utf32Code) == offset, "");

	utf8[0] = 0xE0 | (utf32Code >> 12);
	utf8[1] = 0x80 | ((utf32Code >> 6) & 0x3F);
	utf8[2] = 0x80 | (utf32Code & 0x3F);

	uint32 decoded;
	rc = RKStringUTF_DecodeUTF8((char*)utf8, -1, decoded, offset);
	RKASSERT(rc == RKStringUTF_INVALID_CODE, "");
	RKASSERT(offset == 3, "");
	RKASSERT(decoded == utf32Code, ""); // optional, but helps to test our manual utf8 generation here
	RKASSERT(RKStringUTF_NextCodeLength((char*)utf8, -1) == offset, "");	
}

static void _TestOverlongAndInvalid()
{
// see http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt

// 3.1
	for (uint8 i = 0x80; i <= 0xBF; ++i)
	{
		_EnsureInvalid(&i);
	}
	{
		uint8 utf8[] = {0x80, 0x80};
		_EnsureInvalid(utf8);
	}

// 3.2
	for (int i = 0xC0; i <= 0xFF; ++i)
	{
		uint8 utf8[] = (uint8){i, 0};
		_EnsureInvalid(utf8);
	}

// 3.3
	{
		uint8 utf8Min[] = {0xC0, 0};
		uint8 utf8Max[] = {0xDB, 0};
		_EnsureInvalid(utf8Min, 1);
		_EnsureInvalid(utf8Max, 1);
	}
	{
		uint8 utf8Min[] = {0xE0, 0x80, 0};
		uint8 utf8Max[] = {0xEF, 0xBF, 0};
		_EnsureInvalid(utf8Min, 2);
		_EnsureInvalid(utf8Max, 2);
	}
	{
		uint8 utf8Min[] = {0xF0, 0x80, 0x80, 0};
		uint8 utf8Max[] = {0xF7, 0xBF, 0xBF, 0};
		_EnsureInvalid(utf8Min, 3);
		_EnsureInvalid(utf8Max, 3);
	}

// 3.5
	{
		uint8 utf8_1[] = {0xFE};
		uint8 utf8_2[] = {0xFF};
		_EnsureInvalid(utf8_1);
		_EnsureInvalid(utf8_2);
	}

// 4.1 - 4.2
	{
		uint8 utf8[] = {0xC0, 0xAF};
		uint32 utf32 = 0x2f;
		_TestEndodeDecode(utf8, utf32, 2, RKStringUTF_OVERLONG_CODE);
	}

	{
		uint8 utf8[] = {0xE0, 0x80, 0xAF};
		uint32 utf32 = 0x2f;
		_TestEndodeDecode(utf8, utf32, 3, RKStringUTF_OVERLONG_CODE);
	}

	{
		uint8 utf8[] = {0xF0, 0x80, 0x80, 0xAF};
		uint32 utf32 = 0x2f;
		_TestEndodeDecode(utf8, utf32, 4, RKStringUTF_OVERLONG_CODE);
	}

	{
		uint8 utf8[] = {0xF8, 0x80, 0x80, 0x80, 0xAF};
		uint32 utf32 = 0x2f;
		_TestEndodeDecode(utf8, utf32, 1, RKStringUTF_INVALID_CODE);
	}

	{
		uint8 utf8[] = {0xFC, 0x80, 0x80, 0x80, 0x80, 0xAF};
		uint32 utf32 = 0x2f;
		_TestEndodeDecode(utf8, utf32, 1, RKStringUTF_INVALID_CODE);
	}

// 4.3
	{
		uint8 utf8[] = {0xC0, 0x80};
		uint32 decoded;
		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_DecodeUTF8((char*)utf8, -1, decoded, offset);
		RKASSERT(rc == RKStringUTF_ZERO_CODE, "");
		RKASSERT(offset == 2, "");
		RKASSERT(decoded == 0, "");
	}

// 5.1
	for (uint32 i = 0xD800; i <= 0xDFFF; ++i)
	{
		_TestBannedCode(i);
	}

// 5.3
	for (uint32 i = 0xFFFE; i <= 0xFFFF; ++i)
	{
		_TestBannedCode(i);
	}

	// too big codes
	{
		uint32 utf32 = -1; // too big, not a valid utf
		RKASSERT(RKStringUTF_EncodedLength(utf32) == 0, "");
		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_EncodeUTF8(utf32, NULL, 0, offset);
		RKASSERT(offset == 0, "");
		RKASSERT(rc == RKStringUTF_INVALID_CODE, "");
	}
}

static void _TestBufferOverruns()
{
	{
		uint32 utf32  = 1;
		uint8  utf8[] = {1};

		uint32 decoded;
		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_DecodeUTF8((char*)utf8, 0, decoded, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 0, "");

		rc = RKStringUTF_DecodeUTF8(NULL, 0, decoded, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 0, "");
	}
	
	{
		uint32 utf32  = 0x010000;
		uint8  utf8[] = {0xF0, 0x90, 0x80, 0x80};
		
		uint32 decoded;
		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_DecodeUTF8((char*)utf8, 3, decoded, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 3, "");

		rc = RKStringUTF_DecodeUTF8((char*)utf8, 1, decoded, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 1, "");
	}

	{
		uint32 utf32  = 0x010000;
		uint8  utf8[4];

		uint32 offset;
		RKStringUTF_RC rc = RKStringUTF_EncodeUTF8(utf32, (char*)utf8, 3, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 0, "");

		rc = RKStringUTF_EncodeUTF8(utf32, (char*)utf8, 1, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 0, "");

		rc = RKStringUTF_EncodeUTF8(utf32, NULL, 0, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 0, "");

		rc = RKStringUTF_EncodeUTF8(1, NULL, 0, offset);
		RKASSERT(rc == RKStringUTF_BUFFER_OVERRUN, "");
		RKASSERT(offset == 0, "");
	}
}

static void _TestIteratorBasics()
{
	// Greek word "kosme"
	static const uint32 kosmeUtf8Size = 11;
	uint8 kosmeUtf8[kosmeUtf8Size] = {0xce, 0xba, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, 0xce, 0xbc, 0xce, 0xb5};

	static const uint32 kosmeUtf32Size = 5;
	uint32 kosmeUtf32[kosmeUtf32Size] = {0x03ba, 0x1f79, 0x03c3, 0x03bc, 0x03b5};

	RKStringUTF_Iterator<uint32> it  = RKStringUTF_Iterator<uint32>((const char*)kosmeUtf8, kosmeUtf8Size);
	RKStringUTF_Iterator<uint32> end = RKStringUTF_Iterator<uint32>((const char*)kosmeUtf8 + kosmeUtf8Size, 0);
	for (uint32 i = 0; i < kosmeUtf32Size; ++i, ++it)
	{
		RKASSERT(it.IsCurrentCodeValid(), "");
		RKASSERT(*it == kosmeUtf32[i], "");
	}
	RKASSERT(it == end, "");
}

static void _TestIteratorInvalidAndTruncation()
{
	// wrong "kosme", latin k, broken m
	static const uint32 kosmeUtf8Size = 9;
	uint8 kosmeUtf8[kosmeUtf8Size] = {/* latin k */ 0x6b, 0xe1, 0xbd, 0xb9, 0xcf, 0x83, /* broken */ 0x80, 0xce, 0xb5};

	static const uint32 kosmeUtf32Size = 5;
	uint32 kosmeUtf32[kosmeUtf32Size] = {0x6b, 0x1f79, 0x03c3, 0, 0x03b5};
	const char* kosmeTruncated = "k##?#";

	{
		RKStringUTF_Iterator<uint32> it  = RKStringUTF_Iterator<uint32>((const char*)kosmeUtf8, kosmeUtf8Size);
		RKStringUTF_Iterator<uint32> end = RKStringUTF_Iterator<uint32>((const char*)kosmeUtf8 + kosmeUtf8Size, 0);
		for (uint32 i = 0; i < kosmeUtf32Size; ++i, ++it)
		{
			if (i != 3)
			{
				RKASSERT(it.IsCurrentCodeValid(), "");
				RKASSERT(*it == kosmeUtf32[i], "");
			}
			else
			{
				RKASSERT(!it.IsCurrentCodeValid(), "");
				RKASSERT(*it == RKStringUTF_Iterator<uint32>::InvalidCodeSubstitution, "");
			}
		}
		RKASSERT(it == end, "");
	}

	{
		RKStringUTF_Iterator<char> it  = RKStringUTF_Iterator<char>((const char*)kosmeUtf8, kosmeUtf8Size);
		RKStringUTF_Iterator<char> end = RKStringUTF_Iterator<char>((const char*)kosmeUtf8 + kosmeUtf8Size, 0);
		for (uint32 i = 0; i < kosmeUtf32Size; ++i, ++it)
		{
			if (i != 3)
			{
				RKASSERT(it.IsCurrentCodeValid(), "");
			}
			else
			{
				RKASSERT(!it.IsCurrentCodeValid(), "");
			}
			RKASSERT(kosmeTruncated[i] == *it, "");
		}
		RKASSERT(it == end, "");
	}

	// signed/unsigned truncation test
	{
		static const uint32 utf8Size = 11;
		uint8 utf8[utf8Size] = {
														0x7f, 
														0xDF, 0xBF,
														0xEF, 0xBF, 0xBD,
														0xF0, 0x90, 0x80, 0x80, // truncated,
														0x80, // invalid
														};

		static const uint32 expectedSize = 5;
		int16 expected[expectedSize] = {int16(0x007f), int16(0x07FF), int16(0xFFFD), RKStringUTF_Iterator<int16>::TruncatedCodeSubstitution, RKStringUTF_Iterator<int16>::InvalidCodeSubstitution};

		RKStringUTF_Iterator<int16> it  = RKStringUTF_Iterator<int16>((const char*)utf8, utf8Size);
		RKStringUTF_Iterator<int16> end = RKStringUTF_Iterator<int16>((const char*)utf8 + utf8Size, 0);
		for (uint32 i = 0; i < expectedSize; ++i, ++it)
		{
			if (i == 3)
			{
				RKASSERT(it.IsCurrentCodeTruncated(), "");
			}
			if (i != 4)
			{
				RKASSERT(it.IsCurrentCodeValid(), "");
				RKASSERT(it.CurrentCodeLength() == (i + 1), "");
			}
			else
			{
				RKASSERT(it.CurrentCodeLength() == 1, "");
				RKASSERT(!it.IsCurrentCodeValid(), "");
			}
			RKASSERT(expected[i] == *it, "");
		}
		RKASSERT(it == end, "");
	}
}

void _TestRKStringUTF()
{
	_TestEdges();

	_TestOnRandomData();

	_TestOverlongAndInvalid();

	_TestBufferOverruns();

	_TestIteratorBasics();

	_TestIteratorInvalidAndTruncation();
}
}