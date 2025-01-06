//fuzzy_function.cpp

#include "fuzzy_function.h"

//Fuzzy Set Input pH
float fsipH_kecil[4] = {0, 0, 0.5, 2};
float fsipH_sedang[3] = {0.5, 2, 3.5};
float fsipH_besar[4] = {2.5, 4.5, 5, 5};
//Fuzzy Set Output pH
float fsopH_sebentar[3] = {0, 0, 3};
float fsopH_sedang[3] = {2, 5, 9};
float fsopH_lama[3] = {6, 10, 10};

//Fuzzy Set Input PPM
float fsiPPM_kecil[4] = {0, 0, 50, 200};
float fsiPPM_sedang[3] = {50, 250, 350};
float fsiPPM_besar[4] = {250, 450, 500, 500};
//Fuzzy Set Output PPM
float fsoPPM_sebentar[3] = {0, 0, 4.5};
float fsoPPM_sedang[3] = {3, 7.5, 13.5};
float fsoPPM_lama[3] = {9, 15, 15};

float A, B, lamaPompa_pH, lamaPompa_PPM;;
int sel_;
float a1, b1a, b1b, c1, a2, b2a, b2b, c2;
float AreapH1, AreapH2, AreapH3, AreapH4, AreapH5, AreapH6, AreapH7;
float MomenpH1, MomenpH2, MomenpH3, MomenpH4, MomenpH5, MomenpH6, MomenpH7;
float AreaPPM1, AreaPPM2, AreaPPM3, AreaPPM4, AreaPPM5, AreaPPM6, AreaPPM7;
float MomenPPM1, MomenPPM2, MomenPPM3, MomenPPM4, MomenPPM5, MomenPPM6, MomenPPM7;
float mvipH_kecil, mvipH_sedang, mvipH_besar, mvopH_sebentar, mvopH_sedang, mvopH_lama;
float mviPPM_kecil, mviPPM_sedang, mviPPM_besar, mvoPPM_sebentar, mvoPPM_sedang, mvoPPM_lama;
float Error_pH, outputFuzz_pH, Error_PPM, outputFuzz_PPM;


float mfipH_kecil(){
    if (Error_pH < fsipH_kecil[2]){mvipH_kecil = 1;}
    else if (Error_pH >= fsipH_kecil[2] && Error_pH <= fsipH_kecil[3]){mvipH_kecil = (fsipH_kecil[3] - Error_pH)/(fsipH_kecil[3]-fsipH_kecil[2]);}
    else if (Error_pH > fsipH_kecil[3]){mvipH_kecil = 0;}
    return mvipH_kecil;
}

float mfipH_sedang(){
    if (Error_pH < fsipH_sedang[0]){mvipH_sedang = 0;}
    else if (Error_pH >= fsipH_sedang[0] && Error_pH <= fsipH_sedang[1]){mvipH_sedang = (Error_pH - fsipH_sedang[0])/(fsipH_sedang[1]-fsipH_sedang[0]);}
    else if (Error_pH >= fsipH_sedang[1] && Error_pH <= fsipH_sedang[2]){mvipH_sedang = (fsipH_sedang[2] - Error_pH)/(fsipH_sedang[2]-fsipH_sedang[1]);}
    else if (Error_pH > fsipH_sedang[2]){mvipH_sedang = 0;}
    return mvipH_sedang;
}

float mfipH_besar(){
    if (Error_pH < fsipH_besar[0]){mvipH_besar = 0;}
    else if (Error_pH >= fsipH_besar[0] && Error_pH <= fsipH_besar[1]){mvipH_besar = (Error_pH - fsipH_besar[0])/(fsipH_besar[1]-fsipH_besar[0]);}
    else if (Error_pH > fsipH_besar[1]){mvipH_besar = 1;}
    return mvipH_besar;
}

float mfopH_sebentar(){
    if (outputFuzz_pH < fsopH_sebentar[1]){mvopH_sebentar = 1;}
    else if (outputFuzz_pH >= fsopH_sebentar[1] && outputFuzz_pH <= fsopH_sebentar[2]){mvopH_sebentar = (fsopH_sebentar[2] - outputFuzz_pH)/(fsopH_sebentar[2]-fsopH_sebentar[1]);}
    else if (outputFuzz_pH > fsopH_sebentar[2]){mvopH_sebentar = 0;}
    return mvopH_sebentar;
}

float mfopH_sedang(){
    if (outputFuzz_pH < fsopH_sedang[0]){mvopH_sedang = 0;}
    else if (outputFuzz_pH >= fsopH_sedang[0] && outputFuzz_pH <= fsopH_sedang[1]){mvopH_sedang = (outputFuzz_pH - fsopH_sedang[0])/(fsopH_sedang[1]-fsopH_sedang[0]);}
    else if (outputFuzz_pH >= fsopH_sedang[1] && outputFuzz_pH <= fsopH_sedang[2]){mvopH_sedang = (fsopH_sedang[2] - outputFuzz_pH)/(fsopH_sedang[2]-fsopH_sedang[1]);}
    else if (outputFuzz_pH > fsopH_sedang[2]){mvopH_sedang = 0;}
    return mvopH_sedang;
}

float mfopH_lama(){
    if (outputFuzz_pH < fsopH_lama[0]){mvopH_lama = 0;}
    else if (outputFuzz_pH >= fsopH_lama[0] && outputFuzz_pH <= fsopH_lama[1]){mvopH_lama = (outputFuzz_pH - fsopH_lama[0])/(fsopH_lama[1]-fsopH_lama[0]);}
    else if (outputFuzz_pH > fsopH_lama[1]){mvopH_lama = 1;}
    return mvopH_lama;
}

float mfiPPM_kecil(){
    if (Error_PPM < fsiPPM_kecil[2]){mviPPM_kecil = 1;}
    else if (Error_PPM >= fsiPPM_kecil[2] && Error_PPM <= fsiPPM_kecil[3]){mviPPM_kecil = (fsiPPM_kecil[3] - Error_PPM)/(fsiPPM_kecil[3]-fsiPPM_kecil[2]);}
    else if (Error_PPM > fsiPPM_kecil[3]){mviPPM_kecil = 0;}
    return mviPPM_kecil;
}

float mfiPPM_sedang(){
    if (Error_PPM < fsiPPM_sedang[0]){mviPPM_sedang = 0;}
    else if (Error_PPM >= fsiPPM_sedang[0] && Error_PPM <= fsiPPM_sedang[1]){mviPPM_sedang = (Error_PPM - fsiPPM_sedang[0])/(fsiPPM_sedang[1]-fsiPPM_sedang[0]);}
    else if (Error_PPM >= fsiPPM_sedang[1] && Error_PPM <= fsiPPM_sedang[2]){mviPPM_sedang = (fsiPPM_sedang[2] - Error_PPM)/(fsiPPM_sedang[2]-fsiPPM_sedang[1]);}
    else if (Error_PPM > fsiPPM_sedang[2]){mviPPM_sedang = 0;}
    return mviPPM_sedang;
}

float mfiPPM_besar(){
    if (Error_PPM < fsiPPM_besar[0]){mviPPM_besar = 0;}
    else if (Error_PPM >= fsiPPM_besar[0] && Error_PPM <= fsiPPM_besar[1]){mviPPM_besar = (Error_PPM - fsiPPM_besar[0])/(fsiPPM_besar[1]-fsiPPM_besar[0]);}
    else if (Error_PPM > fsiPPM_besar[1]){mviPPM_besar = 1;}
    return mviPPM_besar;
}

float mfoPPM_sebentar(){
    if (outputFuzz_PPM < fsoPPM_sebentar[1]){mvoPPM_sebentar = 1;}
    else if (outputFuzz_PPM >= fsoPPM_sebentar[1] && outputFuzz_PPM <= fsoPPM_sebentar[2]){mvoPPM_sebentar = (fsoPPM_sebentar[2] - outputFuzz_PPM)/(fsoPPM_sebentar[2]-fsoPPM_sebentar[1]);}
    else if (outputFuzz_PPM > fsoPPM_sebentar[2]){mvoPPM_sebentar = 0;}
    return mvoPPM_sebentar;
}

float mfoPPM_sedang(){
    if (outputFuzz_PPM < fsoPPM_sedang[0]){mvoPPM_sedang = 0;}
    else if (outputFuzz_PPM >= fsoPPM_sedang[0] && outputFuzz_PPM <= fsoPPM_sedang[1]){mvoPPM_sedang = (outputFuzz_PPM - fsoPPM_sedang[0])/(fsoPPM_sedang[1]-fsoPPM_sedang[0]);}
    else if (outputFuzz_PPM >= fsoPPM_sedang[1] && outputFuzz_PPM <= fsoPPM_sedang[2]){mvoPPM_sedang = (fsoPPM_sedang[2] - outputFuzz_PPM)/(fsoPPM_sedang[2]-fsoPPM_sedang[1]);}
    else if (outputFuzz_PPM > fsoPPM_sedang[2]){mvoPPM_sedang = 0;}
    return mvoPPM_sedang;
}

float mfoPPM_lama(){
    if (outputFuzz_PPM < fsoPPM_lama[0]){mvoPPM_lama = 0;}
    else if (outputFuzz_PPM >= fsoPPM_lama[0] && outputFuzz_PPM <= fsoPPM_lama[1]){mvoPPM_lama = (outputFuzz_PPM - fsoPPM_lama[0])/(fsoPPM_lama[1]-fsoPPM_lama[0]);}
    else if (outputFuzz_PPM > fsoPPM_lama[1]){mvoPPM_lama = 1;}
    return mvoPPM_lama;
}

void implikasi (){
    a1 = fsopH_sebentar[2] - (mfipH_kecil() * (fsopH_sebentar[2] - fsopH_sebentar[1]));
    b1a = fsopH_sedang[0] + (mfipH_sedang() * (fsopH_sedang[1] - fsopH_sedang[0]));
    b1b =  fsopH_sedang[2] - (mfipH_sedang() * (fsopH_sedang[2] - fsopH_sedang[1]));
    c1 = fsopH_lama[0] + (mfipH_besar() * (fsopH_lama[1] - fsopH_lama[0]));

    a2 = fsoPPM_sebentar[2] - (mfiPPM_kecil() * (fsoPPM_sebentar[2] - fsoPPM_sebentar[1]));
    b2a = fsoPPM_sedang[0] + (mfiPPM_sedang() * (fsoPPM_sedang[1] - fsoPPM_sedang[0]));
    b2b =  fsoPPM_sedang[2] - (mfiPPM_sedang() * (fsoPPM_sedang[2] - fsoPPM_sedang[1]));
    c2 = fsoPPM_lama[0] + (mfiPPM_besar() * (fsoPPM_lama[1] - fsoPPM_lama[0]));
}

void luas_deffuzzy(){
    implikasi ();

    AreapH1 = ((fsopH_sebentar[2] - a1) * mfipH_kecil()) / 2;
    AreapH2 = (a1 - fsopH_sebentar[0]) * mfipH_kecil();
    AreapH3 = ((b1a - fsopH_sedang[0])* mfipH_sedang()) / 2;
    AreapH4 = ((fsopH_sedang[2] - b1b)* mfipH_sedang()) / 2;
    AreapH5 = (b1b - b1a)*mfipH_sedang();
    AreapH6 = ((c1 - fsopH_lama[0]) * mfipH_besar()) / 2;
    AreapH7 = (fsopH_lama[2] - c1)* mfipH_besar();

    AreaPPM1 = ((fsoPPM_sebentar[2] - a2) * mfiPPM_kecil()) / 2;
    AreaPPM2 = (a2 - fsoPPM_sebentar[0]) * mfiPPM_kecil();
    AreaPPM3 = ((b2a - fsoPPM_sedang[0])* mfiPPM_sedang()) / 2;
    AreaPPM4 = ((fsoPPM_sedang[2] - b2b)* mfiPPM_sedang()) / 2;
    AreaPPM5 = (b2b - b2a)*mfiPPM_sedang();
    AreaPPM6 = ((c2 - fsoPPM_lama[0]) * mfiPPM_besar()) / 2;
    AreaPPM7 = (fsoPPM_lama[2] - c2)* mfiPPM_besar();
}

float f(float x){

    if (B > 0 && sel_ == 0){
        return ((x-A)/B)*x;
    }
    else if (B > 0 && sel_ == 1){
        return ((A-x)/B)*x;
    }
    else {
        return A*x;
    }
}

float simpsons(float f(float x), float a,float b,float n){
    float h,integral,x,sum=0;
    int i;
    h=fabs(b-a)/n;
    for(i=1;i<n;i++){
      x=a+i*h;
      if(i%2==0){
        sum=sum+2*f(x);
      }
      else{
        sum=sum+4*f(x);
      }
    }
    integral=(h/3)*(f(a)+f(b)+sum);
    return integral;
}

float fx(float limd, float limu, float a, float b, int sel){
    int n,i=2;
    float h,x,integral,eps=0.1,integral_new;
    A = a;
    B = b;
    sel_ = sel;

    integral_new=simpsons(f,limd,limu,i);

    do{
        integral=integral_new;
        i=i+2;
        integral_new=simpsons(f,limd,limu,i);
    }while(fabs(integral_new-integral)>=eps);
    return integral_new;
}

void Momen(){
    luas_deffuzzy();

    MomenpH1 = fx(a1, fsopH_sebentar[2], fsopH_sebentar[2], (fsopH_sebentar[2]-fsopH_sebentar[0]), 1);
    MomenpH2 = fx(fsopH_sebentar[0], a1, mfipH_kecil(), 0, 0);
    MomenpH3 = fx(fsopH_sedang[0], b1a, fsopH_sedang[0], (fsopH_sedang[1] - fsopH_sedang[0]), 0);
    MomenpH4 = fx (b1b, fsopH_sedang[2], fsopH_sedang[2], (fsopH_sedang[2]-fsopH_sedang[1]), 1);
    MomenpH5 = fx (b1a, b1b, mfipH_sedang(), 0, 0);
    MomenpH6 = fx(fsopH_lama[0], c1, fsopH_lama[0], (fsopH_lama[2]-fsopH_lama[0]), 0);
    MomenpH7 = fx(c1, fsopH_lama[2], mfipH_besar(), 0, 0);

    MomenPPM1 = fx(a2, fsoPPM_sebentar[2], fsoPPM_sebentar[2], (fsoPPM_sebentar[2]-fsoPPM_sebentar[0]), 1);
    MomenPPM2 = fx(fsoPPM_sebentar[0], a2, mfiPPM_kecil(), 0, 0);
    MomenPPM3 = fx(fsoPPM_sedang[0], b2a, fsoPPM_sedang[0], (fsoPPM_sedang[1] - fsoPPM_sedang[0]), 0);
    MomenPPM4 = fx (b2b, fsoPPM_sedang[2], fsoPPM_sedang[2], (fsoPPM_sedang[2]-fsoPPM_sedang[1]), 1);
    MomenPPM5 = fx (b2a, b2b, mfiPPM_sedang(), 0, 0);
    MomenPPM6 = fx(fsoPPM_lama[0], c2, fsoPPM_lama[0], (fsoPPM_lama[2]-fsoPPM_lama[0]), 0);
    MomenPPM7 = fx(c2, fsoPPM_lama[2], mfiPPM_besar(), 0, 0);
}

float deffuzzyfikasi_pH(){
    return (MomenpH1+MomenpH2+MomenpH3+MomenpH4+MomenpH5+MomenpH6+MomenpH7)/(AreapH1+AreapH2+AreapH3+AreapH4+AreapH5+AreapH6+AreapH7);
}

float deffuzzyfikasi_PPM(){
    return (MomenPPM1+MomenPPM2+MomenPPM3+MomenPPM4+MomenPPM5+MomenPPM6+MomenPPM7)/(AreaPPM1+AreaPPM2+AreaPPM3+AreaPPM4+AreaPPM5+AreaPPM6+AreaPPM7);
}
