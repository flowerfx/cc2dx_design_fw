//
//  ImgProcChecker.cpp
//  ImgProcChecker
//
//  Created by Cristian Sandu on 10/07/14.
//  Copyright (c) 2014 Cristian Sandu. All rights reserved.
//

#include <cstdio>
#include <cstring>
#include <cassert>

#import <dlfcn.h>
#import <mach-o/loader.h>
#include <mach-o/fat.h>
#include <libkern/OSByteOrder.h>

#include <CommonCrypto/CommonDigest.h>

#define TEXT_SEGMENT_NAME       "__TEXT"
#define TEXT_SECTION_NAME       "__text"
#define TEXT_SEGMENT_NAME_SIZE  6
#define UUID_BYTESIZE           16

// We need to know the signature of the main function to get its address
extern int main(int argc, char * argv[]);

// Fwd declaration
void GetTextSegment(const mach_header* header, bool is64bit, uint8_t* uuid, uint64_t& text_addr, uint64_t& text_section_addr, uint64_t& text_section_size);

// The main function uses C-linkage and we need to do the same
extern "C"
{
    /**
     * The caller is responsible for allocating the memory for hash and uuid;
     * hash: CC_SHA256_DIGEST_LENGTH bytes
     * uuid: 16 bytes
     */
    void GetImgCs(uint8_t* hash, uint8_t* uuid)
    {
        const struct mach_header *header;
        Dl_info dlinfo;
        
        // The binary's UUID, could be useful for tracking
        memset(uuid, 0, UUID_BYTESIZE);
        
        // The __TEXT segment's SHA256 hash
        memset(hash, 0, CC_SHA256_DIGEST_LENGTH);
        
        // Fetch the dlinfo for main()
        if (dladdr(reinterpret_cast<void*>(main), &dlinfo) == 0 || dlinfo.dli_fbase == NULL)
        {
            // What? No main? I guess it could happen, maybe for a Swift app?
            return;
        }
        
        // OK, we got a pointer to the Mach-O header
        header = static_cast<mach_header*>(dlinfo.dli_fbase);
        
        // Address and size of the __TEXT segment(we need actually only 32 bits for 32 bit archs)
        uint64_t text_addr = 0;
        uint64_t text_section_addr = 0;
        uint64_t text_section_size = 0;
        
        bool is64bit = false;
        if (OSSwapBigToHostInt32(header->magic) == FAT_MAGIC)
        {
            // Now, the OS should not be as dumb as to load the full fat file into memory but who knows...
            // TODO: just in case, implement this
            /*
            const fat_header* f_header = reinterpret_cast<const fat_header*>(header);
            
            for (unsigned c = OSSwapBigToHostInt32(reinterpret_cast<const fat_header*>(header)->nfat_arch);
                 c != 0; --c)
            {
                const fat_arch* arch = NULL;
            }
            */
        }
        else if (header->magic == MH_MAGIC)
        {
            // 32 bit binary
            GetTextSegment(header, is64bit, uuid, text_addr, text_section_addr, text_section_size);
        }
        else if (header->magic == MH_MAGIC_64)
        {
            // 64 bit binary
            is64bit = true;
            GetTextSegment(header, is64bit, uuid, text_addr, text_section_addr, text_section_size);
        }
        
        // Did we get something useful? If yes, hash that memory
        if (text_addr != 0 && text_section_addr != 0 && text_section_size != 0)
        {
            // Convert VM address to ptr. Lame, I know.
            // Initially I screwed up by not taking the header into coniseration, read the proper solution here:
            // http://applidium.com/en/news/securing_ios_apps_patching_binaries/
            const uint8_t* const text_ptr = reinterpret_cast<const uint8_t* const>((uint64_t)header + text_section_addr - text_addr) ;
            // This returns the hash ptr, no need to check it
            CC_SHA256(text_ptr, static_cast<CC_LONG>(text_section_size), hash);
        }
    }
    
}

void GetTextSegment(const mach_header* header, bool is64bit, uint8_t* uuid, uint64_t& text_addr, uint64_t& text_section_addr, uint64_t& text_section_size)
{
    // 64 bit binary
    
    const mach_header_64* header64 = is64bit ? reinterpret_cast<const mach_header_64*>(header) : NULL;
    
    // Get the address of the first load command
    const load_command* cmd = is64bit ?  reinterpret_cast<const load_command*>(header64 + 1) :
                                         reinterpret_cast<const load_command*>(header + 1);
    
    uint32_t ncmds = is64bit ? header64->ncmds : header->ncmds;
    for (uint32_t i = 0; cmd!= NULL && i < ncmds; i++)
    {
        // Found the UUID, store it
        if (cmd->cmd == LC_UUID)
        {
            const uuid_command* uuid_cmd = reinterpret_cast<const uuid_command*>(cmd);
            memcpy(uuid, uuid_cmd->uuid, sizeof(uuid_cmd->uuid));
            
            if (text_addr != 0  && text_section_addr != 0 && text_section_size != 0)
            {
                // OK, so we got everything we needed from the binary image, time to break out
                break;
            }
        }
        else if(cmd->cmd == LC_SEGMENT || cmd->cmd == LC_SEGMENT_64)
        {
            // Found a load command for a 32 bit segment
            const segment_command*    seg_cmd    = !is64bit  ? reinterpret_cast<const segment_command*>(cmd)    : NULL;
            const segment_command_64* seg64_cmd  = is64bit   ? reinterpret_cast<const segment_command_64*>(cmd) : NULL;
            
            // Is this the text segment?
            const char* segname = is64bit ? seg64_cmd->segname : seg_cmd->segname;
            
            if (strncmp(segname, TEXT_SEGMENT_NAME, TEXT_SEGMENT_NAME_SIZE) == 0)
            {
                text_addr = is64bit ? seg64_cmd->vmaddr : seg_cmd->vmaddr;
                
                section_64* sec64   = is64bit   ? (section_64*) (seg64_cmd + 1) : NULL;
                section*    sec     = !is64bit  ? (section*)    (seg_cmd + 1)   : NULL;
                
                uint32_t nsects = is64bit ? seg64_cmd->nsects : seg_cmd->nsects;
                for (uint32_t j = 0; j < nsects; j++)
                {
                    // Just to be safe
                    if (is64bit && sec64 == NULL)
                    {
                        break;
                    }
                    else if(!is64bit && sec == NULL)
                    {
                        break;
                    }
                    
                    const char* sectname = is64bit ? sec64->sectname : sec->sectname;
                    if (strncmp(sectname, TEXT_SECTION_NAME, TEXT_SEGMENT_NAME_SIZE) == 0)
                    {
                        text_section_addr = is64bit ? sec64->addr   : sec->addr;
                        text_section_size = is64bit ? sec64->size   : sec->size;
                        break;
                    }
                    
                    if (is64bit)
                    {
                        sec64 = (section_64*)(sec64 + 1);
                    }
                    else
                    {
                        sec = (section *)(sec + 1);
                
                    }
                }
            }
        }
        
        // Advance to next load command
        cmd = (load_command*) ((uint8_t*)cmd  + cmd->cmdsize);
    }
}

