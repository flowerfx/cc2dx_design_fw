#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

namespace Constants 
{
	 #define  LOG_CONFIG  0 //Application.LOG_DEBUG
	 #define  HAS_TEST_SERVER  0
	/**
	 * Bật cờ này thành 1 khi build bản release macos để check receipt
	 */
	 #define  CHECK_MAC_RECEIPT  0

	 #define  GIAI_DAU_BUILD  0

	 #define  HIDE_CHANGE_AVATAR_ON_IPAD_MINI1  1

	 #define  TEST_PARSE_CRASH_REPORT  0
	 #define  HAVE_CHIEU_BI  0

	/**
	 * Nhung tham so dung de thuc hien load resource dong.
	 */
	 #define  SCREEN_SIZE240320  "240320"
	 #define  SCREEN_SIZE320480  "320480"
	 #define  SCREEN_SIZE480800  "480800"
	 #define  SCREEN_SIZE640960  "640960"

	/*
	 * Parse Setting
	 */
	 #define  PARSE_APP_ID  "DKOzf6Fumcpfq0NdATk3rhuM4TUyINrpR9RcXEKF"
	 #define  PARSE_CLIENT_ID  "JDWkiVhqpbhLUhlHYgluGDwDZn8CcSQpsXfNApnq"
	 #define  PARSE_WINFREE_BACKGROUND  "imgWinFreeBG"
	 #define  PARSE_INSTALLATION_URL_LOCAL  "http://10.8.14.79:8090/install/new_install"
	 #define  PARSE_INSTALLATION_URL_RELEASE  "http://av.iwin.vn/install/new_install"
	 #define  USER_INSTALLATION_CLASS_NAME  "User_Installation"
	 #define  USER_INSTALLATION_USER_KEY  "user"
	 #define  USER_INSTALLATION_INSTALLATION_KEY  "installation"

	/*
	 * Key để gửi installation lên server mình
	 */
	 #define  KEY_VALUE  "4IOXsYqdnTqCg"

	/**
	 * Configure version for Desktop and Html5.
	 */
	 #define  DESKTOP_HTML5_VERSION  "5.0.1"

	// Constants support countly.
	 #define  CountlyServerDefault  "http://203.162.79.41"
	 #define  CountlyKeyDefault  "d63dd2c4e075961fa65e37674a109cfdb7d7eeab"
	 #define  CountlyAppIdDefault  "5461d3f93d7bdbdf5f000001"

	// Key check valid key.
	 #define  DEBUG_HASHKEY  "d/dcfPbXv8TIdi9GG8yC0xET1Jg"
	 #define  RELEASE_HASHKEY  "k6E2FReCYNsmKAOAljxh8ZyE2Uk"

	 #define  GOOGLE_PLAY_KEY  "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAkQNP6ZM/Ydid/PojmJhKEhPSSSN9HEIDTXSUFFgAVRFGE9QVApbyYX1DbPSvE2Fk/T9XXMxDZQ0l3zTgm+mbCzsuv0Gw1rh/HQ6twr+2X1jJyxFyblFaeiqX909PmWNFC4dm5mEEmEDbZ/PT+OepUgqyJB5putt6zBOPzG7lLCnD2TRkXw9ze+L/roxdknc9RPw3tmMtXCxSktib31Z9vUo0rRmr5eEZtSYCbfXeRrXXwwMFL7xbpbEEK1UaDUeH2c3QMUwN+ji6/pA2CrWTzIJP1hAaUzH3/Uv3fBhZofeyoYqb87D6xftHf6Ir29FTYtaS4jVz6auyiXLjpAd2IQIDAQAB"
	 #define  GOOGLE_PLAY_KEY_FOR_IWIN_VN_M_P592  "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAlKuHlbO86GwIBB9UrdgnG0H2F/wkfUsD7sC9sQkFt08nmf920jXL2T16T458Qy42PfpKPkgwDkrB6lV68rVySqmlvtZxXem5AkLnvPM/up8QxiV3jgQ6DED7xuwzVKZxmPr0Ys5APeJtQrJcRXQI3UhNsba7zCyV00sKjUjUhV9BP7HkzfrpS3n08CN9UlgG6cKbh5sEB3cCvEhbl4simPCcRrwXJB7zTUjl3xTzqW7UNUlS1q8cRoMADC9HnKoYk6g/1vpG+SopdGG/I+yyVqFUipjne107Uiz8qlkYk30kxtqAxGirec0A0spWSGJwRYGBRWdBAtTJO4VCVZQ97QIDAQAB"
	 #define  GOOGLE_PLAY_KEY_FOR_IWIN_VN_M_P593  "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAh5qG3djiX47zgrlImLPIXiDdfDK/LLkiNbeGykj/xLrXSS7nUEgGAoK4nA6A8mRr2CWPAH/oQ+UzIlxW89vNLjtY+cBIQSVlnxwZmG+6d/ypTvLlBEC7Yp4Eh8fCBw8ZWqM9dyUZShlmcw5VbkiTv1LDufzifAdWFdaZ681xY1LOT5gUsHIrgkFbZm8hzwVdwidY9JYZENoXEHYsXh4Zvt6MOXUuX3c7rc45/0t8b20WLOqLfr0pirHIKMxvCnIrzL2heE/8MyorLabi+T0TV9knIsxBdeMIVscr1LreaoF6HqJR7EiMdDMORd/Kqh66VFtUw0Ix2aAFDFQTACVgnwIDAQAB"
	 #define  GOOGLE_PLAY_KEY_FOR_IWIN_VN_M  "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAzPnjuLvl07laEuJIeabUGiaS/RgAzhfgUl1kn1nUMx/dlYR03RGzoZGUH12i2RA0zHdnzf9gbghYNVL2sgf1tBq4+zZXsznmVeNiIF4zBM4mI11rk6iYF0u0dWayKyZULF3ndZPtl45osbXP9ADoCKITOtSxy6KfyeEZ+Wtsln4CdZ6RawtqABOztyCjJV/7fI1NAPSSQsnVeeDuw/0v8ezSFQjq+OdB8JdqRQVmk+TRNCNJU4c1kJcKKokMdW3ejzO5xo1MqYyqm+bv324sUIw8Y0bjyMS9aqDSFX0O68KENzDjVnPpETIexq99udu0JoMx0bboQDKKZzTbdoCJcwIDAQAB"

	// Mint Key
	 #define  IOS_BUGSENSE_KEY  "25013548"
	 #define  ANDROID_BUGSENSE_KEY  "f2c2d2f4"

	 #define  GAI  "UA-65027580-1"
	// Facebook App ID.
	 #define  FACEBOOK_APP_ID  "315837181868526"
	 #define  FACEBOOK_APP_ID_FOR_SALE  "304571766370555"
	 #define  FACEBOOK_APP_ID_FOR_MOBO  "1635025826732920"
	 #define  FACEBOOK_APP_ID_FOR_P592  "223727234642912"
	 #define  FACEBOOK_APP_ID_FOR_P593  "160368184348784"
	

	// Google Plus  ID
	// Account iwin.vn.full da bi banned.
	//  #define  GOOGLE_PLUS_CLIENT_ID 
	// "721589498812-v1bhplf8vprgreur79skj78s0bibj2pc.apps.googleusercontent.com"
	 #define  GOOGLE_PLUS_CLIENT_ID  "1069937706397-h4t751eck831ojl6qq4sfssklg05dta1.apps.googleusercontent.com"//
	 #define  REVERSE_GOOGLE_PLUS_CLIENT_ID  "com.googleusercontent.apps.721589498812-v1bhplf8vprgreur79skj78s0bibj2pc"
	 #define  GOOGLE_PLUS_CLIENT_ID_FOR_SALE  "721589498812-d747b79hmaomqmir0bsug74ekhos14fj.apps.googleusercontent.com"
	 #define  GOOGLE_PLUS_CLIENT_ID_FOR_MOBO  "699153824643-dqle7ckjtkmoeg2teiv2km1d8pgv96gu.apps.googleusercontent.com"
	 #define  REVERSE_GOOGLE_PLUS_CLIENT_ID_FOR_MOBO  "com.googleusercontent.apps.699153824643-dqle7ckjtkmoeg2teiv2km1d8pgv96gu"

	// AppsFlyerConfig iOS
	 #define  APPS_FLYER_DEV_KEY_IOS  "DYFqfjH7kT482Qmyh6AWFG"
	 #define  APPLE_APP_ID_IOS  "955700690"

	// InMobiManager iOS Advertiser
	 #define  APP_ID_INMOBI_IOS  "e34e7163a60344b9bcd4c5abf38a5b1d"

	// MobileAppTracking iOS
	 #define  ADVERTISER_ID_IOS  "17334"
	 #define  CONVERSION_KEY_IOS  "be3d19787ad17005ca720c2828c27e99"

	// [ACTConversionReporter
	// reportWithConversionID:@"962928602"label:@"PHadCPaj8lwQ2r-UywM"
	// value:@"1.00" isRepeatable:NO]
	// GoogleConversionTracking iOS
	 #define  CONVERSION_ID_IOS  "962928602"
	 #define  LABEL_CONVERSION_IOS  "Pz4PCLupt2gQ2r-UywM"

	// password cấm
	/* #define  listNotPassword[]  { "123456", "pa#sword", "12345678", "1234", "123456789", "12345",
			"dragon", "qwerty", "696969", "qwe123", "123abc", "123654", "987654321", "654321", "111111", "222222",
			"333333", "abc123", "444444", "555555", "666666", "777777", "888888", "999999", "000000", "qaz123",
			"123qaz", "aaaaaa", "zzzzzz", "87654321", "batman", "88888888", "999999999", "7777777", "blowjob", "xxxxxx",
			"loveyou", "1234567", "soccer", "killer", "superman", "fuckyou", "121212", "131313", "123123", "chelsea",
			"computer", "hardcore", "arsenal", "forever", "987654", "112233", "11223344", "samsung", "s@msung",
			"iphone", "iphone4", "iphone5", "iphone6" }*/

	// All Link Define Here.
	// #define  LINK_GET_REVIEW_APPS_INFO  "http://av.iwin.vn/wf/help/review"
	 #define  LINK_GET_REVIEW_APPS_INFO  "http://av.iwin.vn/wf/help/review_new"
	 #define  LINK_HELP_FOR_REVIEWING  "http://av.iwin.vn/wf/help/policy.htm"
	 #define  LINK_TERM  "http://iwin.mobo.vn/huong-dan-mobile/2-dieu-khoan-su-dung"
	 #define  LINK_BACKGROUND_WINFREE  "http://av.iwin.vn/wf/imgWinFreeBG.png"
	 #define  LINK_CUSTOMER_SERVICE  "http://hotro.iwin.vn/login?type2"

	// Chat
	 #define  MAX_LENGTH_CHAT  100
	
	// Khoảng thời gian sẽ báo rung khi sắp hết lượt (milisecond)
	 #define  TIME_VIBRATE_CARD_GAME  5000	// 5s cho game bài 
	 #define  TIME_VIBRATE_BOARD_GAME  10000	// 10s cho game cờ
	/**
	 * thoi gian goi lai ham lay danh sach 
	 * */
	 #define  TIME_GET_LIST_WAIT  60	
	
	//Appflyer Android
	 #define  APPFLYER_APP_ID  "DYFqfjH7kT482Qmyh6AWFG"
	
	//Pixel
	 #define  TRACKING_APP_ID  "570f3a724bd04d9a3778bceb"
	 #define  TRACKING_DOMAIN  "https://engine.itracking.io"
	 #define  TRACKING_SECRET_KEY  "9cuAprmtyCWYj6"
	 #define  TRACKING_FACEBOOK_ID  "315837181868526"
	
	 #define  ADWORDS_CONVERSION_ID_M  "962928602"
	 #define  ADWORDS_CONVERSION_LABEL_M  "2k99CPqN-GUQ2r-UywM"
	
	 #define  ADWORDS_CONVERSION_ID_FULL  "962928602"
	 #define  ADWORDS_CONVERSION_LABEL_FULL  "KBuUCKrTumgQ2r-UywM"
	
	 #define  ADWORDS_CONVERSION_ID_592  "962928602"
	 #define  ADWORDS_CONVERSION_LABEL_592  "Y3bWCNXC_2UQ2r-UywM"

	// OneSignal
	// normal : "c14a1bba-6089-4c32-bf71-252f6c63bfda"
	// vn.iwin.full : "6b9619e8-20bc-4350-813e-2e3f737911b9"
	 #define  ONESIGNAL_APP_ID  "c14a1bba-6089-4c32-bf71-252f6c63bfda"
	
	#define BASE_MONEY  1
	#define RUBY_MONEY  2
}
#endif // 