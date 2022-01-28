/*
    Author:  Burkhant Suerfu
    Date:    November 18, 2021
    Contact: suerfu@berkeley.edu
*/

/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "GeometryConstruction.hh"

#include "G4Neutron.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "StepInfo.hh"


SteppingAction::SteppingAction( RunAction* runAction, EventAction* eventAction ) : G4UserSteppingAction(),
    fRunAction( runAction ),
    fEventAction( eventAction )
{}


SteppingAction::~SteppingAction(){}


void SteppingAction::UserSteppingAction( const G4Step* step){
    
    // Get the current track.
    //
    G4Track* track = step->GetTrack();

    // Check if the particle should be ignored.
    //
    G4String particle = track->GetParticleDefinition()->GetParticleName();
    if( fRunAction->ExcludeParticle( particle) ){
        return;
    }

    // Check if the volume should be ignored.
    //
    G4String vol = track->GetVolume()->GetName();
    if( fRunAction->ExcludeVolume( vol ) ){
        return;
    }

    // Maybe check if the process should be ignored.
    //
    G4String procName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    if( fRunAction->ExcludeProcess( procName ) ){
        return;
    }

    /*
    // Record only EM interactions.
    if( particle=="gamma" || particle=="e+" || particle=="e-"){
        fEventAction->GetStepCollection().push_back(StepInfo(step));
    }
    */
    
    fEventAction->GetStepCollection().push_back(StepInfo(step));

	// Check kill-when-hit volume. Remove the track at the volume surface.
    G4VPhysicalVolume* pv = track->GetVolume();
    if( pv!=0 ){
        G4String vol = pv->GetName();
        if( fRunAction->KillWhenHit( vol ) ){
            track->SetTrackStatus(fStopAndKill);
        }
    }
    


}

