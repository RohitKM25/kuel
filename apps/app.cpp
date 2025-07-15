#include <kuel/matcher.hpp>

#include <iostream>
// Excel Query language...

// functional
// x = math.pow(12,2)
// z = z + math.random(5)
// module abc begin
//     func setcellvalue(x, y) begin
//         z = math.pow(x,y)
//         return z
//     end func
// end module

// t

int main(int argc, char** argv)
{
    if(argc<3) return 1;
    std::string src = "mod test begin\nfunc get_sum(a,b) begin\nx = a+b\nend func\nend mod\n";
    kuel::matcher::matcher pm(argv[1]);
    auto x = pm(argv[2]);
    std::cout << "PM(" << argv[1] << ", " << argv[2] << ") = " << x << '\n';
    std::cout << std::flush;
    return 0;
}
