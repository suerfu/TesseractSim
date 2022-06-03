/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RUNACTION_H
#define RUNACTION_H 1

#include "G4UserRunAction.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TMacro.h"

#include <vector>
#include <sstream>
#include <set>

#include "utility.hh"

class G4Run;
class RunActionMessenger;

class RunAction : public G4UserRunAction {

public:

    RunAction( CommandlineArguments* c);

    virtual ~RunAction();

    void SetOutputFileName( G4String newname );

    G4String GetOutputFileName(){ return outputName; }

    virtual void BeginOfRunAction( const G4Run* );

    virtual void EndOfRunAction( const G4Run* );

    /// Keep a record of the macro used for the run.
    void AddMacro( G4String a ){
        macros.push_back( a );
    }

    TTree* GetDataTree();

    CommandlineArguments* GetCommandlineArguments();

    void AddRecordWhenHit( G4String a);
    bool RecordWhenHit( G4String a);

    void AddExcludeParticle( G4String a);
    bool ExcludeParticle( G4String a);

    void AddExcludeVolume( G4String a);
    bool ExcludeVolume( G4String a);

    void AddExcludeProcess( G4String a);
    bool ExcludeProcess( G4String a);

    void AddKillWhenHit( G4String a);
    bool KillWhenHit( G4String a);


private:

    string version;

    RunActionMessenger* fRunActionMessenger;

    CommandlineArguments* fCmdlArgs;

    G4String outputName;
    
    TFile* outputFile;
    TTree* dataTree;

    std::vector< G4String > macros;
    std::vector< long > randomSeeds;

    std::set< G4String > recordWhenHit;
    std::set< G4String > killWhenHit;

    std::set< G4String > excludeParticle;
    std::set< G4String > excludeVolume;
    std::set< G4String > excludeProcess;

};


#endif
