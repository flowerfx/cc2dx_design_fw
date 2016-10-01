#ifndef __CASUALCORE_PLATFORMIOS_H_
#define __CASUALCORE_PLATFORMIOS_H_

/**
 * IGPTracking()
 * Read the doc in root/lib/IGPTracking/how_to_integrate.txt
 * This function suppose to be called in two cases:
 *    1.Main Game Loading. It is done in the PlatformIOS.mm. The user do not need to worry about it
 *    2.When your app resumes from Background. The resume function is in the Game class, which means the IGPTracking() should be called in individual game's resume function. Take a look the example in DemoTest project root/GameStates/StateTest.cpp
 */
void IGPTracking();

#endif
