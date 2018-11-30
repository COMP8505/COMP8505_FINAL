
#include "backdoor_channel.h"


int main(){
    Backdoor_Channel ch;
    ch.start("lo", 8000, 9000);
}
