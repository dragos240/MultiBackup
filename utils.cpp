#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>

#include <string>
#include <vector>

#include "utils.h"

using namespace std;

extern int is_verbose;

Args get_args(int argc, char** argv){
    int is_verbose = 0;
    vector<string> src_paths;
    string dst_path;
    int c;

    while((c = getopt(argc, argv, "v")) != -1){
        switch(c){
            case 'v':
                is_verbose = 1;
                break;
            default:
                abort();
        }
    }

    for(int i=optind; i < argc; i++){
        if(i==optind)
            dst_path = argv[i];
        else
            src_paths.push_back(argv[i]);
    }

    Args args = {is_verbose, dst_path, src_paths};

    return args;
}

vector<string> split(string s){
    vector<string> result;

    size_t pos = 0;
    string token;
    while((pos = s.find("/")) != string::npos){
        token = s.substr(0, pos);
        if(token.length() > 0)
            result.push_back(token);
        s.erase(0, pos+1);
    }
    if(s.length() > 0)
        result.push_back(s);

    return result;
}

string join(vector<string> v, bool is_absolute){
    string s = "";
    for(auto i=v.begin(); i != v.end(); i++){
        if(i == v.begin() && !is_absolute){
            s += *i;
            continue;
        }
        s += string("/") + *i;
    }

    return s;
}

void log(const char* format_string, ...){
    if(is_verbose == 1){
        va_list ap;
        va_start(ap, format_string);
        vprintf(format_string, ap);
        va_end(ap);
    }
}
