//
//  Platform.hpp
//  iwin_ios
//
//  Created by truongps on 5/5/16.
//
//

#ifndef Platform_hpp
#define Platform_hpp
#include <sstream>
#include "cocos2d.h"

namespace Platform
{

	//This class for Android platform
	class GooglePlayServices
	{
	public:	
		static bool isSignedIn();	
		static void signIn();	
		static void signOut();	
		static void submitScore(const char *leaderboardID, long score);	
		static void unlockAchievement(const char *achievementID);	
		static void incrementAchievement(const char *achievementID, int numSteps);	
		static void showAchievements();	
		static void showLeaderboards();	
		static void showLeaderboard(const char *leaderboardID);
	};
	//This class for IOS platform
	class GameCenter
	{
	public:
		
		static void signIn();
		static void showLeaderboard();
		static void showAchievements();
		static void submitScore(int scoreNumber, cocos2d::__String leaderboardID);
		static void unlockAchievement(cocos2d::__String achievementID);
		static void unlockAchievement(cocos2d::__String achievementID, float percent);
		static void resetPlayerAchievements();
	};
	class Notifications
	{
	public:
		
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int notificationTag);
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int notificationTag);
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, int repeatInterval, int notificationTag);
		static void scheduleLocalNotification(float delay, cocos2d::__String textToDisplay, cocos2d::__String notificationTitle, cocos2d::__String notificationAction, int repeatInterval, int notificationTag);
		static void unscheduleAllLocalNotifications();
		static void unscheduleLocalNotification(int notificationTag);

	};

	
	 class GPSLocation 
	 {
	 public:
		 GPSLocation(double longitude, double latitude)
		 {
			 this->longitude = longitude;
			 this->latitude = latitude;
		 }

		 double latitude;
		 double longitude;

		 std::string toString() 
		 {
			 std::ostringstream oss;
			 oss << "longitude: " << longitude << ", latitude: " << latitude;			 
			 return  oss.str();
		 }
	 };

	 void getCurrentLocation(std::function<void (const double,const double)> _resultCallback);

	 void makeCall(std::string phoneNumber, std::string showName);
	 void showWeb(const std::string url);
	 void showSMSComposer(std::string content, std::string toPhoneNumber);
	 void makeCall(std::string phone_number);
	 void sendSMS(std::string content, std::string toPhoneNumber);
	 void vibrate(int milis);
	 std::string getVersion();
	 std::string getPackageName();
	 std::string getAppName();

	 void handleSMSSucceed(std::string phone_number, std::string sms_body);
	 void handleSMSFail();
	 
	
}

#endif /* Platform_hpp */
