//fuzzy_function.h

#ifndef FUZZY_FUNCTION_H
#define FUZZY_FUNCTION_H
#include <Arduino.h>

extern float fsipH_kecil[4] ;
extern float fsipH_sedang[3] ;
extern float fsipH_besar[4] ;

extern float fsopH_sebentar[3] ;
extern float fsopH_sedang[3] ;
extern float fsopH_lama[3] ;

extern float fsiPPM_kecil[4] ;
extern float fsiPPM_sedang[3] ;
extern float fsiPPM_besar[4] ;

extern float fsoPPM_sebentar[3] ;
extern float fsoPPM_sedang[3] ;
extern float fsoPPM_lama[3] ;

extern float A, B, lamaPompa_pH, lamaPompa_PPM;;
extern int sel_;
extern float a1, b1a, b1b, c1, a2, b2a, b2b, c2;
extern float AreapH1, AreapH2, AreapH3, AreapH4, AreapH5, AreapH6, AreapH7;
extern float MomenpH1, MomenpH2, MomenpH3, MomenpH4, MomenpH5, MomenpH6, MomenpH7;
extern float AreaPPM1, AreaPPM2, AreaPPM3, AreaPPM4, AreaPPM5, AreaPPM6, AreaPPM7;
extern float MomenPPM1, MomenPPM2, MomenPPM3, MomenPPM4, MomenPPM5, MomenPPM6, MomenPPM7;
extern float mvipH_kecil, mvipH_sedang, mvipH_besar, mvopH_sebentar, mvopH_sedang, mvopH_lama;
extern float mviPPM_kecil, mviPPM_sedang, mviPPM_besar, mvoPPM_sebentar, mvoPPM_sedang, mvoPPM_lama;
extern float Error_pH, outputFuzz_pH, Error_PPM, outputFuzz_PPM;

float mfipH_kecil();
float mfipH_sedang();
float mfipH_besar();
float mfopH_sebentar();
float mfopH_sedang();
float mfopH_lama();
float mfiPPM_kecil();
float mfiPPM_sedang();
float mfiPPM_besar();
float mfoPPM_sebentar();
float mfoPPM_sedang();
float mfoPPM_lama();
void implikasi ();
void luas_deffuzzy();
float f(float x);
float simpsons(float f(float x), float a,float b,float n);
float fx(float limd, float limu, float a, float b, int sel);
void Momen();
float deffuzzyfikasi_pH();
float deffuzzyfikasi_PPM();

#endif