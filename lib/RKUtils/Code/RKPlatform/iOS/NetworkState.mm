//
//  NetworkState.m
//  ZooRescue
//
//  Created by Ray Zhang on 19/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "NetworkState.h"
#include "RKNSLog.h"

@interface NetworkState ()
- (void)networkStatusChanged:(NSNotification *)notification;
- (void)updateByReachability:(Reachability *)reachability;
@end

@implementation NetworkState
@dynamic isHostReachable;
@dynamic isHostReachableViaWiFi;
@dynamic isHostReachableViaWWAN;
@dynamic isInternetAvailable;
@dynamic isInternetViaWiFi;
@dynamic isInternetViaWWAN;
@dynamic isWiFiAvailable;

- (id)init
{
  self = [super init];
  if (self)
  {
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(networkStatusChanged:)
                                                 name:kReachabilityChangedNotification
                                               object:nil];
    _hostReach = [[Reachability reachabilityWithHostName:@"www.gameloft.com"] retain];
    [_hostReach startNotifier];
    [self updateByReachability:_hostReach];
    
    _internetReach = [[Reachability reachabilityForInternetConnection] retain];
    [_internetReach startNotifier];
    [self updateByReachability:_internetReach];
    
    _localWiFiReach = [[Reachability reachabilityForLocalWiFi] retain];
    [_localWiFiReach startNotifier];
    [self updateByReachability:_localWiFiReach];
  }
  return self;
}

- (void)dealloc
{
  [_localWiFiReach stopNotifier], [_localWiFiReach release];
  [_internetReach stopNotifier], [_internetReach release];
  [_hostReach stopNotifier], [_hostReach release];
  [[NSNotificationCenter defaultCenter] removeObserver:self];
  
  [super dealloc];
}


- (void)networkStatusChanged:(NSNotification *)notification
{
  Reachability * reachability = [notification object];
	if ([reachability isKindOfClass:[Reachability class]])
  {
    [self updateByReachability:reachability];
  }
  else
  {
    RKNSLOG(@"Error: Not a reachability class: %@", reachability);
  }
}

- (void)updateByReachability:(Reachability *)reachability
{
  if (reachability == _internetReach)
  {
    _statusInternetReach = [reachability currentReachabilityStatus];
  }
  else if (reachability == _localWiFiReach)
  {
    _statusWiFiReach = [reachability currentReachabilityStatus];
  }
  
  RKNSLOG(@"Info: HostViaWiFi:%@, InternetViaWiFi:%@, WiFi:%@, Host:%@, Internet:%@", 
        self.isHostReachableViaWiFi ? @"YES" : @"NO",
        self.isInternetViaWiFi ? @"YES" : @"NO",
        self.isWiFiAvailable ? @"YES" : @"NO",
        self.isHostReachable ? @"YES" : @"NO",
        self.isInternetAvailable ? @"YES" : @"NO"
        );
}

- (BOOL)isHostReachable
{
  return [_hostReach currentReachabilityStatus] != NotReachable;
}

- (BOOL)isHostReachableViaWiFi
{
  return [_hostReach currentReachabilityStatus] == ReachableViaWiFi;
}

- (BOOL)isHostReachableViaWWAN
{
  return [_hostReach currentReachabilityStatus] == ReachableViaWWAN;
}

- (BOOL)isInternetAvailable
{
  return _statusInternetReach != NotReachable;
}

- (BOOL)isInternetViaWiFi
{
  return _statusInternetReach == ReachableViaWiFi;
}

- (BOOL)isInternetViaWWAN
{
  return _statusInternetReach == ReachableViaWWAN;
}

- (BOOL)isWiFiAvailable
{
  return _statusWiFiReach != NotReachable;
}

@end
