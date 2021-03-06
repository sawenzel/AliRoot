void CreateIdealOnlineCalibPars(){

  // Create TOF Dummy (delay=0, pulser/noise/HW status ok) Online Calibration Object for reconstruction
  // and write it on CDB
  // Both old type objects (using TObjArrays) and new type objects (using AliTOFChannelOnlineArray/
  // AliTOFChannelOnlineStatusArray are written

  AliTOFcalib *tofcalib = new AliTOFcalib();
  tofcalib->CreateCalArrays();
  TObjArray *tofCalOnline = (TObjArray*) tofcalib->GetTOFCalArrayOnline(); 
  TObjArray *tofCalOnlinePulser = (TObjArray*) tofcalib->GetTOFCalArrayOnlinePulser(); 
  TObjArray *tofCalOnlineNoise = (TObjArray*) tofcalib->GetTOFCalArrayOnlineNoise(); 
  TObjArray *tofCalOnlineHW = (TObjArray*) tofcalib->GetTOFCalArrayOnlineHW(); 
  AliTOFChannelOnlineArray *delayArr = (AliTOFChannelOnlineArray*) tofcalib->GetTOFOnlineDelay();
  AliTOFChannelOnlineStatusArray *status = (AliTOFChannelOnlineStatusArray*) tofcalib->GetTOFOnlineStatus();
  // Write the dummy offline calibration object on CDB

  AliCDBManager *man = AliCDBManager::Instance();
  man->SetDefaultStorage("local://$ALICE_ROOT/OCDB");
  Int_t nChannels = AliTOFGeometry::NSectors()*(2*(AliTOFGeometry::NStripC()+AliTOFGeometry::NStripB())+AliTOFGeometry::NStripA())*AliTOFGeometry::NpadZ()*AliTOFGeometry::NpadX();
  for (Int_t ipad = 0 ; ipad<nChannels; ipad++){
    AliTOFChannelOnline *calChannelOnline = (AliTOFChannelOnline*)tofCalOnline->At(ipad);
    AliTOFChannelOnlineStatus *calChannelOnlinePulser = (AliTOFChannelOnlineStatus*)tofCalOnlinePulser->At(ipad);
    AliTOFChannelOnlineStatus *calChannelOnlineNoise = (AliTOFChannelOnlineStatus*)tofCalOnlineNoise->At(ipad);
    AliTOFChannelOnlineStatus *calChannelOnlineHW = (AliTOFChannelOnlineStatus*)tofCalOnlineHW->At(ipad);
    Float_t delay = 0.;
    delayArr->SetDelay(ipad,delay);
    calChannelOnline->SetDelay(delay); 
    calChannelOnlinePulser->SetStatus(AliTOFChannelOnlineStatus::kTOFPulserOk);
    calChannelOnlineNoise->SetStatus(AliTOFChannelOnlineStatus::kTOFNoiseOk);
    calChannelOnlineHW->SetStatus(AliTOFChannelOnlineStatus::kTOFHWOk);
    status->SetHWStatus(ipad,AliTOFChannelOnlineStatusArray::kTOFHWOk);
    status->SetPulserStatus(ipad,AliTOFChannelOnlineStatusArray::kTOFPulserOk);
    status->SetNoiseStatus(ipad,AliTOFChannelOnlineStatusArray::kTOFNoiseOk);
 } 
  tofcalib->WriteParOnlineDelayOnCDB("TOF/Calib");  // new obj
  tofcalib->WriteParOnlineStatusOnCDB("TOF/Calib"); // new obj
  tofcalib->WriteParOnlineOnCDB("TOF/Calib");       // old obj
  tofcalib->WriteParOnlinePulserOnCDB("TOF/Calib"); // old obj
  tofcalib->WriteParOnlineNoiseOnCDB("TOF/Calib");  // old obj
  tofcalib->WriteParOnlineHWOnCDB("TOF/Calib");     // old obj
 return;
}
