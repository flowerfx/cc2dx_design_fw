//
//  NetworkState.h
//  ZooRescue
//
//  Created by Ray Zhang on 19/07/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Reachability.h"

@interface NetworkState : NSObject
{
  
  Reachability * _hostReach;
	Reachability * _internetReach;
	Reachability * _localWiFiReach;
  
  NetworkStatus _statusInternetReach;
  NetworkStatus _statusWiFiReach;
}

@property (nonatomic, readonly) BOOL isHostReachable;
@property (nonatomic, readonly) BOOL isHostReachableViaWiFi;
@property (nonatomic, readonly) BOOL isHostReachableViaWWAN;
@property (nonatomic, readonly) BOOL isInternetAvailable;
@property (nonatomic, readonly) BOOL isInternetViaWiFi;
@property (nonatomic, readonly) BOOL isInternetViaWWAN;
@property (nonatomic, readonly) BOOL isWiFiAvailable;


- (id)init;

@end
