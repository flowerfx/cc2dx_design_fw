//  ===================================================================================================================
//
//      USER DEBUG (project) - XCode Config User Override
//
//  ===================================================================================================================

// NOTE : You can use this file to override debug settings at the project level. You can find settings
//        examples and explanations in section "EXAMPLES AND IMPORTANT NOTES"


// --------------------------------------------------------------------------------------------------------------------
// User section (write your stuff below)
// --------------------------------------------------------------------------------------------------------------------

// NOTE : SDKROOT cannot be overriden in this file since it is defined at the project level
//        via the IDE. To change SDKROOT through xcconfigs, use user-debug-libvox.h rather.

// --------------------------------------------------------------------------------------------------------------------
// EXAMPLES AND IMPORTANT NOTES
// --------------------------------------------------------------------------------------------------------------------

// --------------------
// Override
// --------------------

// The following setting if effective for both simulator and device

//NINJA_POWERS = ALWAYS

// If you only want to change a setting for the simulator or the device, you can use the "Conditional Build
// Settings" (See Apple/Xcode documentation for more information)

//NINJA_POWERS[sdk=iphoneos*]        = ALMIGHTY
//NINJA_POWERS[sdk=iphonesimulator*] = MISSING

// Also, if your user settings don't show up in the xcode settings, double check your file syntax as
// xcode is very picky when you use Conditional Build Settings

// --------------------
// Partial Override
// --------------------

// If you want to add a preprocessor definition, you should not override GCC_PREPROCESSOR_DEFINITIONS
// as you will lose the sєttings done by the project and targets. Instead, use the following user settings:
//
//     USER_COMMON_GCC_PREPROCESSOR_DEFINITIONS							// For common debug or release definitions (project)
//     USER_CONFIG_GCC_PREPROCESSOR_DEFINITIONS							// For debug/release definitions (project)
//     USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS						// For platform definitions (project)
//     USER_TARGET_COMMON_GCC_PREPROCESSOR_DEFINITIONS					// For common definitions (target)
//     USER_TARGET_CONFIG_GCC_PREPROCESSOR_DEFINITIONS					// For debug/release definitions (target)
//     USER_TARGET_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS				// For platform definitions (target)
//

// The same applies to HEADER_SEARCH_PATHS (use the following instead)

//     USER_COMMON_HEADER_SEARCH_PATHS									// For common debug or release definitions (project)
//     USER_CONFIG_HEADER_SEARCH_PATHS									// For debug/release definitions (project)
//     USER_PLATFORM_HEADER_SEARCH_PATHS								// For platform definitions (project)
//     USER_TARGET_COMMON_HEADER_SEARCH_PATHS							// For common definitions (target)
//     USER_TARGET_CONFIG_HEADER_SEARCH_PATHS							// For debug/release definitions (target)
//     USER_TARGET_PLATFORM_HEADER_SEARCH_PATHS							// For platform definitions (target)

// NOTE 1 : USER_PLATFORM_* and USER_TARGET_PLATFORM_* require that you specify [sdk=iphoneos*] or
// [sdk=iphonesimulator*]. For example:

//USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS[sdk=iphoneos*]        = ONLY_ON_IPHONE
//USER_PLATFORM_GCC_PREPROCESSOR_DEFINITIONS[sdk=iphonesimulator*] = NOBODY_USE_THIS

// NOTE 2 : Do not use USER_PLATFORM_* = or USER_TARGET_PLATFORM_* = to set both platforms at the same time.
// Use USER_COMMON_* or USER_TARGET_COMMON_* for this purpose (see NOTE 3).

// NOTE 3 : You are discouraged to use USER_COMMON_* or USER_TARGET_COMMON_* directly in this file, as it won't
// be updated for both debug and release (since this file only affects debug). If you need a common definition,
// you can make your own user common configuration file and include it here, or duplicate the preprocessor
// definition and use USER_CONFIG_* or USER_TARGET_CONFIG_*.