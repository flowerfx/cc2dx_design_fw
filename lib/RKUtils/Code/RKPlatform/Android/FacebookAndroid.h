#ifndef __CASUALCORE_FACEBOOKANDROID_H_
#define __CASUALCORE_FACEBOOKANDROID_H_

#include "FacebookManager.h"
#include <jni.h>

namespace CasualCore
{
	/**
	 * This class is a wrapper for things related Facebook on Android.
	 */

	class FacebookAndroid
	{
    public:
		/**
		 * Initialize the static class (including JNI pointers through InitializeJNI).
		 *
		 * \param parent Pointer to the FacebookManager (please note it's platform-independent).
		 */
        static void Initialize(const FacebookManager* parent);
		static void Release();

		/**
		 * Answer whether the static class is initialized.
		 *
		 * \return Answer whether the class is initialized.
		 */
		static bool IsLoggedIn();

		/**
		 * Invoke the native login using the Facebook SDK on Android.
		 *
		 * \param appId Application ID.
		 */
        static void Login(unsigned long long appId);

		/**
		 * Invoke the native function to retrieve friends of the Facebook SDK on Android.
		 */
        static void GetFriends();

		/**
		 * Like an entity (page, application, etc)
		 */
        static void Like(const char* url);

		/**
		 * Invoke the native function to post on the user's wall using the Facebook SDK on Android.
		 *
		 * \param actionName Name of the hyperlink.
		 * \param actionLink Hyperlink to the action associated to the post.
		 * \param name Name associated to the wall post.
		 * \param caption Caption of the wall post.
		 * \param description Description of the wall post.
		 * \param link Link of the wall post.
		 * \param pictureURL URL of the picture to display.
		 */
        static void PostOnUserWall(const char* actionName, const char* actionLink, const char* name, const char* caption, const char* description, const char* link, const char* pictureURL);

		/**
		 * Invoke the native function to post on a user friend's wall using the Facebook SDK on Android.
		 *
		 * \param socialFriend Social network friend. Please note it's platform- and implementation-independent.
		 * \param actionName Name of the hyperlink.
		 * \param actionLink Hyperlink to the action associated to the post.
		 * \param name Name associated to the wall post.
		 * \param caption Caption of the wall post.
		 * \param description Description of the wall post.
		 * \param link Link of the wall post.
		 * \param pictureURL URL of the picture to display.
		 */
		static void PostMessageToFriend(const CasualCore::SocialNetworkFriend& socialFriend, const char* actionName, const char* actionLink, const char* name, const char* caption, const char* description, const char* link, const char* pictureURL);

		/**
		 * Invoke the native function to invite friends of the Facebook SDK on Android.
		 *
		 * \param friends Collection of friends to invite.
		 */
		static void SendInviteToFriends(const RKList<SocialNetworkFriend>& friends, const char* message, const char* notificationText);

		/**
		 * Invoke the native function to open a browser with the given URL address.
		 *
		 * \param url URL to open in a browser.
		 */
        static bool OpenURL(const char* url);

		/**
		 * Invoke the native function to entities the user information using the Facebook SDK on Android.
		 */
		static void GetUserInformation();

		/**
		 * Invoke the native function to entities the user likes using the Facebook SDK on Android.
		 *
		 * \param multiplier Multiplier of the orientation. 1.0 means no change, 0.5 means 90 degrees CW, -0.5 means 90 degrees CCW and so son.
		 */
		static void SetOrientationMultiplier(float multiplier);

		/**
		 * Initialize the JNI pointer of the class.
		 *
		 * \param game JNI pointer to the class.
		 */
		static void InitializeJNI(jclass game);

		/**
		 * Function to be called from Java (callback) when GetFriends() is done.
		 *
		 * \NOTE It is organized as a structure of arrays (ids, names, emails, areAppUsers), instead of having an array of structures.
		 *
		 * \param ids IDs of the user friends.
		 * \param names Names of user friends.
		 * \param emails Emails of user friends.
		 * \param areAppUsers Array of booleans telling whether the user is an app user.
		 */
		static void ProcessFriendsFromJNI(jobjectArray ids, jobjectArray names, jobjectArray emails, jbooleanArray areAppUsers);

		/**
		 * Function to be called from Java (callback) when GetRecommendations() is done.
		 *
		 * \NOTE It is organized as a structure of arrays (ids, names, emails, areAppUsers), instead of having an array of structures.
		 *
		 * \param ids IDs of the entities the user likes.
		 * \param names Names of user friends.
		 */
		static void ProcessRecommendationsFromJNI(jobjectArray ids, jobjectArray names);


	private:
		static void GetEnv(JNIEnv*& env);

		static FacebookManager* m_parent;

		static jclass m_class;
		static jmethodID m_login;
		static jmethodID m_isLoggedIn;
		static jmethodID m_getFriends;
		static jmethodID m_postOnUserWall;
		static jmethodID m_postMessageToFriend;
		static jmethodID m_sendInviteToFriends;
		static jmethodID m_getRecommendations;
		static jmethodID m_setOrientationMultiplier;
	};
}

#endif // __CASUALCORE_FACEBOOKANDROID_H_
