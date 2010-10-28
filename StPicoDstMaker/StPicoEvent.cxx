#include "StPicoEvent.h"
#include "StPicoConstants.h"
#include "StEventTypes.h"
#include "StTree.h"
#include "StuRefMult.hh"
#include "TVector2.h"
#include "StMuDSTMaker/COMMON/StMuDst.h" 
#include "StMuDSTMaker/COMMON/StMuTrack.h" 
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StBTofHeader.h"

ClassImp(StPicoEvent)

StPicoEvent::StPicoEvent()
{}

StPicoEvent::StPicoEvent(StMuEvent* ev, StBTofHeader *header, float *Q)
{
  mRunId = ev->runNumber();
  mEventId = ev->eventNumber();
  mFillId = ev->runInfo().beamFillNumber(blue);
  mBField=ev->magneticField();

  mPrimaryVertex = ev->primaryVertexPosition();
  if( mPrimaryVertex.x()==mPrimaryVertex.y()&&mPrimaryVertex.y()==mPrimaryVertex.z() ){
    mPrimaryVertex.set(-999.,-999.,-999.);
  }

  int triggerId = 0;
  for(int i=0;i<nTrigger;i++) {
    if(ev->triggerIdCollection().nominal().isTrigger(Pico::mTriggerId[i])) triggerId |= (1 << i);;
  }
  mTriggerWord = (UShort_t)triggerId;

  mRefMultFtpcEast = (UShort_t)(ev->refMultFtpcEast());
  mRefMultFtpcWest = (UShort_t)(ev->refMultFtpcWest());
  mRefMultNeg = (UShort_t)(ev->refMultNeg());
  mRefMultPos = (UShort_t)(ev->refMultPos());

  mNVpdHitsEast = 0;
  mNVpdHitsWest = 0;
  mNT0 = 0;
  mVzVpd = Pico::SHORTMAX;
  if(header) {
    mNVpdHitsEast = (UChar_t)(header->numberOfVpdHits(east));
    mNVpdHitsWest = (UChar_t)(header->numberOfVpdHits(west));
    mNT0 = (UShort_t)(header->nTzero());
    mVzVpd = (fabs(header->vpdVz()*100.)>Pico::SHORTMAX) ? Pico::SHORTMAX : (UShort_t)(floor(header->vpdVz()*100.));
  }

    //Nov.10, 2008, Na
    StZdcTriggerDetector &ZDC = ev->zdcTriggerDetector();
    mZdcSumAdcEast = (UShort_t)ZDC.adcSum(east);
    mZdcSumAdcWest = (UShort_t)ZDC.adcSum(west);   
    for (int strip=1;strip<9;strip++) {
        if (ZDC.zdcSmd(east,1,strip)) 
          mZdcSmdEastHorizontal[strip-1] = (UShort_t)ZDC.zdcSmd(east,1,strip);
        if (ZDC.zdcSmd(east,0,strip))  
          mZdcSmdEastVertical[strip-1] = (UShort_t)ZDC.zdcSmd(east,0,strip);
        if (ZDC.zdcSmd(west,1,strip)) 
          mZdcSmdWestHorizontal[strip-1] = (UShort_t)ZDC.zdcSmd(west,1,strip);
        if (ZDC.zdcSmd(west,0,strip))  
          mZdcSmdWestVertical[strip-1] = (UShort_t)ZDC.zdcSmd(west,0,strip);
    }

    StVpdTriggerDetector &VPD = ev->vpdTriggerDetector();

    for(int i=0; i<16; i++){
      //event.VPD[i]= 1.0*theVPD.adc(i);
      if(i>=0 && i<8) {
        mVpd[i]=(UShort_t)VPD.ADC(east,8-i);
        mVpd[i+8]=(UShort_t)VPD.TDC(east,8-i);
        mVpd[i+32]=(UShort_t)VPD.ADC(west,8-i);
        mVpd[i+40]=(UShort_t)VPD.TDC(west,8-i);
      }
      if(i>=8 && i<16) {
        mVpd[i+8]=(UShort_t)VPD.ADC(east,32-(i+8));
        mVpd[i+16]=(UShort_t)VPD.TDC(east,32-(i+8));
        mVpd[i+40]=(UShort_t)VPD.ADC(west,32-(i+8));
        mVpd[i+48]=(UShort_t)VPD.TDC(west,32-(i+8));
        //cout<<"VPD-------  "<<VPD.ADC(east,32-(i+8))<<endl;
      }
    }

    mZDCx = ev->runInfo().zdcCoincidenceRate();
    mBBCx = ev->runInfo().bbcCoincidenceRate();

    mBackgroundRate = ev->runInfo().backgroundRate();

    mBbcBlueBackgroundRate = ev->runInfo().bbcBlueBackgroundRate();
    mBbcYellowBackgroundRate = ev->runInfo().bbcYellowBackgroundRate();

    mBbcEastRate = ev->runInfo().bbcEastRate();
    mBbcWestRate = ev->runInfo().bbcWestRate();

    mZdcEastRate = ev->runInfo().zdcEastRate();
    mZdcWestRate = ev->runInfo().zdcWestRate();

    mSpaceCharge = ev->runInfo().spaceCharge();


    mQx_ran_1 = Q[0];
    mQy_ran_1 = Q[1];
    mQx_ran_2 = Q[2];
    mQy_ran_2 = Q[3];
    
    mQx = mQx_ran_1 + mQx_ran_2;
    mQy = mQy_ran_1 + mQy_ran_2;
    
    mQx_chg_pos = Q[4];
    mQy_chg_pos = Q[5];
    mQx_chg_neg = Q[6];
    mQy_chg_neg = Q[7];
    
    mQx_eta_pos = Q[8];
    mQy_eta_pos = Q[9];
    mQx_eta_neg = Q[10];
    mQy_eta_neg = Q[11];

    mQx_Ftpc_ran_1 = Q[12];
    mQy_Ftpc_ran_1 = Q[13]; 
    mQx_Ftpc_ran_2 = Q[14];
    mQy_Ftpc_ran_2 = Q[15];

    mQx_Ftpc = mQx_Ftpc_ran_1 + mQx_Ftpc_ran_2;
    mQy_Ftpc = mQy_Ftpc_ran_1 + mQy_Ftpc_ran_2;
 
    mQx_Ftpc_chg_pos = Q[16];
    mQy_Ftpc_chg_pos = Q[17];
    mQx_Ftpc_chg_neg = Q[18];
    mQy_Ftpc_chg_neg = Q[19];
 
    mQx_Ftpc_West = Q[20];
    mQy_Ftpc_West = Q[21];
    mQx_Ftpc_East = Q[22];
    mQy_Ftpc_East = Q[23];
 
    mQx_Ftpc_West_ran_1 = Q[24];
    mQy_Ftpc_West_ran_1 = Q[25];
    mQx_Ftpc_West_ran_2 = Q[26];
    mQy_Ftpc_West_ran_2 = Q[27];

    mQx_Ftpc_East_ran_1 = Q[28];
    mQy_Ftpc_East_ran_1 = Q[29];
    mQx_Ftpc_East_ran_2 = Q[30];
    mQy_Ftpc_East_ran_2 = Q[31];

    mQx_Ftpc_West_chg_pos = Q[32];
    mQy_Ftpc_West_chg_pos = Q[33];
    mQx_Ftpc_West_chg_neg = Q[34];
    mQy_Ftpc_West_chg_neg = Q[35];

    mQx_Ftpc_East_chg_pos = Q[36];
    mQy_Ftpc_East_chg_pos = Q[37];
    mQx_Ftpc_East_chg_neg = Q[38];
    mQy_Ftpc_East_chg_neg = Q[39];
    
}

StPicoEvent::~StPicoEvent()
{ }