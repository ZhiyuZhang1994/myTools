#include <iostream>

class Test{
public:
    Test( int i, int j )
    :mI(i),mJ(j){

    }

    void Print(){
        std::cout <<"i= " << mI <<" j= " <<mJ << std::endl;
    }
private:
    int mI;
    int mJ;
};

int main() {

    return 0;
}