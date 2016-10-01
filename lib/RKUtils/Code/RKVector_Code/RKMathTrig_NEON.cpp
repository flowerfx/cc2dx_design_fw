#include "RKMathTrig.h"
namespace RKUtils
{
#if defined(__ARM_NEON__) && !defined(__arm64__)
	// returns {sin, cos}
	void RKMath_SinCos(float angle, RKVector2& results)
	{
		static const uint32 constants[] = {
			/* q2 d4-d5 */ 0x3ea2f983, 0x40490fdb, 0x80000000, 0x80000000, // 1/pi, pi,      sign bit, sign bit
			/* q3 d6-d7 */ 0x3fa2f983, 0xbecf817b, 0x3e666666, 0x3fc90fdb, // 4/pi, -4/pi^2, 0.225,    pi/2
		};
		asm volatile(
			// load constants into [q1, q2]
			"vldmia %2, { q2, q3 }\n\t"

			// fill d0 with [angleSin, angleCos]
			"vmov.f32 s0, %1\n\t"
			"vadd.f32 s1, s0, s15\n\t"

			// angle range fixup, storing angle sign
			"vand.i32 d3, d0, d5\n\t"     // d3 now contains sign bit of d0
			"veor.i32 d0, d0, d3\n\t"     // clear sign bit of d0, d0 now contains abs(angle)

			// angle range fixup, calculating periods count
			"vmul.f32 d1, d0, d4[0]\n\t"  // q1 now contains angle / PI
			"vcvt.u32.f32 d1, d1\n\t"     // truncation
			"vcvt.f32.u32 d2, d1\n\t"     // both d1 and d2 now contains trunc(angle / PI), but d1 as int and d2 as float

			// angle range fixup, period-based sign fix
			"vshl.i32 d1, #31\n\t"        // move first bit into sign bit position
			"veor.i32 d3, d1\n\t"         // fix sign bit stored in q3

			// angle range fixup, taking period remainder
			"vmls.f32 d0, d2, d4[1]\n\t"  // d0 now contain angle in range [0, pi], q2 content is not needed anymore

			// calculate sine expecting d0 to contain angles in range [0, pi]
			"vmul.f32 d2, d0, d0\n\t"     // d2 contains angle^2
			"vmul.f32 d1, d0, d6[0]\n\t"  // d1 contains linear component
			"vmla.f32 d1, d2, d6[1]\n\t"  // d1 contains result (e.g. y)

			// second iteration, precision enhancement, result in and out in d1
			"vmul.f32 d2, d1, d1\n\t"     // d2 contains d1^2
			"vsub.f32 d2, d1\n\t"         // d2 contains d1^2 - d1
			"vmla.f32 d1, d2, d7[0]\n\t"  // d1 contains result

			// fixing result sign
			"veor.i32 d0, d1, d3\n\t"

			// storing result
			"vstmia %0, { d0 }\n\t"
			:: "r"(&results), "r"(angle), "r"(constants)
			: "memory","cc","q0","q1","q2","q3"
			);
	}

	// returns {sin1, cos1, sin2, cos2}
	void RKMath_SinCos2(const RKVector2& angles, RKVector& results)
	{
		static const uint32 constants[] = {
			/* q1 d2-d3 */ 0,          0x3fc90fdb, 0,          0x3fc90fdb, // 0,    pi/2,    0,        pi/2       <- sin to cos conversion angle vector
			/* q2 d4-d5 */ 0x3ea2f983, 0x40490fdb, 0x80000000, 0xdeadc0de, // 1/pi, pi,      sign bit, not used
			/* q3 d6-d7 */ 0x3fa2f983, 0xbecf817b, 0x3e666666, 0x3f466666, // 4/pi, -4/pi^2, 0.225,    0.775
		};
		asm volatile(
			// fill q0 with [angle1, angle1, angle2, angle2]
			"vldmia %1, { d1 }\n\t"
			"vdup.f32 d0, d1[0]\n\t"
			"vdup.f32 d1, d1[1]\n\t"

			// load constants into [q1, q2, q3]
			"vldmia %2, { q1, q2, q3 }\n\t"

			// fixup cos/sin angles
			"vadd.f32 q0, q1\n\t"         // cos converted into sin, q1 content is not needed anymore

			// angle range fixup, storing angle sign
			"vdup.i32 q8, d5[0]\n\t"      // q8 now contains vector of sign bits
			"vand.i32 q8, q0, q8\n\t"     // q8 now contains sign bit of q0
			"veor.i32 q0, q0, q8\n\t"     // clear sign bit of q0, q0 now contains abs(angle)

			// angle range fixup, calculating periods count
			"vmul.f32 q1, q0, d4[0]\n\t"  // q1 now contains angle / PI
			"vcvt.u32.f32 q1, q1\n\t"     // truncation
			"vcvt.f32.u32 q9, q1\n\t"     // both q1 and q9 now contains trunc(angle / PI), but q1 as int and q9 as float

			// angle range fixup, period-based sign fix
			"vshl.i32 q1, #31\n\t"        // move first bit into sign bit position
			"veor.i32 q8, q1\n\t"         // fix sign bit stored in q8

			// angle range fixup, taking period remainder
			"vmls.f32 q0, q9, d4[1]\n\t"  // q0 now contain angle in range [0, pi], q2 content is not needed anymore

			// calculate sine expecting q0 to contain angles in range [0, pi]
			"vmul.f32 q1, q0, d6[0]\n\t"  // q1 contains B * q0
			"vmul.f32 q2, q0, q0\n\t"     // q2 contains q0^2
			"vmla.f32 q1, q2, d6[1]\n\t"  // q1 contains result (e.g. y)

			// second iteration, precision enhancement, result in and out in q1
			"vmul.f32 q0, q1, d7[1]\n\t"  // q0 contains Q*q1
			"vmul.f32 q2, q1, q1\n\t"     // q2 contains q1^2
			"vmla.f32 q0, q2, d7[0]\n\t"  // q0 contains result

			// fixing result sign
			"veor.i32 q0, q8\n\t"

			// storing result
			"vstmia %0, { q0 }\n\t"
			:: "r"(&results), "r"(&angles), "r"(constants)
			: "memory","cc","q0","q1","q2","q3","q8","q9"
			);
	}
#endif
}
