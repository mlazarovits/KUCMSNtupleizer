// -*- C++ -*-
//
// Package:    KUCMSNtupilizer
// Class:      KUCMSNtupilizer
//
/**\class KUCMSNtupilizer KUCMSNtupilizer.cc LLPgammaAnalyzer/plugins/KUCMSNtupilizer.cc

 		Description: [one line class summary]

 		Implementation: [Notes on implementation]

*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Original Author:  Jack W King III
//         Created:  Wed, 27 Jan 2021 19:19:35 GMT
//
//
//
//----------------------------------------  cc file   --------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "KUCMSNtupleizer.hh"
#include "KUCMSHelperFunctions.hh"
//#include "KUCMSRootHelperFunctions.hh"

using namespace std;

//#define DEBUG true
#define DEBUG false

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// constructors and destructor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

KUCMSNtupilizer::KUCMSNtupilizer(const edm::ParameterSet& iConfig) :

	// -- declare tags and flags ----------------------------------------------------------
	// accuire input values form python config master file ( the one we use cmsRun with interactivilly )

	// flags
	
	hasGenInfo(iConfig.existsAs<bool>("hasGenInfo")  ? iConfig.getParameter<bool>("hasGenInfo")  : true),

	// triggers
	//triggerResultsTag(iConfig.getParameter<edm::InputTag>("triggerResults")),
	//triggerObjectsTag(iConfig.getParameter<edm::InputTag>("triggerObjects")),

	// tracks
	tracksTag(iConfig.getParameter<edm::InputTag>("tracks")),
	
	// pfcands
	pfcandTag(iConfig.getParameter<edm::InputTag>("pfcandidates")),
    pfCanTag(iConfig.getParameter<edm::InputTag>("particleflow")),
	pfCanPhoMapTag(iConfig.getParameter<edm::InputTag>("pfcanphomap")),
    pfCanOOTPhoMapTag(iConfig.getParameter<edm::InputTag>("pfcanootphomap")),
    pfCanEleMapTag(iConfig.getParameter<edm::InputTag>("pfcanelemap")),
	
	// vertices
	verticesTag(iConfig.getParameter<edm::InputTag>("vertices")),

	// rho
	//rhoTag(iConfig.getParameter<edm::InputTag>("rho")),

	// mets
	pfmetsTag(iConfig.getParameter<edm::InputTag>("mets")),  

	// supercluster
	superClusterCollectionTag(iConfig.getParameter<edm::InputTag>("superClusters")),
	ootSuperClusterCollectionTag(iConfig.getParameter<edm::InputTag>("ootSuperClusters")),

	// caloclusters
	caloClusterTag(iConfig.getParameter<edm::InputTag>("caloClusters")),

	// jets
	jetsTag(iConfig.getParameter<edm::InputTag>("jets")), 

    // calojets
    caloJetsTag(iConfig.getParameter<edm::InputTag>("calojets")),

    // genJets
    genJetsTag(iConfig.getParameter<edm::InputTag>("genjets")),

	// electrons
	electronsTag(iConfig.getParameter<edm::InputTag>("electrons")),  
    eleMVAIDLooseMapTag(iConfig.getParameter<edm::InputTag>("eleMVAIDLooseMap")),

	// muons
	muonsTag(iConfig.getParameter<edm::InputTag>("muons")),  

	// recHits
	recHitsEBTag(iConfig.getParameter<edm::InputTag>("recHitsEB")),  
	recHitsEETag(iConfig.getParameter<edm::InputTag>("recHitsEE")),

	// gedphotons
	gedPhotonsTag(iConfig.getParameter<edm::InputTag>("gedPhotons")),
    // ootPhotons
    ootPhotonsTag(iConfig.getParameter<edm::InputTag>("ootPhotons")),
    phoCBIDLooseMapTag(iConfig.getParameter<edm::InputTag>("phoCBIDLooseMap")),

	// pfcand ref
	//reco2pfTag(iConfig.getParameter<edm::InputTag>("recoToPFMap")),
	//reco2pf_(mayConsume<edm::ValueMap<std::vector<reco::PFCandidateRef> > >(iConfig.getParameter<edm::InputTag>("recoToPFMap"))),

	// genParticles
	genEvtInfoTag(iConfig.getParameter<edm::InputTag>("genEvt")),
  	gent0Tag(iConfig.getParameter<edm::InputTag>("gent0")),
  	genxyz0Tag(iConfig.getParameter<edm::InputTag>("genxyz0")),
  	pileupInfosTag(iConfig.getParameter<edm::InputTag>("pileups")),
  	genParticlesTag(iConfig.getParameter<edm::InputTag>("genParticles")),

    // ECAL RECORDS 
    caloGeometryToken_(esConsumes()),
    caloTopologyToken_{esConsumes()},
    ecalLaserDbServiceToken_(esConsumes()),
    ecalIntercalibConstantsToken_(esConsumes()),
    ecalADCToGeVConstantToken_(esConsumes()),
    EcalPedestalsToken_(esConsumes())


	// -- end of tag declarations ---------------------------------------
{ //<<<< KUCMSNtupilizer::KUCMSNtupilizer(const edm::ParameterSet& iConfig) :

	usesResource();
	usesResource("TFileService");

	// -- consume tags ------------------------------------------------------------
	// creats "token" for all collections that we wish to process
	
	if( DEBUG ) std::cout << "In constructor for KUCMSNtupilizer - tag and tokens" << std::endl;

	// Triggers
	//triggerResultsToken_ 	= consumes<edm::TriggerResults>(triggerResultsTag);
	//triggerObjectsToken_ 	= consumes<std::vector<pat::TriggerObjectStandAlone>>(triggerObjectsTag);

	// tracks 
	tracksToken_				= consumes<std::vector<reco::Track>>(tracksTag);

	// genparticles
	// genpart_token_              = consumes<std::vector<pat::PackedGenParticle>>(genpartTag);

	// pfcandidates
	pfCan_token_        		= consumes<std::vector<reco::PFCandidate>>(pfCanTag);
	pfCanPhotonMap_token_		= consumes<edm::ValueMap<edm::Ptr<reco::PFCandidate>>>(pfCanPhoMapTag);
    pfCanOOTPhotonMap_token_    = consumes<edm::ValueMap<edm::Ptr<reco::PFCandidate>>>(pfCanOOTPhoMapTag);
    pfCanElectronMap_token_     = consumes<edm::ValueMap<edm::Ptr<reco::PFCandidate>>>(pfCanEleMapTag);

	// pfcandidates view
    pfcand_token_               = consumes<CandidateView>(pfcandTag);

	// vertices
	verticesToken_				= consumes<std::vector<reco::Vertex>>(verticesTag);

	// rho
	//rhoToken_					= consumes<double>(rhoTag);

	// mets
	pfmetsToken_				= consumes<std::vector<reco::PFMET>>(pfmetsTag);

	// supercluster
	scToken_             		= consumes<reco::SuperClusterCollection>(superClusterCollectionTag);
	ootScToken_          		= consumes<reco::SuperClusterCollection>(ootSuperClusterCollectionTag); 

	// caloClusters
	ccToken_			     	= consumes<std::vector<reco::CaloCluster>>(caloClusterTag);	

	// jets
	jetsToken_					= consumes<std::vector<reco::PFJet>>(jetsTag);
    caloJetsToken_              = consumes<std::vector<reco::CaloJet>>(caloJetsTag);

	// leptons
	//electronsToken_				= consumes<std::vector<reco::GsfElectron>>(electronsTag);
    electronsToken_             = consumes<edm::View<reco::GsfElectron>>(electronsTag);
	//eleMVAIDLooseMapToken_ 		= consumes<edm::ValueMap<bool>>(eleMVAIDLooseMapTag);

	// muons
	muonsToken_					= consumes<std::vector<reco::Muon>>(muonsTag);

	// rechits
	recHitsEBToken_				= consumes<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit>>>(recHitsEBTag);
	recHitsEEToken_				= consumes<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit>>>(recHitsEETag);

	// photons
	gedPhotonsToken_ 			= consumes<edm::View<reco::Photon>>(gedPhotonsTag);
	//phoCBIDLooseMapToken_		= consumes<edm::ValueMap<bool>>(phoCBIDLooseMapTag); 
	ootPhotonsToken_ 			= consumes<edm::View<reco::Photon>>(ootPhotonsTag);

	// pfref
	//reco2pf_					= consumes<edm::ValueMap<std::vector<reco::PFCandidateRef>>>(reco2pfTag);

	//genParticles & genJets
	if( hasGenInfo ){

		genJetsToken_ 			= consumes<std::vector<reco::GenJet>>(genJetsTag);
		genEvtInfoToken_   		= consumes<GenEventInfoProduct>(genEvtInfoTag);
    	gent0Token_        		= consumes<float>(gent0Tag);
    	genxyz0Token_      		= consumes<Point3D>(genxyz0Tag);
    	pileupInfosToken_  		= consumes<std::vector<PileupSummaryInfo>>(pileupInfosTag);
    	genParticlesToken_ 		= consumes<std::vector<reco::GenParticle>>(genParticlesTag);

	}//<<>>if( hasGenInfo )

// ---------------------------------------------------------------------------------
}//>>>>KUCMSNtupilizer::KUCMSNtupilizer(const edm::ParameterSet& iConfig)

KUCMSNtupilizer::~KUCMSNtupilizer(){

	///////////////////////////////////////////////////////////////////
	// do anything here that needs to be done at desctruction time   //
	// (e.g. close files, deallocate resources etc.)                 //
	///////////////////////////////////////////////////////////////////

}//>>>>KUCMSNtupilizer::~KUCMSNtupilizer()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ Analyzer Inherited Class Functions ------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KUCMSNtupilizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

	using namespace edm;

	// -- Consume Tokens --------------------------------------------
	// gets pointer to the collections from cmssw using the "token" for that collection

	if( DEBUG ) std::cout << "Consume Tokens -------------------------------------------- " << std::endl;

	// TRIGGER
	//iEvent.getByToken(triggerResultsToken_,triggerResults_);
	//iEvent.getByToken(triggerObjectsToken_,triggerObjects_);

	// TRACKS
	iEvent.getByToken(tracksToken_, tracks_);

	// PFCANDIDATES
	iEvent.getByToken(pfcand_token_, pfcands_);
	iEvent.getByToken(pfCan_token_, pfCans_);
	iEvent.getByToken(pfCanPhotonMap_token_, pfCanPhotonMap_);
    iEvent.getByToken(pfCanOOTPhotonMap_token_, pfCanOOTPhotonMap_);
    iEvent.getByToken(pfCanElectronMap_token_, pfCanElectronMap_);

	// VERTICES
	iEvent.getByToken(verticesToken_, vertices_);

	// RHO
	//iEvent.getByToken(rhoToken_, rho_);

	// METS
	iEvent.getByToken(pfmetsToken_, pfmets_);

	// SUPERCLUSTERS
	iEvent.getByToken(scToken_, superCluster_);  
	iEvent.getByToken(ootScToken_, ootSuperCluster_);

	// CALOCLUSTERS
	iEvent.getByToken(ccToken_, caloCluster_);

	// JETS
	iEvent.getByToken(jetsToken_, jets_);

    // CALOJETS
    iEvent.getByToken(caloJetsToken_, caloJets_);

	// GENPARTICLES & GENJETS
	if( hasGenInfo ){

		iEvent.getByToken(genEvtInfoToken_, genEvtInfo_);
    	iEvent.getByToken(gent0Token_, gent0_);
    	iEvent.getByToken(genxyz0Token_, genxyz0_);
    	iEvent.getByToken(pileupInfosToken_, pileupInfos_);
    	iEvent.getByToken(genParticlesToken_, genParticles_);
    	iEvent.getByToken(genJetsToken_, genJets_);

	}//<<>>if( hasGenInfo )

	// ELECTRONS
	iEvent.getByToken(electronsToken_, electrons_);
	//iEvent.getByToken(eleMVAIDLooseMapToken_, eleMVAIDLooseMap_);

	// PHOTONS
	iEvent.getByToken(gedPhotonsToken_, gedPhotons_);
    //iEvent.getByToken(phoCBIDLooseMapToken_, phoCBIDLooseMap_);
	iEvent.getByToken(ootPhotonsToken_, ootPhotons_);

	// MUONS
    iEvent.getByToken(muonsToken_, muons_);

	// ECAL RECHITS
	iEvent.getByToken(recHitsEBToken_, recHitsEB_);
	iEvent.getByToken(recHitsEEToken_, recHitsEE_);

    // GEOMETRY : https://gitlab.cern.ch/shervin/ECALELF
    caloGeo_ = iSetup.getHandle(caloGeometryToken_);
    barrelGeometry = caloGeo_->getSubdetectorGeometry(DetId::Ecal, EcalSubdetector::EcalBarrel);
    endcapGeometry = caloGeo_->getSubdetectorGeometry(DetId::Ecal, EcalSubdetector::EcalEndcap);
    ecalGeometry = &iSetup.getData(caloGeometryToken_);

    // CaloTopology
    caloTopo_ = iSetup.getHandle(caloTopologyToken_);
    ecalTopology = &iSetup.getData(caloTopologyToken_);
    barrelTopology = caloTopo_->getSubdetectorTopology(DetId::Ecal, EcalSubdetector::EcalBarrel);
    endcapTopology = caloTopo_->getSubdetectorTopology(DetId::Ecal, EcalSubdetector::EcalEndcap);

    // Laser constants : http://cmslxr.fnal.gov/source/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
    laser_ = iSetup.getHandle(ecalLaserDbServiceToken_);
    evTime = iEvent.time();

    // Intercalibration constants : http://cmslxr.fnal.gov/source/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
    interCalib_ = iSetup.getHandle(ecalIntercalibConstantsToken_);
    interCalibMap = &interCalib_->getMap();

    // ADCToGeV : http://cmslxr.fnal.gov/source/RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h
    adcToGeV_ = iSetup.getHandle(ecalADCToGeVConstantToken_);
    adcToGeVEB = adcToGeV_->getEBValue();
    adcToGeVEE = adcToGeV_->getEEValue();

    // Pedestals : https://github.com/ferriff/usercode/blob/master/DBDump/plugins/DBDump.cc
    pedestals_ = iSetup.getHandle(EcalPedestalsToken_);

	// PFCand refs
	//iEvent.getByToken(reco2pf_, reco2pf);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // ---  Collecting objests ( preprocessing object pruning ) ---------------------------------------
    // -------------------------------------------------------------------------------------------------
	// -- Process Event  ---------------------------------------    
	// ** extracted from disphoana : starting point **** not all functios/varibles defined ***************
	// ** for example only -- convert to nano?, use ewkino varibles for output, find rechit information ** 
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

    if( DEBUG ) std::cout << "***********************************************************************************************" << std::endl;

    // intilize global event varibles
    vtxX = 0;
    vtxY = 0;
    vtxZ = 0;
    evtMET = 0;

    // process event varibles

    processEvent( iEvent );
    processVtx();

	// declare local vector for preprocess of collections

	fjets.clear();
	fjetsID.clear();
	fbclusts.clear();
	frechits.clear();
	frhused.clear();
	fphotons.clear();
    fphotonID.clear();
  	//footphotons.clear();
	felectrons.clear();
    felectronID.clear();
	fcalojets.clear();
	fmuons.clear();
	fpfmet.clear();
	fgenparts.clear();
	fgenpartllp.clear();
	fgenjets.clear();

	// Selection criteria declartation for filtering -------------------------------
	// ------  move these to config input values via tag system --------------------------

    float   jetPTmin        = 15.0;// for energy/time comp
    float   jetEtaMax		= 3.0;

	float 	minRHEi			= 0.0;
    float   minRHEf         = 2.0;

    float   minMuonE       	= 1.0;

   	//float 	minMetE			= 1.0;

    float   minCaloCltrE    = 1.0;
    float   minCaloJetE     = 1.0;

    float   minPhoE         = 0.0;
    float 	phoMinPt 		= 0.0;
    float 	phoMinSeedTime 	= -25.0;

	// for pat::photon method IDs
    string  phoMvaWp80("mvaPhoID-RunIIFall17-v1-wp80");//2018
    //string phoMvaWp80("mvaPhoID-RunIIFall17-v2-wp80");//r3
    string  phoCutTight("cutBasedPhotonID-Fall17-94X-V2-tight");//2022
    string  phoCutLoose("cutBasedPhotonID-Fall17-94X-V2-loose");//2022
    //string phoCutLoose("cutBasedPhotonID-Fall17-94X-V1-loose");//2018i

    float   minEleE         = 0.0;

	// for pat::electron method IDs
    string eleMvaWp80("mvaEleID-Fall17-noIso-V2-wp80");
    string eleMvaWpLoose("mvaEleID-Fall17-noIso-V2-wpLoose");//r3
    string eleCutTight("cutBasedElectronID-Fall17-94X-V2-tight");
    string eleCutLoose("cutBasedElectronID-Fall17-94X-V2-loose");//2022
    //string eleCutLoose("cutBasedElectronID-Fall17-94X-V1-loose");//2018


    float   minGenE         = 0.0;
	bool 	onlyEB 			= false;
    float   ebMaxEta        = 1.479;

	// -----------------------------------------------------------------------------------------------
	// ---  Collecting objects ( preprocessing object pruning ) ---------------------------------------
	// -------------------------------------------------------------------------------------------------
	
	// fill filtered collection vectors

    if( DEBUG ) std::cout << "Collecting ECAL RecHits" << std::endl;
    for( const auto &recHit : *recHitsEB_ ){ if( recHit.energy() > minRHEi ) frechits.push_back(recHit); frhused.push_back(false);}
    for( const auto &recHit : *recHitsEE_ ){ if( recHit.energy() > minRHEi ) frechits.push_back(recHit); frhused.push_back(false);}

    //if( DEBUG ) std::cout << "Collecting Muons" << std::endl; //  set EB only filter
    //for( const auto &muon : *muons_ ){ if( muon.energy() > minMuonE ) fmuons.push_back(muon); }

    if( DEBUG ) std::cout << "Collecting Met" << std::endl;
    //for( const auto met : *pfmets_ ){ if( met.energy() > minMetE ) 
	fpfmet.push_back((*pfmets_)[0]);

	if( DEBUG ) std::cout << "Filter Jets" << std::endl;
	for(const auto &jet : *jets_ ){ // Filters jet collection & sorts by pt

		if( jet.pt() < jetPTmin ) continue;
	  	if( onlyEB && std::abs(jet.eta()) > ebMaxEta ) continue;
		if( std::abs(jet.eta()) > jetEtaMax ) continue; 
	  	// save the jets, and then store the ID
	  	fjets.emplace_back(jet);
		auto jetID = 0;		
		fjetsID.push_back(jetID);	  

	  	std::sort(fjets.begin(),fjets.end(),sortByPt);

	}//<<>>for(const auto& jet : *jets_ )

	//calojets
	//if( DEBUG ) std::cout << "Collecting CaloJets" << std::endl; // set eb only filter
	//for(const auto &calojet : *caloJets_ ){ if( calojet.energy() > minCaloJetE ) fcalojets.push_back(calojet);  }

	//if( DEBUG ) std::cout << "Collecting Calo Clusters" << std::endl; // set eb only filter
	//for( const auto &bclust : *caloCluster_ ){ if( bclust.energy() > minCaloCltrE ) fbclusts.push_back(bclust); }
	
    if( DEBUG ) std::cout << "Collecting Photons/OOTPhotons" << std::endl;

    phoExcluded.clear();
    phoIsOotPho.clear();
	phoIdBools.clear();// indexed by pho index ( 0,1,2 ) * number of ids ( 1 current, 6? possible ) + index of ID wanted
    for (edm::View<reco::Photon>::const_iterator itPhoton = ootPhotons_->begin(); itPhoton != ootPhotons_->end(); itPhoton++) {
        auto idx = itPhoton - ootPhotons_->begin();//unsigned int
        auto ootPhoRef = ootPhotons_->refAt(idx);//edm::RefToBase<reco::GsfElectron> 
        auto &ootPho = (*itPhoton);
		if( onlyEB && ootPho.isEE() ) continue;
        auto minPhoPt = ootPho.pt() < phoMinPt;
        auto phoSeedTime = getPhotonSeedTime(ootPho);
        auto minTime = phoSeedTime < phoMinSeedTime;
        auto minEnergy = ootPho.energy() < minPhoE;
		if( minPhoPt || minTime || minEnergy ) continue;
        double minDr(0.5);
        double dRmatch(10.0);
        float matchpt(0);
        auto oEta = ootPho.eta();
        auto oPhi = ootPho.phi();
		auto oPt = ootPho.pt();
        for( const auto &gedPho : *gedPhotons_ ){
			if( onlyEB && gedPho.isEE() ) continue;
            auto pEta = gedPho.eta();
            auto pPhi = gedPho.phi();
            auto pPt = gedPho.pt();
            dRmatch = deltaR( pEta, oEta, pPhi, oPhi );
            if( dRmatch < minDr ){ minDr = dRmatch; matchpt = pPt; }
        }//<<>>for( int ip; ip < nPhotons; ip++ )
		fphotons.push_back(ootPho);
		phoIsOotPho.push_back(true);
		//phoIdBools.push_back((*phoCBIDLooseMap_)[ootPhoRef]);// not implimented 
        if( dRmatch < 0.1 && oPt < matchpt ) phoExcluded.push_back(true); 
		else phoExcluded.push_back(false);
    }//<<>>for( int io = 0; io < nOotPhotons; io++ )
    for (edm::View<reco::Photon>::const_iterator itPhoton = gedPhotons_->begin(); itPhoton != gedPhotons_->end(); itPhoton++) {
        auto idx = itPhoton - gedPhotons_->begin();//unsigned int
        auto gedPhoRef = gedPhotons_->refAt(idx);//edm::RefToBase<reco::GsfElectron> 
        auto &gedPho = (*itPhoton);
		if( onlyEB && gedPho.isEE() ) continue;
        auto minPt = gedPho.pt() < phoMinPt;
        auto phoSeedTime = getPhotonSeedTime(gedPho);
        auto minTime = phoSeedTime < phoMinSeedTime;
        auto minEnergy = gedPho.energy() < minPhoE;
        if( minPt || minTime || minEnergy ) continue;
        double minDr(0.5);
        double dRmatch(10.0);
        float matchpt(0);
		auto pEta = gedPho.eta();
		auto pPhi = gedPho.phi();
		auto pPt = gedPho.pt();
        for( const auto &ootPho : *ootPhotons_ ){
			if( onlyEB && ootPho.isEE() ) continue;
			auto oEta = ootPho.eta();
			auto oPhi = ootPho.phi();
			auto oPt = ootPho.pt(); 
            dRmatch = deltaR( pEta, oEta, pPhi, oPhi );
            if( dRmatch < minDr ){ minDr = dRmatch; matchpt = oPt; }
        }//<<>>for( int ip; ip < nPhotons; ip++ )
        fphotons.push_back(gedPho);
        phoIsOotPho.push_back(false);
		//phoIdBools.push_back((*phoCBIDLooseMap_)[gedPhoRef]);
        if( dRmatch < 0.1 && pPt < matchpt ) phoExcluded.push_back(true);
        else phoExcluded.push_back(false); 
    }//<<>>for( int io = 0; io < nOotPhotons; io++ )

    if( DEBUG ) std::cout << "Collecting Electrons" << std::endl;

	eleIdBools.clear();// indexed by ele index ( 0,1,2 ) * number of ids ( 1 current, 6? possible ) + index of ID wanted
	for (edm::View<reco::GsfElectron>::const_iterator itElectron = electrons_->begin(); itElectron != electrons_->end(); itElectron++) {
		auto idx = itElectron - electrons_->begin();//unsigned int
		auto electronRef = electrons_->refAt(idx);//edm::RefToBase<reco::GsfElectron> 
		auto &electron = (*itElectron);
		if ( onlyEB && std::abs(electron.eta()) > ebMaxEta) continue;
		//auto passIdCut = true; //electron.electronID(eleCutLoose);// pat electron ( miniAOD ) method
		//eleIdBools.push_back((*eleMVAIDLooseMap_)[electronRef]);// order is important, track how this vector is loaded
		auto passEnergyCut = electron.energy() > minEleE;
		if( passEnergyCut ) felectrons.push_back(electron);
	}//<<>>for( const auto electron : *electrons_ )

    int nGenPart(0);
	if( hasGenInfo ){ 

		if( DEBUG ) std::cout << "Collecting Gen Particles" << std::endl;
		for( const auto &genPart : *genParticles_ ){ 
	
			if( genPart.energy() < minGenE ) continue;
			nGenPart++;
			if( genPart.status() == 1 ){
				fgenparts.push_back(genPart);
				auto genPartID = llpGenChaseP(genPart,0);
				fgenpartllp.push_back(genPartID);
			}//<<>>if( genPart.isLastCopy() )
	
		}//<<>>for(const auto& genPart : *genParticles_ )

    	if( DEBUG ) std::cout << "Collecting GenJets" << std::endl;
    	for(const auto &genjet : *genJets_ ){ fgenjets.push_back(genjet); }

	}//<<>>if( hasGenInfo )


    if( DEBUG ) std::cout << "Processing Collections" << std::endl;

    //-------------`-----------------------------------------------------------------------
	// ----   Object processing ----------------------------------------------------------
    //------------------------------------------------------------------------------------
	// call functions to process collections and fill tree varibles to be saved
	// varibles to be saved to ttree are declared in the header

	// process gen collections
	if( hasGenInfo ){

		processGenPart();

	}//<<>>if( hasGenInfo )

	// process main collections
	processMet();
    //processClJet();
	processPhotons();
    //processElectrons();
	processJets();

	// Process RecHits : Must be processed last
    processRecHits( minRHEf );

    //------------------------------------------------------------------------------------
    //---- Object processing Completed ----------------------------------------------------------
    //------------------------------------------------------------------------------------

	auto fillTree = evtMET > 150.f;

	// -- Fill output trees ------------------------------------------
	if( fillTree ) outTree->Fill();


    if( DEBUG ) std::cout << "---------- Next Event -----" << std::endl;
	// -- EOFun ------------------------------------------------------
	//	 #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
	//	 ESHandle<SetupData> pSetup;
	//	 iSetup.get<SetupRecord>().get(pSetup);
	//	 #endif

}//>>>>void KUCMSNtupilizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ beginJob/endJob methods called once each job just before/after starting event loop	------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void KUCMSNtupilizer::beginJob(){

	// Global Varibles

	// Book output files and trees
	edm::Service<TFileService> fs;
	outTree = fs->make<TTree>("llpgtree","KUCMSNtuple");

	// Book //histograms ( if any )
	
	std::cout << "Histograms Booked" << std::endl;

	// Create output Tree branches -----------------------------
	// via set branch functions that call : outTree->Branch("",&); 
	// for the varibles to be saved in the tree for that particular collection

	setBranchesEvent();
	setBranchesVtx();

	setBranchesMet();
	setBranchesJets();
    //setBranchesClusterJets();
	setBranchesPhotons();
	//setBranchesElectrons();

	setBranchesGenParts();

	setBranchesRecHits();

}//>>>>void KUCMSNtupilizer::beginJob()


// ------------ method called once each job just after ending the event loop	------------
void KUCMSNtupilizer::endJob(){

}//>>>>void KUCMSNtupilizer::endJob()


// ------------ method fills 'descriptions' with the allowed parameters for the module	------------
void KUCMSNtupilizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {

	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);

	//Specify that only 'tracks' is allowed
	//To use, remove the default given above and uncomment below
	//ParameterSetDescription desc;
	//desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
	//descriptions.addDefault(desc);

}//>>>>void KUCMSNtupilizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)

//////////////////////////////////////////////////////////////////////////////////////////////////
//  --------------  collection processing and branch declaration functions ----------------------
//////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------
void KUCMSNtupilizer::setBranchesEvent(){

	EventInfoBM.makeBranch("run","RunNumber",KUCMSBranch::UINT,"Run number of event");
    EventInfoBM.makeBranch("lumi","LumiSection",KUCMSBranch::UINT,"Lumi section of event");
    EventInfoBM.makeBranch("event","EventNumber",KUCMSBranch::UINT);
    //outTree->Branch("event", &event, "event/l");

}//<<>>setBranchesEvent()

void KUCMSNtupilizer::processEvent( const edm::Event& iEvent ){

	EventInfoBM.clearBranches();

    uInt run   = iEvent.id().run();
    uInt lumi  = iEvent.luminosityBlock();
    uInt event = iEvent.id().event();

    if( DEBUG ) std::cout << "Processing event: " << event << " in run: " << run << " and lumiblock: " << lumi << std::endl;

	EventInfoBM.fillBranch("run",run);
	EventInfoBM.fillBranch("lumi",lumi);
    EventInfoBM.fillBranch("event",event);

}//<<>>processEvent()

//----------------------------------------------
void KUCMSNtupilizer::setBranchesVtx(){

    VertexBM.makeBranch("nVtx","nVtx",KUCMSBranch::UINT);
	VertexBM.makeBranch("vtxX","vtxX",KUCMSBranch::FLOAT);
    VertexBM.makeBranch("vtxY","vtxY",KUCMSBranch::FLOAT);
    VertexBM.makeBranch("vtxZ","vtxZ",KUCMSBranch::FLOAT);
    VertexBM.initBranches(outTree);

}//<<>>setBranchesVtx()

void KUCMSNtupilizer::processVtx(){

	VertexBM.clearBranches();
	
	// do not need to pass in collectioon handel -> class varible   --  Fix This
	//edm::Handle<std::vector<reco::Vertex>> vertices_
	uInt nvtx = vertices_->size();
    const auto & primevtx = vertices_->front();
    vtxX = primevtx.position().x();
    vtxY = primevtx.position().y();
    vtxZ = primevtx.position().z();

	VertexBM.fillBranch("nVtx",nvtx);
    VertexBM.fillBranch("vtxX",vtxX);
    VertexBM.fillBranch("vtxY",vtxY);
    VertexBM.fillBranch("vtxZ",vtxZ);

}//<<>>processVtx()

//-------------------------------------------------
void KUCMSNtupilizer::setBranchesMet(){

	MetBM.makeBranch("metSumEt","metSumEt",KUCMSBranch::FLOAT);
    MetBM.makeBranch("metPx","metPx",KUCMSBranch::FLOAT);
    MetBM.makeBranch("metPy","metPy",KUCMSBranch::FLOAT);
    MetBM.makeBranch("metCSumEt","metCSumEt",KUCMSBranch::FLOAT);
    MetBM.makeBranch("metCPx","metCPx",KUCMSBranch::FLOAT);
    MetBM.makeBranch("metCPy","metCPy",KUCMSBranch::FLOAT);
	MetBM.initBranches(outTree);

}//<<>>void setBranchesMet()

void KUCMSNtupilizer::processMet(){

    if( DEBUG ) std::cout << "Processing MET" << std::endl;

	MetBM.clearBranches();

    auto t1pfMET = fpfmet[0];
    float metSumEt = t1pfMET.sumEt();
    float metPt = t1pfMET.pt();
    float metPx = t1pfMET.px();
    float metPy = t1pfMET.py();
    float metPhi = t1pfMET.phi();

    float metCSumEt = metSumEt;
    float metCPx = metPx;
    float metCPy = metPy;

	evtMET = std::sqrt( sq2(metPx) + sq2(metPy));

    // ???  need corrected values for photon Et and Pt
    int nphidx = fphotons.size();
    for( auto phidx = 0; phidx < nphidx; phidx++ ){

        auto photon = fphotons[phidx];
        if( phoIsOotPho[phidx] && not phoExcluded[phidx] ){
            metCPx -= ((photon.pt())*std::cos(photon.phi()));
            metCPy -= ((photon.pt())*std::sin(photon.phi()));
            metCSumEt -= photon.et();
        }//<<>>if( phoIsOotPho[phidx] && not phoExcluded[phidx] )
        if( not phoIsOotPho[phidx] && phoExcluded[phidx] ){
            metCPx += ((photon.pt())*std::cos(photon.phi()));
            metCPy += ((photon.pt())*std::sin(photon.phi()));
            metCSumEt += photon.et();
        }//<<>>if( phoIsOotPho[phidx] && not phoExcluded[phidx] )   

    }//<<>>for( auto phidx = 0; phidx < fphotons.size(); phidx++ )

	MetBM.fillBranch("metSumEt",metSumEt);
    MetBM.fillBranch("metPx",metPx);
    MetBM.fillBranch("metPy",metPy);
    MetBM.fillBranch("metCSumEt",metCSumEt);
    MetBM.fillBranch("metCPx",metCPx);
    MetBM.fillBranch("metCPy",metCPy);

}//<<>>processMet( std::vector<reco::PFMET> fpfmet, std::vector<reco::Photon> fphotons )

//---------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesJets(){

    JetBM.makeBranch("jetHt","jetHt",KUCMSBranch::FLOAT);

    JetBM.makeBranch("jetE","jetE",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetM","jetM",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetPt","jetPt",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetEta","jetEta",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetPhi","jetPhi",KUCMSBranch::VFLOAT);

    JetBM.makeBranch("jetNHF","jetNHF",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetNEMF","jetNEMF",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetCHF","jetCHF",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetCEMF","jetCEMF",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetMUF","jetMUF",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetNHM","jetNHM",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetCHM","jetCHM",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetPHM","jetPHM",KUCMSBranch::VFLOAT);

    JetBM.makeBranch("jetParts","jetPartons",KUCMSBranch::VUINT);
    JetBM.makeBranch("jetDrRhIds","jetDrRhIds",KUCMSBranch::VVUINT);

    JetBM.makeBranch("jetGenImpactAngle","jetGenImpactAngle",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenTime","jetGenTime",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenPt","jetGenPt",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenEta","jetGenEta",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenPhi","jetGenPhi",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenEnergy","jetGenEnergy",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenDrMatch","jetGenDrMatch",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenTimeLLP","jetGenTimeLLP",KUCMSBranch::VFLOAT);
    JetBM.makeBranch("jetGenTOF","jetGenTOF",KUCMSBranch::VFLOAT);

    JetBM.initBranches(outTree);

}//<<>> setBranchesJets(){

void KUCMSNtupilizer::processJets(){ 

    if( DEBUG ) std::cout << "Init for Jet Loop " << std::endl;

    vector<uInt> noIdGrp(1,0);
	JetBM.clearBranches();

	// **** const reco::TrackRefVector& associatedTracks() const;
	// **** const reco::PixelClusterTagInfo* tagInfoPixelCluster(const std::string& label = "") const; // from b tagging info methods
	// **** reco::PFCandidateFwdPtrVector const& pfCandidatesFwdPtr() const { return pfCandidatesFwdPtr_; }
    if( DEBUG ) std::cout << "-----------------------------------------------------------" << std::endl;
	auto nJets = fjets.size();
	float jetHt = 0;
    if( DEBUG ) std::cout << "Starting Jet Loop for " << nJets << " jets " << std::endl;
    for ( uInt ijet(0); ijet < nJets; ijet++ ){

        const auto & jet = fjets[ijet];
        const auto jetid = fjetsID[ijet];
        if( DEBUG ) std::cout << " --- Proccesssing : " << jet  << std::endl;

        jetHt += jet.pt();

    	float jetE = jet.energy();
    	float jetM = jet.mass();
    	float jetPt = jet.pt();
    	float jetEta = jet.eta();
    	float jetPhi = jet.phi();

    	float jetNHF = jet.neutralHadronEnergyFraction();
    	float jetNEMF = jet.neutralEmEnergyFraction();
    	float jetCHF = jet.chargedHadronEnergyFraction();
    	float jetCEMF = jet.chargedEmEnergyFraction();
   	 	float jetMUF = jet.muonEnergyFraction();
   	 	float jetNHM = jet.neutralMultiplicity();
	    float jetCHM = jet.neutralMultiplicity();
	    float jetPHM = jet.chargedMultiplicity();

        JetBM.fillBranch("jetE",jetE);
        JetBM.fillBranch("jetM",jetM);
        JetBM.fillBranch("jetPt",jetPt);
        JetBM.fillBranch("jetEta",jetEta);
        JetBM.fillBranch("jetPhi",jetPhi);

        JetBM.fillBranch("jetNHF",jetNHF);
        JetBM.fillBranch("jetNEMF",jetNEMF);
        JetBM.fillBranch("jetCHF",jetCHF);
        JetBM.fillBranch("jetCEMF",jetCEMF);
        JetBM.fillBranch("jetMUF",jetMUF);
        JetBM.fillBranch("jetNHM",jetNHM);
        JetBM.fillBranch("jetCHM",jetCHM);
        JetBM.fillBranch("jetPHM",jetPHM);

        // dR matched RH group  ----------------------------------------------- 
        // --------------------------------------------------------------------

        if( DEBUG ) std::cout << "Getting jet dR rechit group" << std::endl;
		auto deltaRmin = 0.8;
		auto minRhE = 0.5;
        auto jetDrRhGroup = getRHGroup( jet.eta(), jet.phi(), deltaRmin, minRhE );
        auto jetDrRhIdsGroup = getRhGrpIDs( jetDrRhGroup );
    	JetBM.fillBranch("jetDrRhIds",jetDrRhIdsGroup);
		setRecHitUsed( jetDrRhIdsGroup );

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// !!!!!!!!  create list of associated photons and electrons with jet via daughter particles of jet !!!!!!!!!!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<uInt> jetParts;
		for( const auto &kid : jet.daughterPtrVector() ){

			auto kidcand = (pfcands_->ptrAt(kid.key())).get();
			auto kideta = kidcand->eta();
			auto kidphi = kidcand->phi();	
			auto bestdr = 0.01;
			int idx = -1;
			int cnt = 0;
			for( const auto &photon : fphotons  ){
				auto phoeta = photon.eta();
            	auto phophi = photon.phi();
				auto kidpfdr = std::sqrt(reco::deltaR2(phoeta, phophi, kideta, kidphi ));
				if( kidpfdr < bestdr ){ bestdr = kidpfdr; idx = cnt; }
				cnt++; 
			}//<<>>for( const auto photon : fphotons  )
			if( idx >= 0 ){ jetParts.push_back(idx); }
			else {
				cnt = 0;
				for( const auto &electron : felectrons  ){
                	auto eleeta = electron.eta();
                	auto elephi = electron.phi();
                	auto kidpfdr = std::sqrt(reco::deltaR2(eleeta, elephi, kideta, kidphi ));
                	if( kidpfdr < bestdr ){ bestdr = kidpfdr; idx = cnt; }
                	cnt++;
				}//<<>>for( const auto electron : felectrons  )
				if( idx >= 0 ) jetParts.push_back(1000+idx);
			}//<<>>if( phoidx >= 0 )

		}//<<>.for( const auto kid : jet.daughterPtrVector() )

		JetBM.fillBranch("jetParts",jetParts);

        // GenJet Info for MC  -------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------

        if( DEBUG ) std::cout << "Getting jetGenParton Information" << std::endl;

        //const reco::GenParticle * jetGenParton(0);
        float genImpactAngle(7.0);
        float genTime(-99.9);
        float genPt(-1.0);
        float genEta(10.0);
        float genPhi(10.0);
        float genEnergy(-1.0);
        float genEMFrac(-1.0);
        float genDrMatch(-1.0);
        //float genTimeVar(-1.0);
        float genTimeLLP(0.0);
        //float genLLPPurity(-1.0);
        //float genNextBX(-1.0);
        //float genNKids(-1.0);
        float genTOF(-99.9);

        if( hasGenInfo ){

            float goodDr(0.1);
            int matchedIdx(-1);
            int index(0);
            for(const auto &genJet : fgenjets ){

                auto gjeta = genJet.eta();
                auto gjphi = genJet.phi();
                auto jtgjdr = std::sqrt(reco::deltaR2(gjeta, gjphi, jet.eta(), jet.phi() ));
                if( jtgjdr <= goodDr ){
                    goodDr = jtgjdr;
                    matchedIdx = index;
                }//<<>>if( jtgjdr <= goodDr )
                index++;

            }//<<>>for(const auto& genJet : fgenjets ) 

            if( matchedIdx >= 0 ){

                auto genJet = fgenjets[matchedIdx];
                if( DEBUG ) std::cout << " --- Jet-GenJet dR match : " << goodDr << std::endl;

                auto nSources = genJet.numberOfSourceCandidatePtrs();
                if( DEBUG ) std::cout << " ---------------------------------------------------- " << std::endl;
                if( DEBUG ) std::cout << " - genJet srcs : " << nSources << " PV(" << vtxX << "," << vtxY << "," << vtxZ << ")" << std::endl;
                auto kids = genJet.daughterPtrVector();
				auto theta = 2*std::atan(std::exp(-1*genJet.eta()));
                auto cx = 120*std::sin(genJet.phi());
                auto cy = 120*std::cos(genJet.phi());
                auto cz = 120/std::tan(theta);
                auto tofcor = hypo( cx, cy, cz )/SOL;
                //if( DEBUG ) kidChase( kids, vtxX, vtxY, vtxZ );
                auto genKidInfo = kidTOFChain( kids, cx, cy, cz );
                if( DEBUG ) std::cout << " - genJet GenTime noTOF : " << genKidInfo[0] << " rhPos: " << cx;
				if( DEBUG ) std::cout << "," << cy << "," << cz << std::endl;
                genEta = genJet.eta();
                genPhi = genJet.phi();
                if( genKidInfo[0] > 25.0 ) genTime = -28.0;
                else if( genKidInfo[0] > -25.0 ) genTime = genKidInfo[0]-tofcor;
                else genTime = -27.0;
                genImpactAngle = genKidInfo[1];
                if( DEBUG ) std::cout << " - genJet GenTime : " << genTime << " Angle: " << genImpactAngle << std::endl;
                genPt = genJet.pt();
                genEnergy = genJet.energy();
                genEMFrac = (genJet.chargedEmEnergy() + genJet.neutralEmEnergy())/genEnergy;
                genDrMatch = goodDr; //std::sqrt(reco::deltaR2(jet.eta(), jet.phi(), genJet.eta(), genJet.phi()));
                //genTimeVar = genKidInfo[2];
                //genNextBX = genKidInfo[3];
                //genTimeLLP = genKidInfo[4];
                //genLLPPurity = genKidInfo[5];
                //genNKids = genKidInfo[6];
                genTOF = tofcor;
                if( DEBUG ) std::cout << " -- Energy : " << genEnergy << " Pt : " << genPt << " EMfrac : " << genEMFrac << std::endl;

            }//<<>>if( matchedIdx >= 0 ) 

            // load event level vectors for this jet with gen info

    		JetBM.fillBranch("jetGenImpactAngle",genImpactAngle);
    		JetBM.fillBranch("jetGenTime",genTime);
    		JetBM.fillBranch("jetGenPt",genPt);
    		JetBM.fillBranch("jetGenEta",genEta);
    		JetBM.fillBranch("jetGenPhi",genPhi);
    		JetBM.fillBranch("jetGenEnergy",genEnergy);
    		JetBM.fillBranch("jetGenDrMatch",genDrMatch);
    		JetBM.fillBranch("jetGenTimeLLP",genTimeLLP);
    		JetBM.fillBranch("jetGenTOF",genTOF);

            if( DEBUG ) std::cout << " ---------------------------------------------------- " << std::endl;

        }//<<>>if( hasGenInfo )
        if( DEBUG ) std::cout << "Next Jet .......................... " << std::endl;

    }//<<>>for ( uInt ijet = 0; ijet < nJets; ijet++ )
    // ** end of jets   ***************************************************************************************************

}//<<>>processJets( fjets, fjetsID, fgenparts )

//-----------------------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesClusterJets(){

	CaloJetBM.makeBranch("cljRhIds","cljRhIds",KUCMSBranch::VVUINT);
    CaloJetBM.makeBranch("cljPt","cljPt",KUCMSBranch::VFLOAT);
    CaloJetBM.makeBranch("cljEnergy","cljEnergy",KUCMSBranch::VFLOAT);
    CaloJetBM.makeBranch("cljPhi","cljPhi",KUCMSBranch::VFLOAT);
    CaloJetBM.makeBranch("cljEta","cljEta",KUCMSBranch::VFLOAT);
    CaloJetBM.makeBranch("cljPx","cljPx",KUCMSBranch::VFLOAT);
    CaloJetBM.makeBranch("cljPy","cljPy",KUCMSBranch::VFLOAT);
    CaloJetBM.makeBranch("cljPz","cljPz",KUCMSBranch::VFLOAT);

	CaloJetBM.initBranches(outTree);

}//<<>>setBranchesClusterJets()

void KUCMSNtupilizer::processClJet(){ 
    
	if( DEBUG ) std::cout << "Processing CaloJets" << std::endl;
    CaloJetBM.clearBranches();

    for (const auto &caloJet : fcalojets ){

        float cljPt = caloJet.pt();
        float cljEnergy = caloJet.energy();
        float cljPhi = caloJet.phi();
        float cljEta = caloJet.eta();
        float cljPx = caloJet.px();
        float cljPy = caloJet.py();
        float cljPz = caloJet.pz();

        CaloJetBM.fillBranch("cljPt",cljPt);
        CaloJetBM.fillBranch("cljEnergy",cljEnergy);
        CaloJetBM.fillBranch("cljPhi",cljPhi);
        CaloJetBM.fillBranch("cljEta",cljEta);
        CaloJetBM.fillBranch("cljPx",cljPx);
        CaloJetBM.fillBranch("cljPy",cljPy);
        CaloJetBM.fillBranch("cljPz",cljPz);

        rhGroup cljRhGroup;
        int nCljBcs(0);
        bool first = true;
        float cjeta = caloJet.eta();
        float cjphi = caloJet.phi();
        for( const auto &cluster : fbclusts ){
            auto cleta = cluster.eta();
            auto clphi = cluster.phi();
            auto cjcldr = std::sqrt(reco::deltaR2( cjeta, cjphi, cleta, clphi ));
            if( cjcldr < 0.4 ){ //std::cout << "CJ - BC Match : " << cjcldr << std::endl;
                nCljBcs++;
                if( first ){ cljRhGroup = getRHGroup( cluster, 0.5 ); first = false; }
                else { auto rhgrp = getRHGroup( cluster, 0.5 ); mrgRhGrp( cljRhGroup, rhgrp ); }
            }//<<>>if( cjcldr < 0.1 )
        }//<<>>for( const auto cluster : fbclusts )
        auto cljRhIdsGroup = getRhGrpIDs( cljRhGroup );
        CaloJetBM.fillBranch("cljRhIds",cljRhIdsGroup );
		setRecHitUsed( cljRhIdsGroup );
/*
        if( cljRhGroup.size() < 3 ){ 
            //cljCMeanTime.push_back( -29.75 ); 
            cljSeedTOFTime.push_back( -29.75 );
        } else { //<<>>if( cljRhGroup.size() < minRHcnt ) ***** IF cljRhGroup.size() > minRHcnt BELOW THIS POINT IN LOOP *****
            auto tofTimes = getLeadTofRhTime( cljRhGroup, vtxX, vtxY, vtxZ );
            //auto timeStats = getTimeDistStats( tofTimes, cljRhGroup );
            auto leadJetRh = getLeadRh( cljRhGroup );
            auto seedTOFTime = leadJetRh.time();
            cljSeedTOFTime.push_back(seedTOFTime);
            //cljCMeanTime.push_back(timeStats[6]);
            cljBcCnt.push_back(nCljBcs);
        }//<<>>if( cljRhGroup.size() < minRHcnt )
*/
        //const auto cljDrGroup = getRHGroup( cjeta, cjphi, 0.4, 2.0 );
        //if( cljDrGroup.size() < 13 ){ cljCDrMeanTime.push_back( -29.75 ); }
        //else { // <<>> if( cljRhGroup.size() < 3 )
        //    auto tofTimes = getLeadTofRhTime( cljRhGroup, vtxX, vtxY, vtxZ );
        //    auto timeStats = getTimeDistStats( tofTimes, cljRhGroup );
        //    cljCDrMeanTime.push_back(timeStats[6]);
        //}//<<>>if( cljRhGroup.size() < 3 )

    }//<<>>for (const auto caloJet : fcalojets )

}//<<>>processClJet( fcalojets )

//----------------------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesPhotons(){

    PhotonBM.makeBranch("phoIsOotPho","phoIsOotPho",KUCMSBranch::VBOOL);
    PhotonBM.makeBranch("phoExcluded", "phoExcluded",KUCMSBranch::VBOOL);
    PhotonBM.makeBranch("phoSeedTOFTime","phoSeedTOFTime",KUCMSBranch::VFLOAT);

    PhotonBM.makeBranch("phoPt","phoPt",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoEnergy","phoEnergy",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoPhi","phoPhi",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoEta","phoEta",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoPx","phoPx",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoPy","phoPy",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoPz","phoPz",KUCMSBranch::VFLOAT);

    PhotonBM.makeBranch("phoRhIds","phoRhIds",KUCMSBranch::VVUINT);
    PhotonBM.makeBranch("phoIsPixelSeed","phoIsPixelSeed",KUCMSBranch::VBOOL);
    PhotonBM.makeBranch("phoIsEB","phoIsEB",KUCMSBranch::VBOOL);

    PhotonBM.makeBranch("phoHadTowOverEM","phoHadTowOverEM",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoSigmaIEtaIEta","phoSigmaIEtaIEta",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoR9","phoR9",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoEcalRHSumEtConeDR04","phoEcalRHSumEtConeDR04",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoHcalTowerSumEtBcConeDR04","phoHcalTowerSumEtBcConeDR04",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoTrkSumPtSolidConeDR04","phoTrkSumPtSolidConeDR04",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoTrkSumPtHollowConeDR04","phoTrkSumPtHollowConeDR04",KUCMSBranch::VFLOAT);

    PhotonBM.makeBranch("phoGenIdx","phoGenIdx",KUCMSBranch::VUINT);
    PhotonBM.makeBranch("phoGenDr","phoGenDr",KUCMSBranch::VFLOAT);

    PhotonBM.makeBranch("phoSMaj","phoSMaj",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoSMin","phoSMin",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoSAlp","phoSAlp",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoCovEtaEta","phoCovEtaEta",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoCovEtaPhi","phoCovEtaPhi",KUCMSBranch::VFLOAT);
    PhotonBM.makeBranch("phoCovPhiPhi","phoCovPhiPhi",KUCMSBranch::VFLOAT);

    PhotonBM.initBranches(outTree);

}//<<>>setBranchesPhotons()

void KUCMSNtupilizer::processPhotons(){ 

    if( DEBUG ) std::cout << "Processing Photons" << std::endl;

	PhotonBM.clearBranches();

    for( const auto &photon : fphotons ){

        float phoPt = photon.pt();
        float phoEnergy = photon.energy();
        float phoPhi = photon.phi();
        float phoEta = photon.eta();
        float phoPx = photon.px();
        float phoPy = photon.py();
        float phoPz = photon.pz();

        //float phoHadOverEM = photon.hadronicOverEm();
        //float phoHadOverEMVaid = photon.hadronicOverEmValid();
        float phoHadTowOverEM = photon.hadTowOverEm();
        //float phoHadTowOverEMValid = photon.hadTowOverEmValid();
        //float phoMaxEnergyXtal = photon.maxEnergyXtal();
        //float phoSigmaEtaEta = photon.sigmaEtaEta();
        float phoSigmaIEtaIEta = photon.sigmaIetaIeta();

        //float phoR1x5 = photon.r1x5();
        //float phoR2x5 = photon.r2x5();
        float phoR9 = photon.r9();

        float phoEcalRHSumEtConeDR04 = photon.ecalRecHitSumEtConeDR04();
        //float phoHcalTwrSumEtConeDR04 = photon.hcalTowerSumEtConeDR04();
        float phoHcalTowerSumEtBcConeDR04 = photon.hcalTowerSumEtBcConeDR04();
        float phoTrkSumPtSolidConeDR04 = photon.trkSumPtSolidConeDR04();
        float phoTrkSumPtHollowConeDR04 = photon.trkSumPtHollowConeDR04();
        //float phoNTrkSolidConeDR04 = photon.nTrkSolidConeDR04();
        //float phoNTrkHollowConeDR04 = photon.nTrkHollowConeDR04();

        PhotonBM.fillBranch("phoPt",phoPt);
        PhotonBM.fillBranch("phoEnergy",phoEnergy);
        PhotonBM.fillBranch("phoPhi",phoPhi);
        PhotonBM.fillBranch("phoEta",phoEta);
        PhotonBM.fillBranch("phoPx",phoPx);
        PhotonBM.fillBranch("phoPy",phoPy);
        PhotonBM.fillBranch("phoPz",phoPz);

        //PhotonBM.fillBranch("phoHadOverEM",phoHadOverEm);
        //PhotonBM.fillBranch("phoHadOverEMVaid",phoHadOverEmValid);
        PhotonBM.fillBranch("phoHadTowOverEM",phoHadTowOverEM);
        //PhotonBM.fillBranch("phohadTowOverEMValid",phoHadTowOverEmValid);
        //PhotonBM.fillBranch("phoMaxEnergyXtal",phoMaxEnergyXtal);
        //PhotonBM.fillBranch("phoSigmaEtaEta",phoSigmaEtaEta);
        PhotonBM.fillBranch("phoSigmaIEtaIEta",phoSigmaIEtaIEta);

        //PhotonBM.fillBranch("phoR1x5",phoR1x5);
        //PhotonBM.fillBranch("phoR2x5",phoR2x5);
        PhotonBM.fillBranch("phoR9",phoR9);

        PhotonBM.fillBranch("phoEcalRHSumEtConeDR04",phoEcalRHSumEtConeDR04);
        //PhotonBM.fillBranch("phoHcalTwrSumEtConeDR04",phoHcalTwrSumEtConeDR04);
        PhotonBM.fillBranch("phoHcalTowerSumEtBcConeDR04",phoHcalTowerSumEtBcConeDR04);
        PhotonBM.fillBranch("phoTrkSumPtSolidConeDR04",phoTrkSumPtSolidConeDR04);
        PhotonBM.fillBranch("phoTrkSumPtHollowConeDR04",phoTrkSumPtHollowConeDR04);
        //PhotonBM.fillBranch("phoNTrkSolidConeDR04",phoNTrkSolidConeDR04);
        //PhotonBM.fillBranch("phoNTrkHollowConeDR04",phoNTrkHollowConeDR04);

        if( DEBUG ) std::cout << " --- Proccesssing : " << photon << std::endl;
        const auto &phosc = photon.superCluster().isNonnull() ? photon.superCluster() : photon.parentSuperCluster();
        const auto scptr = phosc.get();
        const auto &seedDetId = scptr->seed()->seed();// seed detid
        const auto isEB = (seedDetId.subdetId() == EcalBarrel);// which subdet
       //const auto recHits = ( isEB ? &(*recHitsEB_) : &(*recHitsEE_) );
        const auto recHits = ( isEB ? recHitsEB_.product() : recHitsEE_.product() );

        if( DEBUG ) std::cout << " --- Finding Moments & Covariences : " << scptr << std::endl;
        const auto ph2ndMoments = noZS::EcalClusterTools::cluster2ndMoments( *(scptr->seed()), *recHits );
        const auto lCov = EcalClusterTools::covariances( *(scptr->seed()), recHits, ecalTopology, ecalGeometry );
        if( DEBUG ) std::cout << " --- Storing Moments & Covariences : " << scptr << std::endl;

        float phoSMaj = ph2ndMoments.sMaj;
        float phoSMin = ph2ndMoments.sMin;
        float phoSAlp = ph2ndMoments.alpha;
        float phoCovEtaEta = lCov[0];
        float phoCovEtaPhi = lCov[1];
        float phoCovPhiPhi = lCov[2];

        PhotonBM.fillBranch("phoSMaj",phoSMaj);
        PhotonBM.fillBranch("phoSMin",phoSMin);
        PhotonBM.fillBranch("phoSAlp",phoSAlp);
        PhotonBM.fillBranch("phoCovEtaEta",phoCovEtaEta);
        PhotonBM.fillBranch("phoCovEtaPhi",phoCovEtaPhi);
        PhotonBM.fillBranch("phoCovPhiPhi",phoCovPhiPhi);

		if( DEBUG ) std::cout << " --- Gathering SC info : " << scptr << std::endl;
        scGroup phoSCGroup{*scptr};
        auto phoRhGroup = getRHGroup( phoSCGroup, 0.5 );
        auto phoRhIdsGroup = getRhGrpIDs( phoRhGroup );
        PhotonBM.fillBranch("phoRhIds",phoRhIdsGroup);
		setRecHitUsed(phoRhIdsGroup);
        if( DEBUG ) std::cout << " -- gedPhotons : " << scptr << " #: " << phoRhGroup.size() << std::endl;
        //auto tofTimes = getLeadTofRhTime( phoRhGroup, vtxX, vtxY, vtxZ );
        //auto timeStats = getTimeDistStats( tofTimes, phoRhGroup );
        auto seedTOFTime = getSeedTofTime( *scptr, vtxX, vtxY, vtxZ );
        //auto phoLeadTOFTime =  getLeadTofTime( phoRhGroup, vtxX, vtxY, vtxZ );
        PhotonBM.fillBranch("phoSeedTOFTime",seedTOFTime);
        //PhotonBM.fillBranch("phoCMeanTime",timeStats[6]);

        // GenParticle Info for photon  -------------------------------------------------------------------
        if( hasGenInfo ){

            auto genInfo = getGenPartMatch( scptr, fgenparts );
            if( DEBUG) std::cout << " Photon Match ------------------------- " << std::endl;
            if( DEBUG) std::cout << " Matched Eta: " <<  photon.eta() << " gen: " << genInfo[4] << " match: " << genInfo[2] << std::endl;
            if( DEBUG) std::cout << " Matched Phi: " <<  photon.phi() << " gen: " << genInfo[5] << " match: " << genInfo[3] << std::endl;
            if( DEBUG ) std::cout << " ---------------------------------------------------- " << std::endl;
            PhotonBM.fillBranch("phoGenIdx",genInfo[1]);
            PhotonBM.fillBranch("phoGenDr",genInfo[0]);

        }//<<>>if( hasGenInfo )

    }//<<>>for( const auto &photon : fPhotons )

}//<<>>processPhotons( fphotons, fgenparts )

//-------------------------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesElectrons(){

    ElectronBM.makeBranch("eleRhIds","eleRhIds",KUCMSBranch::VVUINT); 
    ElectronBM.makeBranch("eleSeedTOFTime","eleSeedTOFTime",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("elePt","elePt",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("eleEnergy","eleEnergy",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("elePhi","elePhi",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("eleEta","eleEta",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("elePx","elePx",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("elePy","elePy",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("elePz","elePz",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("eleGenIdx","eleGenIdx",KUCMSBranch::VUINT);
    ElectronBM.makeBranch("eleGenDr","eleGenDr",KUCMSBranch::VFLOAT);
    ElectronBM.makeBranch("eleIdBools","eleIdBools",KUCMSBranch::VBOOL);

    ElectronBM.initBranches(outTree);

}//<<>>setBranchesElectrons()

void KUCMSNtupilizer::processElectrons(){ 

	ElectronBM.clearBranches();

    scGroup jetEleSCGroup;
    if( DEBUG ) std::cout << "Processing Electrons" << std::endl;
    for( const auto &electron : felectrons ){

        float elePt = electron.pt();
        float eleEnergy = electron.energy();
        float elePhi = electron.phi();
        float eleEta = electron.eta();
        float elePx = electron.px();
        float elePy = electron.py();
        float elePz = electron.pz();

        ElectronBM.fillBranch("elePt",elePt);
        ElectronBM.fillBranch("eleEnergy",eleEnergy);
        ElectronBM.fillBranch("elePhi",elePhi);
        ElectronBM.fillBranch("eleEta",eleEta);
        ElectronBM.fillBranch("elePx",elePx);
        ElectronBM.fillBranch("elePy",elePy);
        ElectronBM.fillBranch("elePz",elePz);

        if( DEBUG ) std::cout << " --- Proccesssing : " << electron << std::endl;
        const auto &elesc = electron.superCluster().isNonnull() ? electron.superCluster() : electron.parentSuperCluster();
        const auto scptr = elesc.get();
        scGroup eleSCGroup{*scptr};
        auto eleRhGroup = getRHGroup( eleSCGroup, 0.5 );
        auto eleRhIdsGroup = getRhGrpIDs( eleRhGroup );
        ElectronBM.fillBranch("eleRhIds",eleRhIdsGroup );
		setRecHitUsed( eleRhIdsGroup );
        if( DEBUG ) std::cout << " -- Electrons : " << scptr << " #: " << eleRhGroup.size() << std::endl;
        auto tofTimes = getLeadTofRhTime( eleRhGroup, vtxX, vtxY, vtxZ );
        //auto timeStats = getTimeDistStats( tofTimes, eleRhGroup );
        float seedTOFTime = getSeedTofTime( *scptr, vtxX, vtxY, vtxZ );

        ElectronBM.fillBranch("eleSeedTOFTime",seedTOFTime);

        // GenParticle Info for electron  -------------------------------------------------------------------
        if( hasGenInfo ){

			auto genInfo = getGenPartMatch( scptr, fgenparts );
            if( DEBUG) std::cout << " Electron Match ------------------------- " << std::endl;
            if( DEBUG) std::cout << " Matched Eta: " <<  electron.eta() << " gen: " << genInfo[4] << " match: " << genInfo[2] << std::endl;
            if( DEBUG) std::cout << " Matched Phi: " <<  electron.phi() << " gen: " << genInfo[5] << " match: " << genInfo[3] << std::endl;
            if( DEBUG ) std::cout << " ---------------------------------------------------- " << std::endl;
            ElectronBM.fillBranch("eleGenIdx",genInfo[1]);
            ElectronBM.fillBranch("eleGenDr",genInfo[0]);

        }//<<>>if( hasGenInfo )

    }//<<>>for( const auto electron : *electrons_ )

}//<<>>processElectrons( felectrons, fgenparts )

//----------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesGenParts(){

    GenParticlesBM.makeBranch("genPt","genPt",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genEnergy","genEnergy",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genPhi","genPhi",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genEta","genEta",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genPx","genPx",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genPy","genPy",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genPz","genPz",KUCMSBranch::VFLOAT);
    GenParticlesBM.makeBranch("genPdgId","genPdgId",KUCMSBranch::VUINT);

	GenParticlesBM.initBranches(outTree);

}//<<>>setBranchesGenParts()

void KUCMSNtupilizer::processGenPart(){

    if( DEBUG ) std::cout << "Processing GenParticles" << std::endl;
	GenParticlesBM.clearBranches();

    if( DEBUG ) std::cout << " - enetering Genpart loop" << std::endl;
    for (const auto &genpart : fgenparts ){

        float genPt = genpart.pt();
        float genEnergy = genpart.energy();
        float genPhi = genpart.phi();
        float genEta = genpart.eta();
        uInt genPdgId = genpart.pdgId();
        float genPx = genpart.px();
        float genPy = genpart.py();
        float genPz = genpart.pz();

        GenParticlesBM.fillBranch("genPt",genPt);
        GenParticlesBM.fillBranch("genEnergy",genEnergy);
        GenParticlesBM.fillBranch("genPhi",genPhi);
        GenParticlesBM.fillBranch("genEta",genEta);
        GenParticlesBM.fillBranch("genPdgId",genPdgId);
        GenParticlesBM.fillBranch("genPx",genPx);
        GenParticlesBM.fillBranch("genPy",genPy);
        GenParticlesBM.fillBranch("genPz",genPz);

    }//<<>> for (const auto genpart : fgenparts )

}//<<>>processGenPart( std::vector<reco::GenParticle>  fgenparts )

//-----------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesRecHits(){

    EcalRecHitBM.makeBranch("rhEnergy","rhEnergy",KUCMSBranch::VFLOAT);
    EcalRecHitBM.makeBranch("rhTime","rhTime",KUCMSBranch::VFLOAT);
    EcalRecHitBM.makeBranch("rhTOF","rhTOF",KUCMSBranch::VFLOAT);
    EcalRecHitBM.makeBranch("rhID","rhID",KUCMSBranch::VUINT);
    EcalRecHitBM.makeBranch("rhisOOT","rhisOOT",KUCMSBranch::VBOOL);
    EcalRecHitBM.makeBranch("rhSwCross","rhSwCross",KUCMSBranch::VFLOAT);	

	EcalRecHitBM.initBranches(outTree);

}//<<>>setBranchesRecHits()

void KUCMSNtupilizer::processRecHits( float minRHEf ){

    if( DEBUG ) std::cout << "Processing RecHits" << std::endl;
    
	EcalRecHitBM.clearBranches();
	
	if( DEBUG ) std::cout << " - enetering RecHit loop" << std::endl;

	int nRecHits = frechits.size();
    for ( int it = 0; it < nRecHits; it++ ){

		auto recHit = frechits[it];
		auto used = frhused[it];
        //if( not used ) continue;
		if( not used && recHit.energy() < minRHEf ) continue;

        const auto recHitID = getRawID(recHit);
        const bool isEB = getIsEB(recHit); // which subdet
        const auto geometry( isEB ? barrelGeometry : endcapGeometry );
        const auto recHitPos = geometry->getGeometry(recHit.detid())->getPosition();
        const auto rhX = recHitPos.x();
        const auto rhY = recHitPos.y();
        const auto rhZ = recHitPos.z();
        const float d_rh = hypo(rhX,rhY,rhZ)/SOL;
        const float d_pv = hypo(rhX-vtxX,rhY-vtxY,rhZ-vtxZ)/SOL;
        //float swisscross(0.0);
        //if( isEB ) swisscross = EcalTools::swissCross(recHitID, *recHitsEB_, 0.0, true);
        //else swisscross = EcalTools::swissCross(recHitID, *recHitsEE_, 0.0, true);
		const float swisscross = (isEB) ? 
				EcalTools::swissCross(recHitID, *recHitsEB_, 0.0, true) : EcalTools::swissCross(recHitID, *recHitsEE_, 0.0, true);

        //const auto laser = laser_->getLaserCorrection(recHitID,evTime);
        //const auto interCalibIter = interCalibMap->find(recHitID);
        //const auto interCalib = ((interCalibIter != interCalibMap->end()) ? (*interCalibIter) : - 1.f);
        //const float adcToGeV( isEB ? adcToGeVEB : adcToGeVEE );
        //const auto & pediter = pedestals_->find(recHitID);

		const float rhTime = recHit.time();
		const bool rhIsOOT = recHit.checkFlag(EcalRecHit::kOutOfTime);
		const float rhEnergy = recHit.energy();
		const float rhAdjTime = rhTime-d_rh;  // Note : Margret adds d_rh to the time in her code & subtracts d_pv ( or TOF )

        EcalRecHitBM.fillBranch("rhID",recHitID);
        EcalRecHitBM.fillBranch("rhTOF",d_pv);
        EcalRecHitBM.fillBranch("rhTime",rhAdjTime);
        EcalRecHitBM.fillBranch("rhisOOT",rhIsOOT);
        EcalRecHitBM.fillBranch("rhEnergy",rhEnergy);
        EcalRecHitBM.fillBranch("rhSwCross",swisscross);
        //EcalRecHitBM.fillBranch("rhisWeird",recHit.checkFlag(EcalRecHit::kWeird));
        //EcalRecHitBM.fillBranch("rhisDiWeird",recHit.checkFlag(EcalRecHit::kDiWeird));

    }//<<>>for (const auto recHit : *recHitsEB_ )   

}//<<>>processRecHits( std::vector<EcalRecHit> frechits )


////////////////////////////////////////////////////////////////////////////////////////////////////
// ---------------- CMSSW Ana Helper Functions ---------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////

int KUCMSNtupilizer::getPFJetID(const reco::PFJet & jet){

    const auto eta  = std::abs(jet.eta());
    const auto NHF  = jet.neutralHadronEnergyFraction();
    const auto NEMF = jet.neutralEmEnergyFraction();
    const auto CHF  = jet.chargedHadronEnergyFraction();
    const auto CEMF = jet.chargedEmEnergyFraction();
    const auto NHM  = jet.neutralMultiplicity();
    const auto CHM  = jet.chargedMultiplicity();
    const auto SHM  = jet.chargedMultiplicity()+jet.neutralMultiplicity();
    const auto MUF  = jet.muonEnergyFraction();

    int tighter = 3;
    int tightLepVeto = 0;
    int tight = 2;
    int loose = 1;

    bool nhfup  = NHF  < 0.90;
    bool nhflw  = NHF  > 0.2;

    bool nemfup1 = NEMF < 0.90;
    bool nemfup2 = NEMF < 0.99;
    bool nemf80 = NEMF < 0.80;
    bool nemflw = NEMF > 0.01;
    bool nemf10 = NEMF > 0.1;

    bool shm1  = SHM  > 1;
    bool muf8  = MUF  < 0.80;
    bool chf0  = CHF  > 0;
    bool chf10  = CHF  > 0.10;
    bool chm0  = CHM  > 0;
    bool cemf8 = CEMF > 0.80;
    bool nhm2  = NHM  > 1;
    bool nhm10 = NHM  > 10;

    bool eta1 = eta <= 2.6;
    bool eta2 = eta <= 2.7;
    bool eta3 = eta <= 3.0;

    if (eta1){
        if      (nhfup && nemfup1 && shm1 && muf8 && chf0 && chm0 && cemf8) return tightLepVeto;
        else if (nhfup && nemf80 && shm1 && chf10 && chm0) return tighter;
        else if (nhfup && nemfup1 && shm1 && chf0 && chm0) return tight;
        else    return loose;
    } else if (!eta1 && eta2 ){ //<<>>if (eta1)
        if      (nhfup && nemfup2 && chm0 && muf8 && cemf8) return tightLepVeto;
        else if (nhfup && nemf80 && chm0) return tighter;
        else if (nhfup && nemfup2 && chm0) return tight;
        else    return loose;
    } else if (!eta2 && eta3){ //<<>>if (eta1) : else if
        if      (nemf10 && nemf80 && nhm2) return tighter;
        else if (nemflw && nemfup2 && nhm2) return tight;
        else    return loose;
    } else { //<<>>if (eta1) : else if : else if
        if      (nhflw && nemfup1 && nhm10) return tight;
        else    return loose;
    }//<<>>if (eta1) : else if : else if : else

    return -1; // should not happen

}//>>>>int KUCMSNtupilizer::getPFJetID(const reco::PFJet & jet)

rhGroup KUCMSNtupilizer::getRHGroup( float eta, float phi, float drmin, float minenr = 0.0 ){

    rhGroup result;
    for (const auto &recHit : *recHitsEB_ ){
        if( recHit.checkFlag(EcalRecHit::kHasSwitchToGain6) ) continue;
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId(recHit.detid());
        const auto recHitPos = barrelGeometry->getGeometry(recHitId)->getPosition();
        const auto dr = std::sqrt(reco::deltaR2(eta, phi, recHitPos.eta(), recHitPos.phi()));
        if( dr > drmin ) continue;
        result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEB_ )
    for (const auto &recHit : *recHitsEE_ ){
        if( recHit.checkFlag(EcalRecHit::kHasSwitchToGain6) ) continue;
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId(recHit.detid());
        const auto recHitPos = endcapGeometry->getGeometry(recHitId)->getPosition();
        const auto dr = std::sqrt(reco::deltaR2(eta, phi, recHitPos.eta(), recHitPos.phi()));
        if( dr > drmin ) continue;
        result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEE_ )

    return result;

}//>>>>rhGroup KUCMSNtupilizer::getRHGroup( float eta, float phi, float drmin, float minenr = 0.0 )

rhGroup KUCMSNtupilizer::getRHGroup( uInt detid ){

    rhGroup result;
    for (const auto &recHit : *recHitsEB_ ){
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( detid != rawId ) continue;
        result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEB_ )
    for (const auto &recHit : *recHitsEE_ ){
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( detid != rawId ) continue;
        result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEE_ )

    return result;
}//>>>>rhGroup KUCMSNtupilizer::getRHGroup( uInt detid )

rhGroup KUCMSNtupilizer::getRHGroup(){

    rhGroup result;
    for (const auto &recHit : *recHitsEB_ ) result.push_back(recHit);
    for (const auto &recHit : *recHitsEE_ ) result.push_back(recHit);

    return result;

}//>>>>rhGroup KUCMSNtupilizer::getRHGroup()


rhGroup KUCMSNtupilizer::getRHGroup( const scGroup superClusterGroup, float minenr ){

    rhGroup result;
    vector<uInt> rawIds;
    for ( const auto &superCluster : superClusterGroup ){
        auto & hitsAndFractions = superCluster.hitsAndFractions();
        const auto nHAF = hitsAndFractions.size();
        for( uInt iHAF = 0; iHAF < nHAF; iHAF++ ){
            const auto detId = hitsAndFractions[iHAF].first;
            const auto rawId = detId.rawId();
            if( std::find( rawIds.begin(), rawIds.end(), rawId ) == rawIds.end() ) rawIds.push_back(rawId);
        }//<<>>for( uInt iHAF = 0; iHAF < nHAF; iHAF++ )
    }//<<>>for ( const auto superCluster : superClusterGroup )  
    for (const auto &recHit : *recHitsEB_ ){
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) != rawIds.end() ) result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEB_ )
    for (const auto &recHit : *recHitsEE_ ){
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) != rawIds.end() ) result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEE_ )

    return result;

}//>>>>rhGroup KUCMSNtupilizer::getRHGroup( const scGroup superClusterGroup, float minenr = 0.0 )

rhGroup KUCMSNtupilizer::getRHGroup( const scGroup superClstrGrp, float minenr, vector<float> phEnergy, vector<float> phDr, float phEnMax ){

    rhGroup result;
    vector<uInt> rawIds;

    float dRcut = 0.05;

    int iter = -1;
    for ( const auto &superCluster : superClstrGrp ){
        iter++;
        if( phDr[iter] > dRcut ) continue;
        if( phEnergy[iter] != phEnMax ) continue;
        auto & hitsAndFractions = superCluster.hitsAndFractions();
        const auto nHAF = hitsAndFractions.size();
        for( uInt iHAF = 0; iHAF < nHAF; iHAF++ ){
            const auto detId = hitsAndFractions[iHAF].first;
            const auto rawId = detId.rawId();
            if( std::find( rawIds.begin(), rawIds.end(), rawId ) == rawIds.end() ) rawIds.push_back(rawId);
        }//<<>>for( uInt iHAF = 0; iHAF < nHAF; iHAF++ )
    }//<<>>for ( const auto superCluster : superClstrGrp )   
    for (const auto &recHit : *recHitsEB_ ){
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) != rawIds.end() ) result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEB_ )
    for (const auto &recHit : *recHitsEE_ ){
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) != rawIds.end() ) result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEE_ )

    return result;

}//>>>>rhGroup KUCMSNtupilizer::getRHGroup( const scGroup superClstrGrp, float minenr, vector<float> phEnergy, vector<float> phDr, float phEnMax )

rhGroup KUCMSNtupilizer::getRHGroup( const reco::CaloCluster basicCluster, float minenr = 0.0 ){

    rhGroup result;
    vector<uInt> rawIds;
    auto & hitsAndFractions = basicCluster.hitsAndFractions();
    const auto nHAF = hitsAndFractions.size();
    //std::cout << " --- getRHGroup for basic cluster with " << nHAF << " rechits " << std::endl;
    for( uInt iHAF = 0; iHAF < nHAF; iHAF++ ){
        const auto detId = hitsAndFractions[iHAF].first;
        const auto rawId = detId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) == rawIds.end() ) rawIds.push_back(rawId);
    }//<<>>for( uInt iHAF = 0; iHAF < nHAF; iHAF++ )
    for (const auto &recHit : *recHitsEB_ ){
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) != rawIds.end() ) result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEB_ )
    for (const auto &recHit : *recHitsEE_ ){
        auto enr = recHit.energy();
        if( enr <= minenr ) continue;
        const auto recHitId = recHit.detid();
        const auto rawId = recHitId.rawId();
        if( std::find( rawIds.begin(), rawIds.end(), rawId ) != rawIds.end() ) result.push_back(recHit);
    }//<<>>for (const auto recHit : *recHitsEE_ )       

    return result;

}////rhGroup KUCMSNtupilizer::getRHGroup( const reco::CaloCluster basicCluster, float minenr = 0.0 )

vector<float> KUCMSNtupilizer::getRhTofTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ ){

    vector<float> result;
	if( recHits.size() == 0 ){ result.push_back(-99.0); return result; }
    for (const auto &recHit : recHits ){
        const auto rht = recHit.time();
        //std::cout << " ----- Get TOF Time rh time: " << rht << std::endl;
        const auto recHitId(recHit.detid());
        const auto recHitPos = barrelGeometry->getGeometry(recHitId)->getPosition();
        const auto rhPosX = recHitPos.x();
        const auto rhPosY = recHitPos.y();
        const auto rhPosZ = recHitPos.z();
        //std::cout << " ----- Get TOF Time rh POS: " << rhPosX << " " <<   rhPosY << " " << rhPosZ << std::endl;
        const auto d_rh = hypo(rhPosX,rhPosY,rhPosZ);
        const auto d_pv = hypo(rhPosX-vtxX,rhPosY-vtxY,rhPosZ-vtxZ);
        const auto tof = (d_rh-d_pv)/SOL;
        //std::cout << " ----- Get TOF Time rh tof: " << tof << std::endl;
        result.push_back(rht-tof);
    }//<<>>for (const auto recHit : recHits )   

    return result;

}//>>>>vector<float>  KUCMSNtupilizer::getRhTofTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ )

float KUCMSNtupilizer::getRhTOF( EcalRecHit rechit, double vtxX, double vtxY, double vtxZ ){

    const auto recHitId(rechit.detid());
    const auto recHitPos = barrelGeometry->getGeometry(recHitId)->getPosition();
    const auto rhPosX = recHitPos.x();
    const auto rhPosY = recHitPos.y();
    const auto rhPosZ = recHitPos.z();
    const auto d_rh = hypo(rhPosX,rhPosY,rhPosZ);
    const auto d_pv = hypo(rhPosX-vtxX,rhPosY-vtxY,rhPosZ-vtxZ);
    const auto tof = (d_rh-d_pv)/SOL;
    return tof;

}//>>>>>float getRhTOF( EcalRecHit rechit, double vtxX, double vtxY, double vtxZ )

EcalRecHit KUCMSNtupilizer::getLeadRh( rhGroup recHits ){

    EcalRecHit result;
    float enr(0.0);
    for (const auto &recHit : recHits ){
        auto rhenr = recHit.energy();
        if( rhenr < enr ) continue;
        enr = rhenr;
        result = recHit;
    }//<<>>for (const auto recHit : recHits )

    return result;

}//>>>>EcalRecHit KUCMSNtupilizer::getLeadRh( rhGroup recHits )

vector<float> KUCMSNtupilizer::getLeadTofRhTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ ){

    vector<float> result;
	if( recHits.size() == 0 ){ result.push_back(-99.0); return result; }
    auto lrh = getLeadRh(recHits);
    const auto recHitId(lrh.detid());
    const auto recHitPos = barrelGeometry->getGeometry(recHitId)->getPosition();
    const auto rhPosX = recHitPos.x();
    const auto rhPosY = recHitPos.y();
    const auto rhPosZ = recHitPos.z();
    const auto d_rh = hypo(rhPosX,rhPosY,rhPosZ);
    const auto d_pv = hypo(rhPosX-vtxX,rhPosY-vtxY,rhPosZ-vtxZ);
    const auto tof = (d_rh-d_pv)/SOL;
    for (const auto &recHit : recHits ){result.push_back(recHit.time()-tof);}
    return result;

}//>>>>vector<float>  KUCMSNtupilizer::getLeadTofRhTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ )

float KUCMSNtupilizer::getSeedTofTime( reco::SuperCluster sprclstr, double vtxX, double vtxY, double vtxZ ){

    float rhTime(-100.0);
    const auto seedDetId = sprclstr.seed().get()->seed(); // seed detid
    const auto seedRawId = seedDetId.rawId(); // crystal number
    const auto isEB = (seedDetId.subdetId() == EcalSubdetector::EcalBarrel); // which subdet
    const auto recHits = (isEB ? *recHitsEB_ : *recHitsEE_ );
    for( const auto &rechit : recHits ){
        const auto recHitId = rechit.detid();
        const auto rawId = recHitId.rawId();
        if( rawId == seedRawId ){ rhTime = rechit.time(); continue; }
    }//<<>>for (const auto recHit : *recHitsEE_ ) 

    const auto recHitPos = barrelGeometry->getGeometry(seedDetId)->getPosition();
    const auto rhPosX = recHitPos.x();
    const auto rhPosY = recHitPos.y();
    const auto rhPosZ = recHitPos.z();
    const auto d_rh = hypo(rhPosX,rhPosY,rhPosZ);
    const auto d_pv = hypo(rhPosX-vtxX,rhPosY-vtxY,rhPosZ-vtxZ);
    const auto tof = (d_rh-d_pv)/SOL;
    const auto seedTofTime = rhTime - tof;
    return seedTofTime;

}//>>>>float  KUCMSNtupilizer::getSeedTofTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ )

float KUCMSNtupilizer::getLeadTofTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ ){

	if( recHits.size() == 0 ) return -99.0;
    auto lrh = getLeadRh(recHits);
    const auto recHitId(lrh.detid());
    const auto recHitPos = barrelGeometry->getGeometry(recHitId)->getPosition();
    const auto rhPosX = recHitPos.x();
    const auto rhPosY = recHitPos.y();
    const auto rhPosZ = recHitPos.z();
    const auto d_rh = hypo(rhPosX,rhPosY,rhPosZ);
    const auto d_pv = hypo(rhPosX-vtxX,rhPosY-vtxY,rhPosZ-vtxZ);
    const auto tof = (d_rh-d_pv)/SOL;
    return lrh.time()-tof;

}//>>>>float  KUCMSNtupilizer::getLeadTofTime( rhGroup recHits, double vtxX, double vtxY, double vtxZ )

/*  Not need at this level
vector<float> KUCMSNtupilizer::getTimeDistStats( vector<float> times, rhGroup rechits ){

	//if( rechits.size() == 0 ){ vector<float> result{-99.0}; return result; }

    // N 3.64, C 0.3000  s^2 = (N/(rhe))^2 + 2C^2
    float N(3.64);
    float C(0.3000);

    vector<float> wts;
    for( uInt it(0); it < rechits.size(); it++ ){
    //auto wt = 1/std::sqrt(sq2(N/rechits[it].energy())+2*sq2(C)); 
        auto wt = 1/(sq2(N/rechits[it].energy())+2*sq2(C));
        wts.push_back(wt);
    }//<<>>for( uInt it(0); it < rechits.size(); it++ )

    return getDistStats( times, wts );

}//>>>>vector<float> KUCMSNtupilizer::getTimeDistStats( vector<float> times, rhGroup rechits ){
*/

void KUCMSNtupilizer::mrgRhGrp( rhGroup & x, rhGroup & y ){

    //std::cout << " --- In mrgRhGrp " << std::endl;
    bool matched(false);
    for(const auto rhy : y ){
        for(const auto rhx : x ){ if( rhMatch(rhx,rhy) ) matched = true; }
        if( not matched ) x.push_back(rhy);
    }//<<>>for(const auto rhx : y )
   //std::cout << " ---- Finished mrgRhGrp " << std::endl;

    return;

}//>>>>rhGroup KUCMSNtupilizer::mrgRhGrp(const rhGroup x, const rhGroup y)

bool KUCMSNtupilizer::reduceRhGrps( vector<rhGroup> & x ){

    //std::cout << " --- In reduceRhGrps " << std::endl;
    uInt s = x.size();
    uInt a(0), b(0);
    bool match(false);
    for( uInt ita(0); ita < s; ita++ ){
        for( auto itb = ita+1; itb < s; itb++ ){
            if( dupRhFnd(x[ita], x[itb]) ){ a = ita; b = itb; match = true; }
            if(match) break;
    }//<<>>for( int itb = ita+1; itb < s; itb++ ) 
        if(match) break;
    }//<<>>for( int ita = 0; ita < nBcRhGroups; ita++ )
    if(match){
        mrgRhGrp( x[a], x[b] );
        x.erase(x.begin()+b);
    } //<<>>if(match) : else
    //std::cout << " ---- Finished reduceRhGrps " << std::endl;

    return match;

}//>>>>vector<rhGroup> KUCMSNtupilizer::reduceRhGrps(const vector<rhGroup> x)

string KUCMSNtupilizer::bigKidChase( std::vector<reco::CandidatePtr> kids, float vx ){

    std::string result("");
    float maxpt(0.0);
    const reco::Candidate* bigKid(NULL);
    for( auto &kid : kids ){
        if( kid->pt() > maxpt ){
            maxpt = kid->pt();
            bigKid = kid.get();
        }//<<>>if( kid->pt() > maxpt )
    }//<<>>for( auto kid : kids )
    result += std::string(" -- BigKid : pdgID : ") + bigKid->pdgId() + std::string(" pt : ") + bigKid->pt();
    result += std::string(" vtx : ") + bigKid->mother(0)->vx() + std::string(" / ") + vx ;
    return result;

}//<<>>string bigKidChase( std::vector<reco::CandidatePtr> kids, float vx )

void KUCMSNtupilizer::motherChase( const reco::Candidate* kid, string & depth ){

    depth += "-";
    for( long unsigned int gmit(0); gmit < kid->numberOfMothers(); gmit++ ){
        std::cout <<  depth  << " gMomID : " << kid->mother(gmit)->pdgId() << " pt : " << kid->mother(gmit)->pt();
        std::cout << " Vertix (" << kid->mother(gmit)->vx() << "," << kid->mother(gmit)->vy() << "," << kid->mother(gmit)->vz() << ")";
        std::cout << " nGMothers " << kid->mother(gmit)->numberOfMothers() << std::endl;
    }//<<>>for( long unsigned int gmit(0); gmit < nKMother; gmit++ )
    if( not depth.empty() ) depth = depth.substr (0,depth.length()-1);

}//<<>> void MotherChase( Candidate* kid, string depth  )

int KUCMSNtupilizer::llpGenChase( const reco::Candidate* kid ){

    auto match = ( std::abs(kid->pdgId()) > 1000020 ) && ( std::abs(kid->pdgId()) < 1000040 );
    if( match ) return kid->pdgId();
    for( long unsigned int gmit(0); gmit < kid->numberOfMothers(); gmit++ ){
        if( kid->mother(gmit)->numberOfMothers() > 0 ){ auto r = llpGenChase(kid->mother(gmit)); if(r != 0) return r; }
    }//<<>>for( long unsigned int gmit(0); gmit < nKMother; gmit++ )
    return 0;

}//<<>> void llpChase( Candidate* kid )

int KUCMSNtupilizer::llpGenChase( const reco::GenParticle kid ){

    auto match = ( std::abs(kid.pdgId()) > 1000020 ) && ( std::abs(kid.pdgId()) < 1000040 );
    auto hard = kid.isLastCopy();
    if( match && hard ) return kid.pdgId();
    for( long unsigned int gmit(0); gmit < kid.numberOfMothers(); gmit++ ){
        //if( kid->pdgId() == 6000113 ) return true;
        if( kid.mother(gmit)->numberOfMothers() > 0 ){ auto r = llpGenChase(kid.mother(gmit)); if(r != 0) return r; }
    }//<<>>for( long unsigned int gmit(0); gmit < nKMother; gmit++ )
    return 0;

}//<<>> void llpChase( Candidate* kid )

int KUCMSNtupilizer::llpGenChaseP( const reco::Candidate* kid, int depth ){

    int nMoms = kid->numberOfMothers();
    if( ! nMoms ) return 999;
    int genPartID = 1000;
    auto kidPdgID = std::abs(kid->pdgId());
    int wzFlag = ( kidPdgID == 23 || kidPdgID == 24 ) ? 1 : 0;
    for( int gmit(0); gmit < nMoms; gmit++ ){
        auto genmom = kid->mother(gmit);
        auto momPdgID = std::abs(genmom->pdgId());
        if( momPdgID == 1000022 && kidPdgID == 22 && depth == 0 ) return 1;
        else if( momPdgID == 1000022 ) { if( wzFlag ) return 13; else return 12; }
        else if( momPdgID == 1000023 ){ if( wzFlag ) return 23; else return 22; }
        else if( momPdgID == 1000025 ){ if( wzFlag ) return 33; else return 32; }
        else if( momPdgID == 1000035 ){ if( wzFlag ) return 43; else return 42; }
        else if( momPdgID == 1000024 && kidPdgID == 22 && depth == 0 ) return 111;
        else if( momPdgID == 1000024 ) { if( wzFlag ) return 113; else return 112; }
        else if( momPdgID == 1000037 ){ if( wzFlag ) return 123; else return 122; }
        else if( (momPdgID >= 1000001 && momPdgID <= 1000006) || momPdgID == 1000021 ) return 500;

        auto mGenPartID = llpGenChaseP(kid->mother(gmit),depth+1);
        if( mGenPartID < genPartID ) genPartID = mGenPartID;
    }//<<>>for( long unsigned int gmit(0); gmit < nKMother; gmit++ )
    return genPartID;

}//<<>> void llpChase( const reco::Candidate* kid, int wzFlag ){)

int KUCMSNtupilizer::llpGenChaseP( const reco::GenParticle kid, int depth ){

    int nMoms = kid.numberOfMothers();
    if( ! nMoms ) return 999;
    int genPartID = 1000;
    auto kidPdgID = std::abs(kid.pdgId());
    int wzFlag = ( kidPdgID == 23 || kidPdgID == 24 ) ? 1 : 0;
    for( int gmit(0); gmit < nMoms; gmit++ ){
        auto genmom = kid.mother(gmit);
        auto momPdgID = std::abs(genmom->pdgId());

        if( momPdgID == 1000022 && kidPdgID == 22 && depth == 0 ) return 1;
        else if( momPdgID == 1000022 ) { if( wzFlag ) return 13; else return 12; }
        else if( momPdgID == 1000023 ){ if( wzFlag ) return 23; else return 22; }
        else if( momPdgID == 1000025 ){ if( wzFlag ) return 33; else return 32; }
        else if( momPdgID == 1000035 ){ if( wzFlag ) return 43; else return 42; }
        else if( momPdgID == 1000024 && kidPdgID == 22 && depth == 0 ) return 111;
        else if( momPdgID == 1000024 ) { if( wzFlag ) return 113; else return 112; }
        else if( momPdgID == 1000037 ){ if( wzFlag ) return 123; else return 122; }
        else if( (momPdgID >= 1000001 && momPdgID <= 1000006) || momPdgID == 1000021 ) return 500;

        auto mGenPartID = llpGenChaseP(kid.mother(gmit),depth+1);
        if( mGenPartID < genPartID ) genPartID = mGenPartID;
    }//<<>>for( long unsigned int gmit(0); gmit < nKMother; gmit++ )
    return genPartID;

}//<<>> void llpChase( Candidate* kid )

void KUCMSNtupilizer::kidChase( std::vector<reco::CandidatePtr> kids, float vx, float vy, float vz ){

    for( auto kid : kids ){

        bool done(false);
        auto mom = kid->mother();
        std::cout << " -- Kid pdgID : " << kid->pdgId() << " p : " << kid->p() << " e : " << kid->energy() << std::endl;
        bool top(true);
        while( not done ){
            if( mom->pt() == 0 ) done = true;
            else {
                auto gmom = mom->mother();
                if( top ){
                    std::cout << " ---- KidStep pdgID : " << mom->pdgId() << " p : " << mom->p() << " e : " << mom->energy();
                    std::cout << " vert : (" << mom->vx() << "," << mom->vy() << "," << mom->vz() << ")" << std::endl;
                }//<<>>if( top )
                if( gmom->pt() == 0 ) done = true;
                else {
                    if( mom->vx() == gmom->vx() ) top = false;
                    else top = true;
                    mom = gmom;
                }//<<>>if( gmom->pt() == 0 )
            }//<<>>if( mom->pt() == 0 )                 
        }//<<>>while( not done )
    }//<<>>for( auto kid : kids )

}//<<>>string kidChase( std::vector<reco::CandidatePtr> kids, float vx )

vector<float> KUCMSNtupilizer::kidTOFChain( std::vector<reco::CandidatePtr> kids, float cx, float cy, float cz  ){
// redo this function to give tof and impact angle for input gen particle

    vector<float> result;
    vector<float> kidtime;
    vector<float> kide;
    vector<float> impact;
    bool llpcut(false);
    float nextBX(0.0);
    float llpkid(0.0);
    for( auto kid : kids ){

        bool done(false);
        bool first(true);
        bool llp(false);
        bool bquark(false);
        auto mom = kid->mother();
        vector<double> stepp{kid->p()};
        vector<double> stepe{kid->energy()};
        vector<double> orignvx;
        vector<double> orignvy;
        vector<double> orignvz;
        bool top(true);
        int steps(0);
        while( not done ){
            auto pdgid = llpGenChaseP( kid.get(), 0 );
            if( pdgid < 200 ) llp = true;
            if( pdgid == 500 ) bquark = true;
            if( mom->pt() == 0 ) done = true;
            else {
                auto gmom = mom->mother();
                if( top ){
                    if( first ) first = false;
                    else {
                        stepp.push_back(mom->p());
                        stepe.push_back(mom->energy());
                    }//<<>>if( first )
                    steps++;
                    //if( llp || bquark ) llpstep.push_back(1.0); else llpstep.push_back(0.0);
                    orignvx.push_back(mom->vx());
                    orignvy.push_back(mom->vy());
                    orignvz.push_back(mom->vz());
                }//<<>>if( top )
                if( gmom->pt() == 0 ) done = true;
                else {
                    if( mom->vx() == gmom->vx() ) top = false;
                    else top = true;
                    mom = gmom;
                }//<<>>if( gmom->pt() == 0 )
            }//<<>>if( mom->pt() == 0 )                 
        }//<<>>while( not done )
        first = true;

        if( DEBUG && false ) std::cout << " ---- jetGenTime Calc Steps : " << steps << std::endl;
        bool stepcut( steps == 0 );
        if( DEBUG && false ) std::cout << " ---- jetGenTime Calc llpcut : " << llpcut << std::endl;
        if( not stepcut ){
            float maxe(0.0);
            float totaltime(0.0);
            auto destvx(cx);
            auto destvy(cy);
            auto destvz(cz);
            for( int it(0); it < steps; it++ ){
                if( stepe[it] > maxe ) maxe = stepe[it];
                auto beta = stepp[it]/stepe[it];
                auto legtime = hypo( (destvx-orignvx[it]), (destvy-orignvy[it]), (destvz-orignvz[it]) )/(SOL*beta);
                totaltime += legtime;
                //std::cout << "Sum path legs beta: " << beta << " x: " << (destvx-orignvx[it]) << " y: " << (destvy-orignvy[it]);
                //std::cout << " z: " << (destvz-orignvz[it]) << " legtime: " << legtime << " totaltime: " << totaltime << std::endl;
                //std::cout << " -- dest : " << destvx << ", " << destvy << ", " << destvz; 
                //std::cout << " orig : " << orignvx[it] << ", " << orignvy[it] << ", " << orignvz[it] << std::endl;
                destvx = orignvx[it];
                destvy = orignvy[it];
                destvz = orignvz[it];
            }//<<>>for( int it(0); it < steps; it++ )
            auto r = hypo((cx-orignvx[0]), (cy-orignvy[0]));
            auto iangle = getAngle((cz-orignvz[0]),r);
            //std::cout << "getAngle: " << iangle << " r: " << r << " z: " << cz-orignvz[0] << " x: " << cx-orignvx[0];
            //std::cout << " y: " << cy-orignvy[0] << std::endl; 
            if( totaltime > 25.0 ) {
                nextBX = 1.0;
            } else {
                impact.push_back(iangle);
                kidtime.push_back(totaltime);
                kide.push_back(maxe);
                llpcut = llpcut || llp || bquark;
                if( llp ) llpkid++;
            }//<<>>if( totaltime > 25.0 )
        }//<<>>if( steps == 0 )

    }//<<>>for( auto kid : kids )

    auto mtime(-30.0);
    auto mangle(7.0);
    auto tvar(-2.0);
    auto ktsize = kidtime.size();
    //if( llpcut && ( ktsize > 0 ) ){
    if( ktsize > 0 ){
        if( ktsize > 1 ){
            mtime = mean(kidtime,kide);
            mangle = mean(impact,kide);
            tvar = var(kidtime,mtime,kide);
        } else {
            mtime = kidtime[0];
            mangle = impact[0];
            tvar = -1.0;
        }//<<>>if( kidtime.size() > 1 )
    }//<<>>if( llpcut )
    result.push_back(mtime);//0
    result.push_back(mangle);//1
    result.push_back(tvar);//2
    result.push_back(nextBX);//3
    if( llpcut ) result.push_back(1.0); else result.push_back(0.0);//4
    result.push_back(llpkid/ktsize);//5
    result.push_back(ktsize);//6
    return result;

}//>>>>vector<float> KUCMSNtupilizer::kidTOFChain( std::vector<reco::CandidatePtr> kids, float cx, float cy, float cz  )


vector<float>  KUCMSNtupilizer::getGenPartMatch( const reco::SuperCluster* scptr, std::vector<reco::GenParticle> fgenparts ){

    // GenParticle Info   -------------------------------------------------------------------
    if( DEBUG ) std::cout << "Getting phoGenParton Information" << std::endl;
    // set defaults for no match here
    vector<float> results;
    auto mgenDr = -1.0;// 0
    float goodDr(0.4);
    int matchedIdx(-1);// 1
    int index(0);
    float meta(9);// 2
    float mphi(9);// 3
    float geta(9);// 4
    float gphi(9);// 5
    for(const auto& genPart : fgenparts ){

        const auto rhX = scptr->x();
        const auto rhY = scptr->y();
        const auto rhZ = scptr->z();
        const auto gnX = genPart.vx();
        const auto gnY = genPart.vy();
        const auto gnZ = genPart.vz();
        auto cphoEta = std::asinh((rhZ-gnZ)/hypo(rhX-gnX,rhY-gnY));
        auto cphoPhi = std::atan2(rhY-gnY,rhX-gnX);
        auto dr = std::sqrt(reco::deltaR2(genPart.eta(), genPart.phi(), cphoEta, cphoPhi ));
        if( dr < goodDr ){
            goodDr = dr;
            matchedIdx = index;
            meta = cphoEta;
            mphi = cphoPhi;
            geta = genPart.eta();
            gphi = genPart.phi();
        }//<<>>if( jtgjdr <= goodDr )
        index++;

    }//<<>>for(const auto& genPart : fgenparts  ) 
    if( matchedIdx >= 0 ){ mgenDr = goodDr; }//<<>>if( matchedIdx >= 0 ) 

	results.push_back( mgenDr );
    results.push_back( matchedIdx );
    results.push_back( meta );
    results.push_back( mphi );
    results.push_back( geta );
    results.push_back( gphi );
	return results;

}//<<>>getGenPartMatch( reco::SuperClusterCollection *scptr, std::vector<reco::GenParticle> fgenparts )

float KUCMSNtupilizer::getSuperClusterSeedTime( reco::SuperClusterRef supercluster ){

    const auto & seedDetId = supercluster->seed()->seed(); // get seed detid
    const auto recHits = ((seedDetId.subdetId() == EcalSubdetector::EcalBarrel) ? recHitsEB_ : recHitsEE_); // which recHits to use
    const auto seedHit = recHits->find(seedDetId); // get the underlying rechit
    const auto seedTime = ((seedHit != recHits->end()) ? seedHit->time() : -9999.f);
    //if( DEBUG && seedTime == -9999.f ) std::cout << "Bad Photon seed time !!!! " << std::endl;
    return seedTime;

}//<<>>getSuperClusterSeedTime( reco::SuperClusterRef supercluster )

float KUCMSNtupilizer::getPhotonSeedTime( pat::Photon photon ){

	const auto & phosc = photon.superCluster().isNonnull() ? photon.superCluster() : photon.parentSuperCluster();
	return getSuperClusterSeedTime( phosc );

}//<<>>float KUCMSNtupilizer::getPhotonSeedTime( pat::Photon photon )

float KUCMSNtupilizer::getPhotonSeedTime( reco::Photon photon ){

    const auto & phosc = photon.superCluster().isNonnull() ? photon.superCluster() : photon.parentSuperCluster();
    return getSuperClusterSeedTime( phosc );

}//<<>>float KUCMSNtupilizer::getPhotonSeedTime( reco::Photon photon )

void KUCMSNtupilizer::setRecHitUsed( rhIdGroup idgroup){

	int nRecHits = frechits.size();
	for( auto rhid : idgroup ){
		for( int it = 0; it < nRecHits; it++ ){ if( getRawID( frechits[it] ) == rhid ){ frhused[it] = true; break; } }
	}//<<>>for( auto rhid : phoRhIdsGroup )

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------ define this as a plug-in --------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DEFINE_FWK_MODULE(KUCMSNtupilizer);

