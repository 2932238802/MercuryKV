#include<bits/stdc++.h>
using namespace std;
int sushu(int abcf){
    if(abcf==2){
        return 1;
    }
    else{
        for(int i=2;i*i<=abcf;i++){
            if(abcf%i==0){
                return 0;
            }
        }
    }
    return 1;
}
int main(){
    int a,t=0;
    cin>>a;
    for(int i=2;i<=a;i++){
        if(sushu(i)){
            t++;
        }
    }cout<<t;
}