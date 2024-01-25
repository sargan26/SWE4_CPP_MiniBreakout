#include <ml5/ml5.h>

int main () {
   std::cout << "detected os:       " << ml5::util::detected_os_as_string () << "\n"
                "detected compiler: " << ml5::util::detected_compiler_as_string () << "\n"
                "MiniLib version:   " << ml5::version << "\n\n";
}
