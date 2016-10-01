// 
// ARKManager.mm
// ...
// Liam de Koster-Kjaer
// February 2012
// 

#include "../Include/ARKManager.h"
#include "../RKFile.h"

#import "CoreFoundation/CFBundle.h"
#import "CoreFoundation/CFArray.h"
#import "CoreFoundation/CFURL.h"

#import <Foundation/NSString.h>
#import <Foundation/NSFileManager.h>

/**
* GenerateARKList_iOS
* Liam de Koster-Kjaer
**/
namespace RKUtils
{

bool ArkManager::GenerateArkList_iOS(FilenameList* ARKFilenames)
{
	bool bSuccess = true;
  
	RKString* pFilename = 0;
  
// Files in bundle ------------------------------------------------------------------------
	CFBundleRef cfMainBundle = CFBundleGetMainBundle();

    //cfMainBundle += @"Data/" ;
    
	CFArrayRef ARKUrls = CFBundleCopyResourceURLsOfType(cfMainBundle, CFSTR("ark"), CFSTR(""));
	char buffer[RKSTRING_LENGTH_TEMP];
	for (CFIndex i = 0; i < CFArrayGetCount(ARKUrls); ++i)
	{
		CFURLRef URL = (CFURLRef)CFArrayGetValueAtIndex(ARKUrls, i);
    memset(buffer, 0L, RKSTRING_LENGTH_TEMP);
		if (CFURLGetFileSystemRepresentation(URL, true, (uint8*)buffer, RKSTRING_LENGTH_TEMP))
		{
			pFilename = RKNEW(RKString(buffer));
			//RKString_ExtractFileName(str.GetString(), pFilename->GetString());
			ARKFilenames->Append(pFilename);
		}
		else
		{
			RKDELETE(pFilename);
			bSuccess = false; // Failed to get ArkFilepath
			break;
		}
	}
    
	[ARKUrls release];
    
  // [Liam] No longer adding arks from the support folder. This is so we only load the startup arks from the bundle.
    // Arks from the support folder will be loaded later after the DLCManager has a manifest because we only want to 
    // load the files on the manifest.

// Files in application support folder --------------------------------------------------------------
	// http://stackoverflow.com/questions/7440662/how-to-get-all-paths-for-files-in-documents-directory
    
	//const char* pDocsPath = RKFile_GetSupportFilesPath();
	//NSString* path = [NSString stringWithUTF8String:pDocsPath];

	//NSArray* filePathsArray = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:path error:nil];
	//int iNumFiles = (int)[filePathsArray count];
	//for (int i = 0; i < iNumFiles; ++i)
	//{
		// Full file path
//	NSString* filePath = [path stringByAppendingPathComponent:[filePathsArray objectAtIndex:i]];
//	RKNSLOG(@"\nFile %d: %@", i, filePath);

		// Filename
//	RKNSLOG(@"\nFile %d: %@", i, [filePathsArray objectAtIndex:i]);

	  //NSString* filename = [path stringByAppendingPathComponent:[filePathsArray objectAtIndex:i]];
//		NSString* filename = [filePathsArray objectAtIndex:i];
		//NSString* ext = [filename pathExtension];
		//NSComparisonResult comparison = [ext compare: @"ark"];

		//if (comparison == NSOrderedSame)
		//{
			//RKString str([filename cStringUsingEncoding:NSASCIIStringEncoding]);
			//pFilename = new RKString();
			//pFilename = new RKString(str.GetString());
			//RKString_ExtractFileName(str.GetString(), pFilename->GetString());
			//ARKFilenames->Append(pFilename);
		//}
	//}
  
	ARKFilenames->Sort(ArkFilenameSort);

	return bSuccess;
}
}
