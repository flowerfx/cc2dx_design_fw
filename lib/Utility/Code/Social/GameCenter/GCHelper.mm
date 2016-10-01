//
//  GCHelper.m
//  SpaceInvadersGameCenter
//
//  Created by Alvin Stanescu on 21.09.13.
//  Copyright (c) 2013 Barbara Köhler. All rights reserved.
//

#import "GCHelper.h"

#include "cocos2d.h"
USING_NS_CC;

static UIViewController* s_gameViewController = nil;
@interface GCHelper ()

- (void)loadLeaderBoardInfo;

@end

@implementation GCHelper

@synthesize gameCenterAvailable;
@synthesize authVC;
@synthesize localPlayer;
@synthesize m_accessToken;

#pragma mark Initialization

static GCHelper *_sharedHelper = nil;

+ (GCHelper*)defaultHelper {
    // dispatch_once will ensure that the method is only called once (thread-safe)
    static dispatch_once_t pred = 0;
    dispatch_once(&pred, ^{
        _sharedHelper = [[GCHelper alloc] init];
    });
    return _sharedHelper;
}

- (id)init {
    if ((self = [super init])) {
        gameCenterAvailable = [self isGameCenterAvailable];
        
        if (gameCenterAvailable) {
            NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
            [nc addObserver:self selector:@selector(authenticationChanged) name:GKPlayerAuthenticationDidChangeNotificationName object:nil];
        }
    }
    gameCenterOnShow = FALSE;
    userAuthenticated = FALSE;
    GetInfoLeaderboardSucceed = FALSE;
    rank_store.clear();
    return self;
}

+ (void) setGameViewControoler:(UIViewController*)gameViewController
{
    s_gameViewController = gameViewController;
}

- (BOOL)isGameCenterAvailable {
    // check for presence of GKLocalPlayer API
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    
    // check if the device is running iOS 4.1 or later
    NSString *reqSysVer = @"4.1";
    NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer
                                           options:NSNumericSearch] != NSOrderedAscending);
    
    return (gcClass && osVersionSupported);
}

#pragma mark Authentication

- (void)authenticationChanged {
    if ([GKLocalPlayer localPlayer].isAuthenticated && !userAuthenticated) {
        NSLog(@"Authentication changed: player authenticated.");
        userAuthenticated = TRUE;
        
        [self loadLeaderBoardInfo];
        
        //self->GetInfoLeaderboardSucceed = TRUE;
        
       // [self loadAchievements];
        
    } else if (![GKLocalPlayer localPlayer].isAuthenticated && userAuthenticated) {
        NSLog(@"Authentication changed: player not authenticated.");
        userAuthenticated = FALSE;
    }
}

-(void) authenticateLocalPlayer
{
    if (!gameCenterAvailable)
        return;
    
    self.localPlayer = [GKLocalPlayer localPlayer];
    if(!localPlayer)
        return;
    
    if(!userAuthenticated)
    {
        localPlayer.authenticateHandler = ^(UIViewController* vc, NSError* error)
        {
            if(error || !vc)
            {
                if(authVC != nil)
                {
                    [self DismissModalViewController:authVC];
                    authVC = nil;
                }
                
                [error retain];// we're gonna call another thread, so let's keep the value
                dispatch_async(dispatch_get_main_queue(), ^
                               {
                                   [self authenticationChanged];
                                   userAuthenticated = localPlayer.isAuthenticated;
                                   [error release];
                               });
            }
            else if(vc != nil)
            {
                userAuthenticated = localPlayer.isAuthenticated;
                authVC = vc;
                // = YES;
                
                //make sure on iOS 8 the VC follows the proper orientation
                float deviceVersion = [[[UIDevice currentDevice] systemVersion] floatValue];
                if (deviceVersion >= 8.0f)
                {
                    //vc.modalPresentationStyle = UIModalPresentationOverCurrentContext;
                }
                
                //show view controller
                [self PresentModalViewController:vc];
            }
        };
    }
    else
    {
        dispatch_async(dispatch_get_main_queue(), ^
                       {
                           [self authenticationChanged];
                       });
    }
}

- (void)authenticateLocalUserOnViewController:(UIViewController*)viewController
{
    if (!gameCenterAvailable)
        return;
    GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
    
    NSLog(@"Authenticating local user...");
    if (localPlayer.authenticated == NO)
    {
        [localPlayer setAuthenticateHandler:^(UIViewController* authViewController, NSError *error)
        {
            if (authViewController != nil)
            {

                
                [viewController presentViewController:authViewController animated:YES completion:^
                {
                    
                }];
                
                UIWindow *window = [UIApplication sharedApplication].keyWindow;
                [window addSubview:s_gameViewController.view];
            
            }
            else if (error != nil)
            {
                // process error
            }
        }];
    }
    else
    {
        NSLog(@"Already authenticated!");
    }
}

#pragma mark Leaderboards

- (void)loadLeaderBoardInfo
{
    [GKLeaderboard loadLeaderboardsWithCompletionHandler:^(NSArray *leaderboards, NSError *error) {
        self.leaderboards = leaderboards;
        
         for (GKLeaderboard * object_lb in [self leaderboards])
         {
             object_lb.timeScope = GKLeaderboardTimeScopeWeek;
             
             [object_lb loadScoresWithCompletionHandler:^(NSArray * scores, NSError* error)
             {
                 int rank = 0;
                 int score = 0;
                 if (error == nil && scores != nil)
                 {
                    NSLog(@"Player id: %@" ,[GKLocalPlayer localPlayer].playerID);
                     for (GKScore* refScore in scores)
                     {
                        NSLog(@"leaderboard Player id: %@" ,[refScore playerID]);
                         if([[refScore playerID] isEqualToString:[GKLocalPlayer localPlayer].playerID])
                         {
                         //NOTE: Retrieved score array is sorted already.
                             rank = (int)[refScore rank];
                             score = (int)[refScore value];
                             break;
                         //only get the first
                         }
                     }
                     //show the rank to player. using delegate, notification, popups etc...
                 }
                 else
                 {
                     //handle errors
                 }
                 self->rank_store.insert(std::pair<NSString*, score_detail>([object_lb identifier] , score_detail(rank , score)));
                 
                 int count_leaderboard = (int)[self.leaderboards count];
                 
                 if((int)self->rank_store.size() == count_leaderboard )
                 {
                     self->GetInfoLeaderboardSucceed = TRUE;
                 }
             }];
         }
    
    }];
}

- (void)reportScore:(int64_t)score forLeaderboardID:(NSString*)identifier
{
    GKScore *scoreReporter = [[GKScore alloc] initWithLeaderboardIdentifier: identifier];
    scoreReporter.value = score;
    scoreReporter.context = 0;
    
    [GKScore reportScores:@[scoreReporter] withCompletionHandler:^(NSError *error) {
        if (error == nil) {
            NSLog(@"Score reported successfully!");
        } else {
            NSLog(@"Unable to report score!");
        }
    }];
}

- (void)showLeaderboardOnViewController:(UIViewController*)viewController withLeaderboardID:(NSString*)identifier;
{
    if(!userAuthenticated)
        return;
    
    GKGameCenterViewController *gameCenterController = [[[GKGameCenterViewController alloc] init] autorelease];
    if (gameCenterController != nil)
    {
        
        gameCenterController.gameCenterDelegate = self;
        gameCenterController.viewState = GKGameCenterViewControllerStateLeaderboards;
        gameCenterController.leaderboardIdentifier = identifier;
        gameCenterController.leaderboardTimeScope = GKLeaderboardTimeScopeAllTime;
        
        [self PresentModalViewController:gameCenterController];
        
        gameCenterOnShow = true;
    }
}

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    if(gameCenterViewController != nil && gameCenterViewController.view != nil)
    {
        [self DismissModalViewController:gameCenterViewController];
    }
    
    gameCenterOnShow = false;
}

#pragma mark Achievements

- (void)reportAchievementIdentifier: (NSString*) identifier percentComplete: (float) percent
{
    GKAchievement *achievement = [self getAchievementForIdentifier:identifier];
    if (achievement && achievement.percentComplete != 100.0) {
        achievement.percentComplete = percent;
        achievement.showsCompletionBanner = YES;
        
        [GKAchievement reportAchievements:@[achievement] withCompletionHandler:^(NSError *error) {
            if (error != nil) {
                NSLog(@"Error while reporting achievement: %@", error.description);
            }
        }];
    }
}

- (void)loadAchievements
{
    self.achievementsDictionary = [[NSMutableDictionary alloc] init];
    [GKAchievement loadAchievementsWithCompletionHandler:^(NSArray *achievements, NSError *error) {
        if (error != nil) {
            // Handle the error.
            NSLog(@"Error while loading achievements: %@", error.description);
        }
        else if (achievements != nil) {
            // Process the array of achievements.
            for (GKAchievement* achievement in achievements)
                self.achievementsDictionary[achievement.identifier] = achievement;
        }
    }];
}

- (GKAchievement*)getAchievementForIdentifier: (NSString*) identifier
{
    GKAchievement *achievement = [self.achievementsDictionary objectForKey:identifier];
    if (achievement == nil) {
        achievement = [[GKAchievement alloc] initWithIdentifier:identifier];
        self.achievementsDictionary[achievement.identifier] = achievement;
    }
    return achievement;
}

- (void)resetAchievements
{
    // Clear all locally saved achievement objects.
    self.achievementsDictionary = [[NSMutableDictionary alloc] init];
    // Clear all progress saved on Game Center.
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError *error)
     {
         if (error != nil) {
             // handle the error.
             NSLog(@"Error while reseting achievements: %@", error.description);
             
         }
     }];
}

- (void)completeMultipleAchievements:(NSArray*)achievements
{
    [GKAchievement reportAchievements:achievements withCompletionHandler:^(NSError *error) {
        if (error != nil) {
            NSLog(@"Error while reporting achievements: %@", error.description);
        }
    }];
}

#pragma mark Challenges

- (void)registerListener:(id<GKLocalPlayerListener>)listener
{
    [[GKLocalPlayer localPlayer] registerListener:listener];
}

- (BOOL)IsGameCenterOpen
{
    return gameCenterOnShow;
}

- (BOOL)IsUserAuthenticate
{
    return userAuthenticated;
}

- (BOOL)IsLoadInfoLeaderboardSucceed
{
    return GetInfoLeaderboardSucceed;
}

- (NSString* const) GetGameCenterName
{
    if(userAuthenticated)
    {
        GKLocalPlayer *lp = [GKLocalPlayer localPlayer];
        return lp.alias;
    }
    return nil;
}

- (int const) GetRankInLeaderboard:(NSString *)id_leaderboard
{
    if(userAuthenticated && GetInfoLeaderboardSucceed)
    {
        if(rank_store.size() > 0)
        {
            for (auto it = rank_store.begin(); it != rank_store.end(); it ++) {
                if([it->first isEqualToString:id_leaderboard ])
                {
                    return it->second.rank;
                }
            }
        }
        
        return 0;
    }
    return -1;
}

- (int const) GetBestScoreInLeaderboard:(NSString *)id_leaderboard
{
    if(userAuthenticated && GetInfoLeaderboardSucceed)
    {
        if(rank_store.size() > 0)
        {
            for (auto it = rank_store.begin(); it != rank_store.end(); it ++) {
                if([it->first isEqualToString:id_leaderboard ])
                {
                    return it->second.score;
                }
            }
        }
        
        return 0;
    }
    return -1;
}

- (void)showGameCenter
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"gamecenter:"]];
}

//use when you want to present modally a view controller
-(void) PresentModalViewController:(UIViewController*)viewController
{
    if(s_gameViewController && viewController)
    {
        [s_gameViewController presentViewController:viewController animated:TRUE completion:nil];
        
        UIWindow *window = [UIApplication sharedApplication].keyWindow;
        [window addSubview:s_gameViewController.view];
    }
}
-(void) DismissModalViewController:(UIViewController*)viewController
{
    if(s_gameViewController && viewController)
    {
        [s_gameViewController dismissViewControllerAnimated:YES completion:nil];
        
        [s_gameViewController.view removeFromSuperview];
    }
}

@end
