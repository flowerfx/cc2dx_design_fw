//
//  GameCenterWrapper.m
//  MainGameProj_iOS
//
//  Created by qchien on 11/24/15.
//
//

#import <Foundation/Foundation.h>
#include "GCHelper.h"
#include "GameCenterController.h"


USING_NS_CC;
namespace Utility
{
    namespace Social
    {
        namespace GameCenter
        {
            
            bool IsGameCenterInit= false;
            
            GameCenterController::GameCenterController()
            {
                
            }
            
            GameCenterController::~GameCenterController()
            {

            }
            
            void GameCenterController::PostScoreToGameCenter(int score , RKString id_leaderboard)
            {
                if(!IsGameCenterInit)
                    return ;
                
                NSString * str_id = [NSString stringWithUTF8String:id_leaderboard.GetString()];
                
                [[GCHelper defaultHelper] reportScore:score forLeaderboardID:str_id];
            }
            
            void GameCenterController::ShowLeaderboard(RKString id_leaderboard)
            {
                if(!IsGameCenterInit)
                    return ;
                
                UIViewController * myUiViewController = [[[UIViewController alloc]init]autorelease];
                
                NSString * str_id = [NSString stringWithUTF8String:id_leaderboard.GetString()];
                
                [[GCHelper defaultHelper] showLeaderboardOnViewController : myUiViewController withLeaderboardID:str_id];
                

            }
            
            void GameCenterController::authenticationRequired()
            {
                //if the game is open, it should be paused
                
            }
            
            void GameCenterController::Init()
            {
                UIViewController * myViewController = [[UIViewController alloc]init];
                //[[GCHelper defaultHelper] authenticateLocalUserOnViewController :myViewController];
                [[GCHelper defaultHelper] authenticateLocalPlayer];
                [GCHelper setGameViewControoler:myViewController];
                
                IsGameCenterInit = true;
            }
            
            bool GameCenterController::IsGameCenterOpen()
            {
                if(!IsGameCenterInit)
                    return false;
                
                return [[GCHelper defaultHelper] IsGameCenterOpen];
            }
            
            bool GameCenterController::IsGameCenterAuthenticate()
            {
                if(!IsGameCenterInit)
                    return false;
                
                return [[GCHelper defaultHelper]IsUserAuthenticate];
            }
            
            RKString GameCenterController::GetGameCenterName()
            {
                if(!IsGameCenterInit)
                    return "";
                
                const NSString * name_gamecenter = [[GCHelper defaultHelper]GetGameCenterName];
                if(name_gamecenter != nil)
                {
                    RKString name_return = [name_gamecenter UTF8String];
                    return name_return;
                }
                return "";
            }
            
            int GameCenterController::GetRankLeaderboard(RKString id_leaderboard)
            {
                if(!IsGameCenterInit)
                    return -1;
                
                NSString * str_id = [NSString stringWithUTF8String:id_leaderboard.GetString()];
                int rank = [[GCHelper defaultHelper]GetRankInLeaderboard:str_id];
                return rank;
            }
            
            int GameCenterController::GetBestScoreLeaderboard(RKString id_leaderboard)
            {
                if(!IsGameCenterInit)
                    return -1;
                
                NSString * str_id = [NSString stringWithUTF8String:id_leaderboard.GetString()];
                int score = [[GCHelper defaultHelper]GetBestScoreInLeaderboard:str_id];
                return score;
            }
            
            bool GameCenterController::IsLoadleaderboardInfoSucceed()
            {
                if(!IsGameCenterInit)
                    return false;
                
                return [[GCHelper defaultHelper]IsLoadInfoLeaderboardSucceed];
            }
            
            void GameCenterController::ShowGameCenterScheme()
            {
                if(!IsGameCenterInit)
                    return ;
                
                [[GCHelper defaultHelper]showGameCenter];
            }
        }
    }
}