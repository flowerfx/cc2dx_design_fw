#include "FileManager.h"
#include <Foundation/Foundation.h>
#include <vector>

namespace Utility
{
	namespace File
	{
        NSBundle* _bundle = nullptr;
        
        void SetFileUtilsApple() {
            _bundle = [NSBundle mainBundle];
        }
        
        
        void setBundle(NSBundle* bundle) {
            _bundle = bundle;
        }
        
        NSBundle* getBundle() {
            return _bundle;
        }
        
		//
		RKString GetWritablePath_IOS()
		{
			// save to document folder
    		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
   		 	NSString *documentsDirectory = [paths objectAtIndex:0];
    		std::string strRet = [documentsDirectory UTF8String];
    		//strRet.append("/");
    		return strRet;	
		}
        
        RKString GetFullPathResourceFile_IOS(std::string filename)
		{
			NSString* fullpath = [getBundle() pathForResource:[NSString stringWithUTF8String:filename.c_str()]
                                                             ofType:nil];
	        if (fullpath != nil)
            {
	            return [fullpath UTF8String];
	        }	
	        return "";
		}
        
        RKString GetBundlePath_IOS()
        {
            NSArray *files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[[NSBundle mainBundle] bundlePath] error:NULL];
            for (NSString *fileName in files)
            {
                NSString *path = [[NSBundle mainBundle] pathForResource:fileName ofType:nil];
                // do something with `url`
                RKString path_temp =  [path UTF8String];
                auto list_file = path_temp.SplitLast("/");
                path_temp =list_file.GetAt(0);
                
                path_temp+= "/";
                return path_temp;
            }
        }
        
    }
}