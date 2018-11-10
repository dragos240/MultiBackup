#include <string>
#include <vector>

using namespace std;

struct Args{
    int is_verbose;
    string dst_path;
    vector<string> src_paths;
};

Args get_args(int argc, char** argv);

vector<string> split(string s);

string join(vector<string> v, bool is_absolute=true);

void log(const char* format_string, ...);
