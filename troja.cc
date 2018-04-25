//
/// \file troja.cc
/// \brief Main program

#include "DetectorConstruction.hh"
#include "DetectorInfo.hh"
#include "DetectorGlobals.hh"

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
// #include "QGSP_BIC_EMY.hh" //geant4.9
//#include "FTFP_BERT.hh" //geant4.10
//#include "G4EmStandardPhysics_option3.hh" //geant4.10
#include "PhysicsList.hh" //geant4.10.0
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "Randomize.hh"

#include "TApplication.h"
#include "TFile.h"

#include "LibPerso.h"
#include "InputInfo.hh"

//#include "MUST2Array.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{

  // automatically load all root libraries
  TApplication* theApp=new TApplication("theApp",  0, 0);
  
  char inputFileName[100];
  if(argc==2){
    sprintf(inputFileName, "%s", argv[1]);
    printf("Using input file '%s'", inputFileName);
  }else if(argc<2){
    printf("Help:\n - one input text file is needed, e.g. troja input.txt\n - any second input argument will start interactive mode\n");
    return 0;
  }else{
    printf("Info: only the first input file/argument is used to collect information; second argument just starts interactive mode!\n");
    sprintf(inputFileName, "%s", argv[1]);
    printf("Using input file '%s'", inputFileName);
    //return 0;
  }

  //sprintf(inputFileName, "doAll/input.txt");

  // read input file
  InputInfo* info = new InputInfo();
  
  info->parse(inputFileName);
  
  
  printf("\nTroja: input file parsed.\nGetting detector geometry...\n\n");


  // contains detector information
  DetectorInfo* detInfo = new DetectorInfo();
  
  //detInfo->Parse("/home/philipp/sim/troja/detector_geometry.txt");  // this file is defined in InputInfo 
  detInfo->Parse(info->fInFileNameGeometry);  


  printf("\nTroja: detector geometry file parsed.\nSetting up Geant stuff...\n\n");




  // Choose the Random engine
  //
printf("- CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine)\n");
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  //
printf("- G4RunManager* runManager = new G4RunManager()\n");
  G4RunManager* runManager = new G4RunManager();

  // Set mandatory initialization classes
  //
  // Detector construction
printf("- runManager->SetUserInitialization(new DetectorConstruction(detInfo))\n");
  runManager->SetUserInitialization(new DetectorConstruction(info, detInfo));

  // Physics list
  //  G4VModularPhysicsList* physicsList = new QGSP_BIC_EMY;
  //  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  //  G4VModularPhysicsList* physicsList = new PhysicsList();
printf("- PhysicsList* physicsList = new PhysicsList()\n");
  PhysicsList* physicsList = new PhysicsList();
printf("- physicsList->AddPhysicsList(emstandard_opt4)\n");
  physicsList->AddPhysicsList("emstandard_opt4"); // good for low energy ions
  //physicsList->AddPhysicsList("emstandard_opt0");

  //physicsList->SetCutForGamma(0.001*mm);

  physicsList->SetVerboseLevel(2);
  //physicsList->SetVerboseLevel(0);
  runManager->SetUserInitialization(physicsList);
    
  // Primary generator action
  // runManager->SetUserAction(new PrimaryGeneratorAction());
  runManager->SetUserAction(new PrimaryGeneratorAction(info));

  // Set user action classes
  //
  // Stepping action
  //runManager->SetUserAction(new SteppingAction());     
  runManager->SetUserAction(new SteppingAction(detInfo));     

  // Event action
printf("- runManager->SetUserAction(new EventAction())\n");
  runManager->SetUserAction(new EventAction());

  // Run action
  runManager->SetUserAction(new RunAction(info, detInfo));
     
  // Initialize G4 kernel
  //
printf("- runManager->Initialize()\n");
  runManager->Initialize();
  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if (argc==2) {
    // batch mode
    //    G4String command = "/control/execute ";
    //    G4String fileName = argv[1];
    //    UImanager->ApplyCommand(command+fileName);

    //G4cout << "Creating string " << G4endl;
    
    G4String command;
    //command = "/run/beamOn " + (G4String)info->GetNumberOfEvents();
    char evnts[100];
    sprintf(evnts,"%i",info->fNumberEvents);
    command = evnts;
    command = "/run/beamOn " + command;
    G4cout << "EXECUTING " << command << G4endl;
    
    TStopwatch* watch = new TStopwatch();
    watch->Start();
        
    UImanager->ApplyCommand(command);

    watch->Stop();
    printf("Took: real time %f sec., CPU time %f sec.\n", watch->RealTime(), watch->CpuTime());

    printf("Events written to file '%s'\n", info->fOutFileNameTroja);
    
  }
  else {
    argc=1;
    // interactive mode : define UI session
    #ifdef G4UI_USE
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
    #ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute init_vis.mac"); 
    #else
      UImanager->ApplyCommand("/control/execute init.mac"); 
    #endif
      ui->SessionStart();
      delete ui;
    #endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !
  
  #ifdef G4VIS_USE
    delete visManager;
  #endif
  delete runManager;
  
  delete theApp;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
