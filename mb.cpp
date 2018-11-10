#define STAT_AVAIL 0
#define STAT_USED  1

#include <cstdio>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

using namespace std;

int is_verbose = 0;

vector<string> listdir(string path, bool is_relative=false, string _basepath="");

int mkdirs(string dstdir, vector<string> srcdirs){
    struct stat sb;
    for(auto i=srcdirs.begin(); i != srcdirs.end(); i++){
        vector<string> srctree = listdir(*i, true);
        for(auto j=srctree.begin(); j != srctree.end(); j++){
            string dstbranch = dstdir + string("/") + *j;
            if(stat(dstbranch.c_str(), &sb) == 0){
                // directory exists, move on
                log("%s exists, moving on...\n", dstbranch.c_str());
                continue;
            }
            else{
                // directory does not exist, create directory
                log("%s does not exist, creating.\n", dstbranch.c_str());
                mkdir(dstbranch.c_str(), S_IRWXU|S_IRGRP|S_IROTH);
            }
        }
    }

    return 0;
}

vector<string> listdir(string path, bool is_relative, string _basepath){
    dirent* entry;
    vector<string> dirs;
    DIR* dp;
    string basepath;

    if(is_relative){
        basepath = split(path).back();
        if(!_basepath.empty())
            basepath = _basepath + string("/") + basepath;
        else
            dirs.push_back(basepath);
    }

    log("Entering %s...\n", path.c_str());

    if((dp = opendir(path.c_str())) == NULL){
        printf("opendir error\n");
        return dirs;
    }

    while((entry = readdir(dp))){
        if(string(entry->d_name).compare("..") == 0 ||
                string(entry->d_name).compare(".") == 0)
            continue;
        if(entry->d_type == DT_DIR){
            string dirpath = path + string("/") + entry->d_name;
            if(is_relative)
                dirs.push_back(basepath + string("/") + entry->d_name);
            else
                dirs.push_back(dirpath);
            vector<string> tmpdirs = listdir(dirpath, is_relative, basepath);
            for(auto i=tmpdirs.begin(); i != tmpdirs.end(); i++){
                string newpath = *i;
                if(is_relative){
                    vector<string> splitpath = split(newpath);
                    vector<string> newpath_v;
                    bool reached = false;
                    for(int j=0; j < splitpath.size(); j++){
                        if(splitpath.at(j).compare(split(basepath).front()) == 0)
                            reached = true;
                        if(reached)
                            newpath_v.push_back(splitpath.at(j));
                    }
                    newpath = join(newpath_v, false);
                }
                dirs.push_back(newpath);
            }
        }
    }

    closedir(dp);
    return dirs;
}

unsigned long get_disk_stat(string path, int type){
    struct statvfs vfs;
    unsigned long nblocks;
    unsigned long blocks_size;
    unsigned long free_blocks;
    unsigned long avail_blocks;
    unsigned long used_blocks;
    if(statvfs(path.c_str(), &vfs) != 0){
        return 1L;
    }

    nblocks = vfs.f_blocks;
    blocks_size = vfs.f_bsize;
    free_blocks = vfs.f_bfree;
    avail_blocks = vfs.f_bavail;
    used_blocks = nblocks-free_blocks;

    switch(type){
        case STAT_AVAIL:
            return avail_blocks*blocks_size;
        case STAT_USED:
            return used_blocks*blocks_size;
    }

    return 1L;
}

int main(int argc, char* argv[]){
    Args args;
    vector<string> src_paths;
    string dst_path;
    unsigned long dst_avail_space;
    unsigned long src_used_space;

    if(argc < 3){
        printf("Usage: %s [-v] dstPath srcPath [srcPath ...]\n", argv[0]);
        return 0;
    }

    args = get_args(argc, argv);
    is_verbose = args.is_verbose;
    // get backup dst path
    dst_path = args.dst_path;
    // get backup src paths
    src_paths = args.src_paths;
    // get total avail size of backup dst
    dst_avail_space = get_disk_stat(dst_path, STAT_AVAIL);
    // get total used size of backup src
    src_used_space = 0L;
    for(auto i=src_paths.begin(); i != src_paths.end(); i++){
        src_used_space += get_disk_stat(*i, STAT_USED);
    }

    mkdirs(dst_path, src_paths);

    return 0;
}
