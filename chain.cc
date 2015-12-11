{
  TChain* chain = new TChain("troja");
  //chain->Add("132Sndp_5_troja.root");
  //chain->Add("132Sntd_5_troja.root");

  chain->Add("132Sndp_1_troja.root");
  chain->Add("132Sntp_1_troja.root");
  chain->Add("132Sntd_1_troja.root");
  
  chain->StartViewer();


}
