{
  TFile *input = TFile::Open("132Snout.root");
  TTree *intree = (TTree*)input->Get("events");
  
  TFile *output = TFile::Open("troja.root");
  TTree *outree = (TTree*)output->Get("troja");

  intree->AddFriend(outree);
  intree->StartViewer();


}
