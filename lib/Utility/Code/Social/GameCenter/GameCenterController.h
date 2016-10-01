#ifndef _GAME_CENTER_CONTROLLER_H_
#define _GAME_CENTER_CONTROLLER_H_

#include "DisplayView.h"
#include "cocos2d.h"
#include "RKString_Code/RKString.h"
USING_NS_CC;
using namespace RKUtils;
namespace Utility
{
    namespace Social
    {
        namespace GameCenter
        {
            class GameCenterController
            {
                void authenticationRequired();
        
            public:
            
                GameCenterController();
                ~GameCenterController();
                
                void Init();
                
                void PostScoreToGameCenter(int score , RKString id_leaderboard);
                
                void ShowLeaderboard(RKString id_leaderboard);
                
                int GetRankLeaderboard(RKString id_leaderboard);
                int GetBestScoreLeaderboard(RKString id_leaderboard);
                
                bool IsGameCenterOpen();
                
                bool IsGameCenterAuthenticate();
                
                bool IsLoadleaderboardInfoSucceed();
                
                void ShowGameCenterScheme();
                
                RKString GetGameCenterName();
                
            };
		
        }
    }
}
#endif//_GAME_CENTER_CONTROLLER_H_