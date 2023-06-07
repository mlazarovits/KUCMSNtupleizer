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

	// -- init tags  ----------------------------------------------------------
	// accuire input values form python config master file ( the one we use cmsRun with interactivilly )

    // tracks
    tracksTag(iConfig.getParameter<edm::InputTag>("tracks")),

	// conversions	
	conversionsTag(iConfig.getParameter<edm::InputTag>("conversions")),

	// beamSpot
	beamSpotTag(iConfig.getParameter<edm::InputTag>("beamSpot")),

	// triggers
	triggerResultsTag(iConfig.getParameter<edm::InputTag>("triggerResults")),
	//triggerObjectsTag(iConfig.getParameter<edm::InputTag>("triggerObjects")),

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

	// genInfo
	genEvtInfoTag(iConfig.getParameter<edm::InputTag>("genEvt")),
  	gent0Tag(iConfig.getParameter<edm::InputTag>("gent0")),
  	genxyz0Tag(iConfig.getParameter<edm::InputTag>("genxyz0")),
  	pileupInfosTag(iConfig.getParameter<edm::InputTag>("pileups")),

    // genParticles
  	genParticlesTag(iConfig.getParameter<edm::InputTag>("genParticles")),

    // genJets
    genJetsTag(iConfig.getParameter<edm::InputTag>("genjets")),

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

    // -- set input flags and parmeters  ----------------------------------------------------------
    // accuire input values form python config master file ( the one we use cmsRun with interactivilly )

    cfFlag.set( "hasGenInfo", iConfig.existsAs<bool>("hasGenInfo") ? iConfig.getParameter<bool>("hasGenInfo") : true );
    cfFlag.set( "onlyEB", iConfig.existsAs<bool>("onlyEB") ? iConfig.getParameter<bool>("onlyEB") : true );

    cfPrm.set( "ebMaxEta",iConfig.existsAs<double>("ebMaxEta")? iConfig.getParameter<double>("ebMaxEta") : 1.479 );

	cfPrm.set( "jetPTmin", iConfig.existsAs<double>("jetPTmin") ? iConfig.getParameter<double>("jetPTmin") : 15.0 );
    cfPrm.set( "jetEtaMax", iConfig.existsAs<double>("jetEtaMax") ? iConfig.getParameter<double>("jetEtaMax") : 3.0 );

    cfPrm.set( "minRHEi", iConfig.existsAs<double>("minRHEi") ? iConfig.getParameter<double>("minRHEi") : 0.0 );
    cfPrm.set( "minRHEf", iConfig.existsAs<double>("minRHEf") ? iConfig.getParameter<double>("minRHEf") : 2.0 );

    cfPrm.set( "minMuonE", iConfig.existsAs<double>("minMuonE") ? iConfig.getParameter<double>("minMuonE") : 1.0 );

    cfPrm.set( "minMetE", iConfig.existsAs<double>("minMetE") ? iConfig.getParameter<double>("minMetE") : 1.0 );

    cfPrm.set( "minCaloCltrE", iConfig.existsAs<double>("minCaloCltrE") ? iConfig.getParameter<double>("minCaloCltrE") : 1.0 );

    cfPrm.set( "minCaloJetE", iConfig.existsAs<double>("minCaloJetE") ? iConfig.getParameter<double>("minCaloJetE") : 1.0 );

    cfPrm.set( "minPhoE", iConfig.existsAs<double>("minPhoE") ? iConfig.getParameter<double>("minPhoE") : 0.0 );
    cfPrm.set( "phoMinPt", iConfig.existsAs<double>("phoMinPt") ? iConfig.getParameter<double>("phoMinPt") : 0.0 );
    cfPrm.set( "phoMinSeedTime", iConfig.existsAs<double>("phoMinSeedTime") ? iConfig.getParameter<double>("phoMinSeedTime") : -25.0 );

    cfPrm.set( "minEleE", iConfig.existsAs<double>("minEleE") ? iConfig.getParameter<double>("minEleE") : 0.0 );
    cfPrm.set( "minGenE", iConfig.existsAs<double>("minGenE") ? iConfig.getParameter<double>("minGenE") : 0.0 );

    cfPrm.set( "minEvtMet", iConfig.existsAs<double>("minEvtMet") ? iConfig.getParameter<double>("minEvtMet") : 150.0 );

	// -- consume tags ------------------------------------------------------------
	// creats "token" for all collections that we wish to process
	
	if( DEBUG ) std::cout << "In constructor for KUCMSNtupilizer - tag and tokens" << std::endl;

	// conversions
	conversionsToken_			= consumes<reco::ConversionCollection>(conversionsTag);

	// beamSpot
	beamLineToken_				= consumes<reco::BeamSpot>(beamSpotTag);

	// Triggers
	triggerResultsToken_ 		= consumes<edm::TriggerResults>(triggerResultsTag);
	//triggerObjectsToken_ 		= consumes<std::vector<pat::TriggerObjectStandAlone>>(triggerObjectsTag);

	// tracks 
	tracksToken_				= consumes<std::vector<reco::Track>>(tracksTag);

	// genparticles
	// genpart_token_           = consumes<std::vector<pat::PackedGenParticle>>(genpartTag);

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
	if( cfFlag("hasGenInfo") ){

		genEvtInfoToken_   		= consumes<GenEventInfoProduct>(genEvtInfoTag);
    	gent0Token_        		= consumes<float>(gent0Tag);
    	genxyz0Token_      		= consumes<Point3D>(genxyz0Tag);
    	pileupInfosToken_  		= consumes<std::vector<PileupSummaryInfo>>(pileupInfosTag);
    	genParticlesToken_ 		= consumes<std::vector<reco::GenParticle>>(genParticlesTag);
        genJetsToken_           = consumes<std::vector<reco::GenJet>>(genJetsTag);

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
	iEvent.getByToken(triggerResultsToken_,triggerResults_);
	//iEvent.getByToken(triggerObjectsToken_,triggerObjects_);

	// Conversions
	iEvent.getByToken(conversionsToken_,conversions_);

	// BeamSpot
	iEvent.getByToken(beamLineToken_,beamSpot_);

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

    // GENInfo & GENPARTICLES & GENJETS
    if( cfFlag("hasGenInfo") ){

        iEvent.getByToken(genEvtInfoToken_, genEvtInfo_);
        iEvent.getByToken(gent0Token_, gent0_);
        iEvent.getByToken(genxyz0Token_, genxyz0_);
        iEvent.getByToken(pileupInfosToken_, pileupInfos_);
        iEvent.getByToken(genParticlesToken_, genParticles_);
        iEvent.getByToken(genJetsToken_, genJets_);

    }//<<>>if( hasGenInfo )

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

    // clear global event varibles 
	geVar.clear(); // floats

    // process event varibles

    processEvent( iEvent );
    processVtx();

	// -------------------------------------------------------------------------------------------------
	// ---  Collecting objects ( preprocessing object pruning & fill global object vectors )------------
	// -------------------------------------------------------------------------------------------------

	
	// fill filtered collection vectors

    if( DEBUG ) std::cout << "Collecting ECAL RecHits" << std::endl;
    frechits.clear();
    frhused.clear();
    for( const auto &recHit : *recHitsEB_ ){ if(recHit.energy() > cfPrm("minRHEi")) frechits.push_back(recHit); frhused.push_back(false);}
    for( const auto &recHit : *recHitsEE_ ){ if(recHit.energy() > cfPrm("minRHEi")) frechits.push_back(recHit); frhused.push_back(false);}


    if( DEBUG ) std::cout << "Collecting Muons" << std::endl;
	fmuons.clear();
    //if( DEBUG ) std::cout << "Collecting Muons" << std::endl; //  set EB only filter
    //for( const auto &muon : *muons_ ){ if( muon.energy() > minMuonE ) fmuons.push_back(muon); }

    if( DEBUG ) std::cout << "Collecting Met" << std::endl;
    fpfmet.clear();
    //for( const auto met : *pfmets_ ){ if( met.energy() > minMetE ) 
	fpfmet.push_back((*pfmets_)[0]);

	if( DEBUG ) std::cout << "Filter Jets" << std::endl;
    fjets.clear();
    jetIds.clear();	
	for(const auto &jet : *jets_ ){ // Filters jet collection & sorts by pt

		if( jet.pt() < cfPrm("jetPTmin") ) continue;
	  	if( cfFlag("onlyEB") && std::abs(jet.eta()) > cfPrm("ebMaxEta") ) continue;
		if( std::abs(jet.eta()) > cfPrm("jetEtaMax") ) continue; 
	  	// save the jets, and then store the ID
	  	fjets.emplace_back(jet);
		auto jetID = 0;		
		jetIds.push_back(jetID);	  

	  	std::sort(fjets.begin(),fjets.end(),sortByPt);

	}//<<>>for(const auto& jet : *jets_ )

    if( DEBUG ) std::cout << "Filter CaloJets" << std::endl;
    fcalojets.clear();
	//for(const auto &calojet : *caloJets_ ){ if( calojet.energy() > minCaloJetE ) fcalojets.push_back(calojet);  }

	if( DEBUG ) std::cout << "Collecting Calo Clusters" << std::endl; // set eb only filter
    fbclusts.clear();
	//for( const auto &bclust : *caloCluster_ ){ if( bclust.energy() > minCaloCltrE ) fbclusts.push_back(bclust); }
	
    if( DEBUG ) std::cout << "Collecting Photons/OOTPhotons" << std::endl;
    fphotons.clear();
    phoExcluded.clear();
    phoIsOotPho.clear();
	phoIds.clear();// indexed by pho index ( 0,1,2 ) * number of ids ( 1 current, 6? possible ) + index of ID wanted
    for (edm::View<reco::Photon>::const_iterator itPhoton = ootPhotons_->begin(); itPhoton != ootPhotons_->end(); itPhoton++) {
        //auto idx = itPhoton - ootPhotons_->begin();//unsigned int
        //auto ootPhoRef = ootPhotons_->refAt(idx);//edm::RefToBase<reco::GsfElectron> 
        auto &ootPho = (*itPhoton);
		if( cfFlag("onlyEB") && ootPho.isEE() ) continue;
        auto minPhoPt = ootPho.pt() < cfPrm("phoMinPt");
        auto phoSeedTime = getPhotonSeedTime(ootPho);
        auto minTime = phoSeedTime < cfPrm("phoMinSeedTime");
        auto minEnergy = ootPho.energy() < cfPrm("minPhoE");
		if( minPhoPt || minTime || minEnergy ) continue;
        double minDr(0.5);
        double dRmatch(10.0);
        float matchpt(0);
        auto oEta = ootPho.eta();
        auto oPhi = ootPho.phi();
		auto oPt = ootPho.pt();
        for( const auto &gedPho : *gedPhotons_ ){
			if( cfFlag("onlyEB") && gedPho.isEE() ) continue;
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
        //auto idx = itPhoton - gedPhotons_->begin();//unsigned int
        //auto gedPhoRef = gedPhotons_->refAt(idx);//edm::RefToBase<reco::GsfElectron> 
        auto &gedPho = (*itPhoton);
		if( cfFlag("onlyEB") && gedPho.isEE() ) continue;
        auto minPt = gedPho.pt() < cfPrm("phoMinPt");
        auto phoSeedTime = getPhotonSeedTime(gedPho);
        auto minTime = phoSeedTime < cfPrm("phoMinSeedTime");
        auto minEnergy = gedPho.energy() < cfPrm("minPhoE");
        if( minPt || minTime || minEnergy ) continue;
        double minDr(0.5);
        double dRmatch(10.0);
        float matchpt(0);
		auto pEta = gedPho.eta();
		auto pPhi = gedPho.phi();
		auto pPt = gedPho.pt();
        for( const auto &ootPho : *ootPhotons_ ){
			if( cfFlag("onlyEB") && ootPho.isEE() ) continue;
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
    felectrons.clear();
	eleIds.clear();// indexed by ele index ( 0,1,2 ) * number of ids ( 1 current, 6? possible ) + index of ID wanted
	for (edm::View<reco::GsfElectron>::const_iterator itElectron = electrons_->begin(); itElectron != electrons_->end(); itElectron++) {
		//auto idx = itElectron - electrons_->begin();//unsigned int
		//auto electronRef = electrons_->refAt(idx);//edm::RefToBase<reco::GsfElectron> 
		auto &electron = (*itElectron);
		if ( cfFlag("onlyEB") && std::abs(electron.eta()) > cfPrm("ebMaxEta") ) continue;
		//auto passIdCut = true; //electron.electronID(eleCutLoose);// pat electron ( miniAOD ) method
		//eleIdBools.push_back((*eleMVAIDLooseMap_)[electronRef]);// order is important, track how this vector is loaded
		auto passEnergyCut = electron.energy() > cfPrm("minEleE");
		if( passEnergyCut ) felectrons.push_back(electron);
	}//<<>>for( const auto electron : *electrons_ )

    fgenparts.clear();
    fgenpartllp.clear();
    fgenjets.clear();
    int nGenPart(0);
	if( cfFlag("hasGenInfo") ){ 

		if( DEBUG ) std::cout << "Collecting Gen Particles" << std::endl;
		for( const auto &genPart : *genParticles_ ){ 
	
			if( genPart.energy() < cfPrm("minGenE") ) continue;
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


    //------------------------------------------------------------------------------------
	// ----   Object processing ----------------------------------------------------------
    //------------------------------------------------------------------------------------
	// call functions to process collections and fill tree varibles to be saved
	// varibles to be saved to ttree are declared in the header

	// process gen collections
	if( cfFlag("hasGenInfo") ){ processGenPart(); }//<<>>if( hasGenInfo )
	// process main collections
	processMet();
    //processClJet();
	processPhotons();
    //processElectrons();
	processJets();

	// Process RecHits : Must be processed last
    processRecHits();

    //------------------------------------------------------------------------------------
    //---- Object processing Completed ----------------------------------------------------------
    //------------------------------------------------------------------------------------

	// -- Fill output trees ------------------------------------------
	if( selectedEvent() ) outTree->Fill();

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
	// via the KUCMS BranchManager

	// Event varibles branches
	setBranchesEvent();
	setBranchesVtx();

	// object collection branches
	setBranchesMet();
	setBranchesJets();
    //setBranchesClusterJets();
	setBranchesPhotons();
	//setBranchesElectrons();

	// Gen collection branches
	setBranchesGenParts();

	// reduced ecal rechit collection branches
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

//  --------------  collection processing and branch declaration functions ----------------------
//////////////////////////////////////////////////////////////////////////////////////////////////

bool KUCMSNtupilizer::selectedEvent(){

	bool hasMinMet = geVar("evtMET") > cfPrm("minEvtMet");

	bool selected = hasMinMet;
	return selected; 

}//<<>>bool KUCMSNtupilizer::selectedEvent()

//---------------------------------------------
void KUCMSNtupilizer::setBranchesEvent(){

	EventInfoBM.makeBranch("run","RunNumber",UINT,"Run number of event");
    EventInfoBM.makeBranch("lumi","LumiSection",UINT,"Lumi section of event");
    EventInfoBM.makeBranch("event","EventNumber",UINT);
    EventInfoBM.makeBranch("wgt","EventWeight",FLOAT);

    EventInfoBM.attachBranches(outTree);

}//<<>>setBranchesEvent()

void KUCMSNtupilizer::processEvent( const edm::Event& iEvent ){

	EventInfoBM.clearBranches();

    uInt run   = iEvent.id().run();
    uInt lumi  = iEvent.luminosityBlock();
    uInt event = iEvent.id().event();
	float wgt = ( cfFlag("hasGenInfo") ) ? genEvtInfo_->weight() : 1.f;

    if( DEBUG ) std::cout << "Processing event: " << event << " in run: " << run << " and lumiblock: " << lumi << std::endl;

	EventInfoBM.fillBranch("run",run);
	EventInfoBM.fillBranch("lumi",lumi);
    EventInfoBM.fillBranch("event",event);
    EventInfoBM.fillBranch("wgt",wgt);

}//<<>>processEvent()

//----------------------------------------------
void KUCMSNtupilizer::setBranchesVtx(){

    VertexBM.makeBranch("nVtx","nVtx",UINT);
	VertexBM.makeBranch("vtxX","vtxX",FLOAT);
    VertexBM.makeBranch("vtxY","vtxY",FLOAT);
    VertexBM.makeBranch("vtxZ","vtxZ",FLOAT);

    VertexBM.attachBranches(outTree);

}//<<>>setBranchesVtx()

void KUCMSNtupilizer::processVtx(){

	VertexBM.clearBranches();
	
	// do not need to pass in collectioon handel -> class varible   --  Fix This
	//edm::Handle<std::vector<reco::Vertex>> vertices_
	uInt nvtx = vertices_->size();
    const auto & primevtx = vertices_->front();
    const float vtxX = primevtx.position().x();
    const float vtxY = primevtx.position().y();
    const float vtxZ = primevtx.position().z();

	// set global event varibles for vertix
	geVar.set("vtxX",vtxX);
    geVar.set("vtxY",vtxY);
    geVar.set("vtxZ",vtxZ);

	VertexBM.fillBranch("nVtx",nvtx);
    VertexBM.fillBranch("vtxX",vtxX);
    VertexBM.fillBranch("vtxY",vtxY);
    VertexBM.fillBranch("vtxZ",vtxZ);

}//<<>>processVtx()

//-------------------------------------------------
void KUCMSNtupilizer::setBranchesMet(){

	MetBM.makeBranch("SumEt","metSumEt",FLOAT,"scalar sum of Et");
    MetBM.makeBranch("Px","metPx",FLOAT);
    MetBM.makeBranch("Py","metPy",FLOAT);

    MetBM.makeBranch("signif","metSignificance",FLOAT,"MET significance");
    MetBM.makeBranch("covXX","metCovXX",FLOAT,"xx element of met covariance matrix");
    MetBM.makeBranch("covXY","metCovXY",FLOAT,"xy element of met covariance matrix");
    MetBM.makeBranch("covYY","metCovYY",FLOAT,"yy element of met covariance matrix");

    MetBM.makeBranch("CSumEt","metCSumEt",FLOAT,"SumEt corrected for ootPhotons");
    MetBM.makeBranch("CPx","metCPx",FLOAT,"Met Px corrected for oootPhotons");
    MetBM.makeBranch("CPy","metCPy",FLOAT,"Met Py corrected for oootPhotons");

	MetBM.attachBranches(outTree);

}//<<>>void setBranchesMet()

void KUCMSNtupilizer::processMet(){

    if( DEBUG ) std::cout << "Processing MET" << std::endl;

	MetBM.clearBranches();

    auto t1pfMET = fpfmet[0];
    const float SumEt = t1pfMET.sumEt();
    //const float Pt = t1pfMET.pt();
    const float Px = t1pfMET.px();
    const float Py = t1pfMET.py();
    //const float Phi = t1pfMET.phi();

    const float signif = t1pfMET.significance();
    const float covXX = t1pfMET.getSignificanceMatrix().At(0,0);
    const float covXY = t1pfMET.getSignificanceMatrix().At(0,1);
    const float covYY = t1pfMET.getSignificanceMatrix().At(1,1);

    float CSumEt = SumEt;
    float CPx = Px;
    float CPy = Py;

    // ???  need corrected values for photon Et and Pt
    int nphidx = fphotons.size();
    for( auto phidx = 0; phidx < nphidx; phidx++ ){

        auto photon = fphotons[phidx];
        if( phoIsOotPho[phidx] && not phoExcluded[phidx] ){
            CPx -= ((photon.pt())*std::cos(photon.phi()));
            CPy -= ((photon.pt())*std::sin(photon.phi()));
            CSumEt -= photon.et();
        }//<<>>if( phoIsOotPho[phidx] && not phoExcluded[phidx] )
        if( not phoIsOotPho[phidx] && phoExcluded[phidx] ){
            CPx += ((photon.pt())*std::cos(photon.phi()));
            CPy += ((photon.pt())*std::sin(photon.phi()));
            CSumEt += photon.et();
        }//<<>>if( phoIsOotPho[phidx] && not phoExcluded[phidx] )   

    }//<<>>for( auto phidx = 0; phidx < fphotons.size(); phidx++ )

    //evtMET = std::sqrt( sq2(Px) + sq2(Py));
    geVar.set( "evtMET", SumEt );

	MetBM.fillBranch("SumEt",SumEt);
    MetBM.fillBranch("Px",Px);
    MetBM.fillBranch("Py",Py);

    MetBM.fillBranch("signif",signif);
    MetBM.fillBranch("covXX",covXX);
    MetBM.fillBranch("covXY",covXY);
    MetBM.fillBranch("covYY",covYY);

    MetBM.fillBranch("CSumEt",CSumEt);
    MetBM.fillBranch("CPx",CPx);
    MetBM.fillBranch("CPy",CPy);

}//<<>>processMet( std::vector<reco::PFMET> fpfmet, std::vector<reco::Photon> fphotons )

//---------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesJets(){

    //JetAK4ChsBM.makeBranch("Ht","jetHt",FLOAT);

    JetAK4ChsBM.makeBranch("E","jetE",VFLOAT);
    JetAK4ChsBM.makeBranch("M","jetM",VFLOAT);
    JetAK4ChsBM.makeBranch("Pt","jetPt",VFLOAT);
    JetAK4ChsBM.makeBranch("Eta","jetEta",VFLOAT);
    JetAK4ChsBM.makeBranch("Phi","jetPhi",VFLOAT);
    JetAK4ChsBM.makeBranch("Area","jetArea",VFLOAT,"jet catchment area, for JECs");

    JetAK4ChsBM.makeBranch("nKids","jetNumberOfDaughters",VINT,"Number of particles in the jet");
    JetAK4ChsBM.makeBranch("NHF","jetNHF",VFLOAT,"neutral Hadron Energy Fraction");
    JetAK4ChsBM.makeBranch("NEMF","jetNEMF",VFLOAT,"neutral Electromagnetic Energy Fraction");
    JetAK4ChsBM.makeBranch("CHF","jetCHF",VFLOAT,"charged Hadron Energy Fraction");
    JetAK4ChsBM.makeBranch("CEMF","jetCEMF",VFLOAT,"charged Electromagnetic Energy Fraction");
    JetAK4ChsBM.makeBranch("MUF","jetMUF",VFLOAT,"muon Energy Fraction");
    JetAK4ChsBM.makeBranch("NHM","jetNHM",VFLOAT);
    JetAK4ChsBM.makeBranch("CHM","jetCHM",VFLOAT);

    JetAK4ChsBM.makeBranch("Parts","jetPartons",VVUINT);
    JetAK4ChsBM.makeBranch("DrRhIds","jetDrRhIds",VVUINT);

    JetAK4ChsBM.makeBranch("GenImpactAngle","jetGenImpactAngle",VFLOAT);
    JetAK4ChsBM.makeBranch("GenTime","jetGenTime",VFLOAT);
    JetAK4ChsBM.makeBranch("GenPt","jetGenPt",VFLOAT);
    JetAK4ChsBM.makeBranch("GenEta","jetGenEta",VFLOAT);
    JetAK4ChsBM.makeBranch("GenPhi","jetGenPhi",VFLOAT);
    JetAK4ChsBM.makeBranch("GenEnergy","jetGenEnergy",VFLOAT);
    JetAK4ChsBM.makeBranch("GenDrMatch","jetGenDrMatch",VFLOAT);
    JetAK4ChsBM.makeBranch("GenDptMatch","jetGenDptMatch",VFLOAT);
    JetAK4ChsBM.makeBranch("GenTimeLLP","jetGenTimeLLP",VFLOAT);
    JetAK4ChsBM.makeBranch("GenTOF","jetGenTOF",VFLOAT);

    JetAK4ChsBM.attachBranches(outTree);

}//<<>> setBranchesJets(){

void KUCMSNtupilizer::processJets(){ 

    if( DEBUG ) std::cout << "Init for Jet Loop " << std::endl;

    vector<uInt> noIdGrp(1,0);
	JetAK4ChsBM.clearBranches();

	// **** const reco::TrackRefVector& associatedTracks() const;
	// **** const reco::PixelClusterTagInfo* tagInfoPixelCluster(const std::string& label = "") const; // from b tagging info methods
	// **** reco::PFCandidateFwdPtrVector const& pfCandidatesFwdPtr() const { return pfCandidatesFwdPtr_; }
    if( DEBUG ) std::cout << "-----------------------------------------------------------" << std::endl;
	auto nJets = fjets.size();
	float jetHt = 0;
    if( DEBUG ) std::cout << "Starting Jet Loop for " << nJets << " jets " << std::endl;
    for ( uInt ijet(0); ijet < nJets; ijet++ ){

        const auto & jet = fjets[ijet];
        //const auto jetid = fjetsID[ijet];
        if( DEBUG ) std::cout << " --- Proccesssing : " << jet  << std::endl;

        jetHt += jet.pt();

    	float jetE = jet.energy();
    	float jetM = jet.mass();
    	float jetPt = jet.pt();
    	float jetEta = jet.eta();
    	float jetPhi = jet.phi();
        float jetArea = jet.jetArea();

    	float jetNHF = jet.neutralHadronEnergyFraction();
    	float jetNEMF = jet.neutralEmEnergyFraction();
    	float jetCHF = jet.chargedHadronEnergyFraction();
    	float jetCEMF = jet.chargedEmEnergyFraction();
   	 	float jetMUF = jet.muonEnergyFraction();
   	 	float jetNHM = jet.neutralMultiplicity();
	    float jetCHM = jet.chargedMultiplicity();

        JetAK4ChsBM.fillBranch("E",jetE);
        JetAK4ChsBM.fillBranch("M",jetM);
        JetAK4ChsBM.fillBranch("Pt",jetPt);
        JetAK4ChsBM.fillBranch("Eta",jetEta);
        JetAK4ChsBM.fillBranch("Phi",jetPhi);
        JetAK4ChsBM.fillBranch("Area",jetArea);

        JetAK4ChsBM.fillBranch("NHF",jetNHF);
        JetAK4ChsBM.fillBranch("NEMF",jetNEMF);
        JetAK4ChsBM.fillBranch("CHF",jetCHF);
        JetAK4ChsBM.fillBranch("CEMF",jetCEMF);
        JetAK4ChsBM.fillBranch("MUF",jetMUF);
        JetAK4ChsBM.fillBranch("NHM",jetNHM);
        JetAK4ChsBM.fillBranch("CHM",jetCHM);

        // dR matched RH group  ----------------------------------------------- 
        // --------------------------------------------------------------------

        if( DEBUG ) std::cout << "Getting jet dR rechit group" << std::endl;
		auto deltaRmin = 0.8;
		auto minRhE = 0.5;
        auto jetDrRhGroup = getRHGroup( jetEta, jetPhi, deltaRmin, minRhE );
        auto jetDrRhIdsGroup = getRhGrpIDs( jetDrRhGroup );
    	JetAK4ChsBM.fillBranch("DrRhIds",jetDrRhIdsGroup);
		setRecHitUsed( jetDrRhIdsGroup );

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// !!!!!!!!  create list of associated photons and electrons with jet via daughter particles of jet !!!!!!!!!!
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		int nKids = jet.nConstituents();
		JetAK4ChsBM.fillBranch("nKids",nKids);

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

		JetAK4ChsBM.fillBranch("Parts",jetParts);

        // GenJet Info for MC  -------------------------------------------------------------------
        // ---------------------------------------------------------------------------------------

        if( DEBUG ) std::cout << "Getting jetGenParton Information" << std::endl;

        //const reco::GenParticle * jetGenParton(0);
        float genImpactAngle(-10.0);
        float genTime(-99.9);
        float genPt(-10.0);
        float genEta(-10.0);
        float genPhi(-10.0);
        float genEnergy(-10.0);
        float genEMFrac(-1.0);
        float genDrMatch(-1.0);
        float genDptMatch(-1.0);
        //float genTimeVar(-1.0);
        float genTimeLLP(-99.9);
        //float genLLPPurity(-1.0);
        //float genNextBX(-1.0);
        //float genNKids(-1.0);
        float genTOF(-99.9);

        if( cfFlag("hasGenInfo") ){

            float goodDr(5.0);
			float goodDpt(5.0);
            int matchedIdx(-1);
            int index(0);
            for(const auto &genJet : fgenjets ){

                auto gjeta = genJet.eta();
                auto gjphi = genJet.phi();
				auto gjpt = genJet.pt();
                auto jtgjdr = std::sqrt(reco::deltaR2(gjeta, gjphi, jetEta, jetPhi ));
				auto jtgjdp = std::abs(jetPt-gjpt)/gjpt;
                if( jtgjdr < goodDr && jtgjdp < goodDpt ){
                    goodDr = jtgjdr;
					goodDpt = jtgjdp;
                    matchedIdx = index;
                }//<<>>if( jtgjdr <= goodDr )
                index++;

            }//<<>>for(const auto& genJet : fgenjets ) 

            if( matchedIdx >= 0 ){

                auto genJet = fgenjets[matchedIdx];
                if( DEBUG ) std::cout << " --- Jet-GenJet dR match : " << goodDr << std::endl;

                auto nSources = genJet.numberOfSourceCandidatePtrs();

                if( DEBUG ) std::cout << " ---------------------------------------------------- " << std::endl;
                if( DEBUG ) std::cout << " - genJet srcs : " << nSources << " PV(" << geVar("vtxX") << "," << geVar("vtxY"); 
                if( DEBUG ) std::cout << "," << geVar("vtxZ") << ")" << std::endl;

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
				genDptMatch = goodDpt;
                //genTimeVar = genKidInfo[2];
                //genNextBX = genKidInfo[3];
                genTimeLLP = genKidInfo[4];
                //genLLPPurity = genKidInfo[5];
                //genNKids = genKidInfo[6];
                genTOF = tofcor;
                if( DEBUG ) std::cout << " -- Energy : " << genEnergy << " Pt : " << genPt << " EMfrac : " << genEMFrac << std::endl;

            }//<<>>if( matchedIdx >= 0 ) 

            // load event level vectors for this jet with gen info

    		JetAK4ChsBM.fillBranch("GenImpactAngle",genImpactAngle);
    		JetAK4ChsBM.fillBranch("GenTime",genTime);
    		JetAK4ChsBM.fillBranch("GenPt",genPt);
    		JetAK4ChsBM.fillBranch("GenEta",genEta);
    		JetAK4ChsBM.fillBranch("GenPhi",genPhi);
    		JetAK4ChsBM.fillBranch("GenEnergy",genEnergy);
    		JetAK4ChsBM.fillBranch("GenDrMatch",genDrMatch);
            JetAK4ChsBM.fillBranch("GenDptMatch",genDptMatch);
    		JetAK4ChsBM.fillBranch("GenTimeLLP",genTimeLLP);
    		JetAK4ChsBM.fillBranch("GenTOF",genTOF);

            if( DEBUG ) std::cout << " ---------------------------------------------------- " << std::endl;

        }//<<>>if( hasGenInfo )
        if( DEBUG ) std::cout << "Next Jet .......................... " << std::endl;

    }//<<>>for ( uInt ijet = 0; ijet < nJets; ijet++ )
    // ** end of jets   ***************************************************************************************************

}//<<>>processJets( fjets, fjetsID, fgenparts )

//-----------------------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesClusterJets(){

	CaloJetBM.makeBranch("cljRhIds","cljRhIds",VVUINT);
    CaloJetBM.makeBranch("cljPt","cljPt",VFLOAT);
    CaloJetBM.makeBranch("cljEnergy","cljEnergy",VFLOAT);
    CaloJetBM.makeBranch("cljPhi","cljPhi",VFLOAT);
    CaloJetBM.makeBranch("cljEta","cljEta",VFLOAT);
    CaloJetBM.makeBranch("cljPx","cljPx",VFLOAT);
    CaloJetBM.makeBranch("cljPy","cljPy",VFLOAT);
    CaloJetBM.makeBranch("cljPz","cljPz",VFLOAT);

	CaloJetBM.attachBranches(outTree);

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

    PhotonBM.makeBranch("IsOotPho","phoIsOot",VBOOL);
    PhotonBM.makeBranch("Excluded", "phoExcluded",VBOOL);

    PhotonBM.makeBranch("Pt","phoPt",VFLOAT);
    PhotonBM.makeBranch("Energy","phoEnergy",VFLOAT);
    PhotonBM.makeBranch("Phi","phoPhi",VFLOAT);
    PhotonBM.makeBranch("Eta","phoEta",VFLOAT);
    PhotonBM.makeBranch("Px","phoPx",VFLOAT);
    PhotonBM.makeBranch("Py","phoPy",VFLOAT);
    PhotonBM.makeBranch("Pz","phoPz",VFLOAT);

    PhotonBM.makeBranch("EnergyErr","phoEnergyErr",VFLOAT,"energy error of the cluster from regression");//
    PhotonBM.makeBranch("EnergyRaw","phoEnergyRaw",VFLOAT,"raw energy of photon supercluster");//

    //PhotonBM.makeBranch("JetIdx",VUINT);// index of matching jet/ele -> can do at skimmer
    //PhotonBM.makeBranch("eleIdx",VUINT);// index of matching jet/ele -> can do at skimmer

    PhotonBM.makeBranch("SeedTOFTime","phoSeedTOFTime",VFLOAT,"time of flight from PV to photon seed crystal");
    PhotonBM.makeBranch("RhIds","phoRhIds",VVUINT,"list of rechit raw ids in hits and fractions list from supercluster");
    PhotonBM.makeBranch("hasPixelSeed","phoHasPixelSeed",VBOOL,"has pixel seed");
    PhotonBM.makeBranch("eleVeto","phoPassEleVeto",VBOOL,"pass electron veto");//
    PhotonBM.makeBranch("isEB","phoSeedIsEB",VBOOL,"photon supercluster seed crystal is in ecal barrel");

    PhotonBM.makeBranch("R9","phoR9",VFLOAT,"R9 of the supercluster, calculated with full 5x5 region");
    PhotonBM.makeBranch("SigmaIEtaIEta","phoSigmaIEtaIEta",VFLOAT,"sigma_IetaIeta of supercluster, calc w/ full 5x5 region");
    PhotonBM.makeBranch("SigmaIEtaIPhi","pohoSigmaIEtaIPhi",VFLOAT,"sigmaIphiIphi of supercluster");//
    PhotonBM.makeBranch("SigmaIPhiIPhi","phoSigmaIPhiIPhi",VFLOAT,"sigma_IetaIphi of supercluster, calc w/ full 5x5 region");//
    PhotonBM.makeBranch("S4",VFLOAT,"e2x2/e5x5 of supercluster, calc w/ full 5x5 region");//

    PhotonBM.makeBranch("HadTowOverEM","phoHadTowOverEM",VFLOAT);
    PhotonBM.makeBranch("HadOverEM","phoHadOverEM",VFLOAT);//
    PhotonBM.makeBranch("EcalRHSumEtConeDR04","phoEcalRHSumEtConeDR04",VFLOAT);
    PhotonBM.makeBranch("HcalTowerSumEtBcConeDR04","phoHcalTowerSumEtBcConeDR04",VFLOAT);
    PhotonBM.makeBranch("TrkSumPtSolidConeDR04","phoTrkSumPtSolidConeDR04",VFLOAT);
    PhotonBM.makeBranch("TrkSumPtHollowConeDR04","phoTrkSumPtHollowConeDR04",VFLOAT);
    PhotonBM.makeBranch("TrkSumPtHollowConeDR03","phoTrkSumPtHollowConeDR03",VFLOAT,"Sum of track pT in a hollow cone of outer radius, inner radius");// nano -> DR03?

    PhotonBM.makeBranch("pfPhoIso03","phoPfPhoIso03",VFLOAT,"PF abs iso dR=0.3, photon component (uncorrected)");//
    PhotonBM.makeBranch("pfChargedIsoPFPV","phoPfChargedIsoPFPV",VFLOAT,"PF abs iso dR=0.3, charged component (PF PV only)");//
    PhotonBM.makeBranch("pfChargedIsoWorstVtx","phoPfChargedIsoWorstVtx",VFLOAT,"PF abs iso dR=0.3, charged component (Vertex w/ largest iso)");//
    //PhotonBM.makeBranch("pfRelIso03_chg_quadratic",VFLOAT);//
    //PhotonBM.makeBranch("pfRelIso03_all_quadratic",VFLOAT);//
    //PhotonBM.makeBranch("hoe_PUcorr","phoHoe_PUcorr",VFLOAT,
	//						"PU corrected H/E (cone-based with quadraticEA*rho*rho + linearEA*rho Winter22V1 corrections)");// userFloat
    PhotonBM.makeBranch("isScEtaEB","phoIsScEtaEB",VBOOL,"is supercluster eta within barrel acceptance");//
    PhotonBM.makeBranch("isScEtaEE","phoIsScEtaEE",VBOOL,"is supercluster eta within endcap acceptance");//

    //PhotonBM.makeBranch("seedGain",VINT);// ? can find through rh collection if needed
    PhotonBM.makeBranch("seediEtaOriX","phoSeedIex",VINT,"iEta or iX of seed crystal. iEta is barrel-only, iX is endcap-only. iEta runs from -85 to +85, with no crystal at iEta=0. iX runs from 1 to 100.");//
    PhotonBM.makeBranch("seediPhiOriY","phoSeedIpy",VINT,"iPhi or iY of seed crystal. iPhi is barrel-only, iY is endcap-only. iPhi runs from 1 to 360. iY runs from 1 to 100.");//
    PhotonBM.makeBranch("x_calo","phoXcalo",VFLOAT,"photon supercluster position on calorimeter, x coordinate (cm)");//
    PhotonBM.makeBranch("y_calo","phoYcalo",VFLOAT,"photon supercluster position on calorimeter, y coordinate (cm)");//
    PhotonBM.makeBranch("z_calo","phoZcalo",VFLOAT,"photon supercluster position on calorimeter, z coordinate (cm)");//

    PhotonBM.makeBranch("esEffSigmaRR","phoEsEffSigmaRR",VFLOAT,"preshower sigmaRR");//
    PhotonBM.makeBranch("esEnergyOverRawE","phoEsEnergyOverRawE",VFLOAT,"ratio of preshower energy to raw supercluster energy");
    PhotonBM.makeBranch("haloTaggerMVAVal","phoHaloTaggerMVAVal",VFLOAT,"Value of MVA based beam halo tagger in the Ecal endcap (valid for pT > 200 GeV)");//

    PhotonBM.makeBranch("GenIdx","phoGenIdx",VINT);
    PhotonBM.makeBranch("GenDr","phoGenDr",VFLOAT);
    PhotonBM.makeBranch("GenDp","phoGenDp",VFLOAT);
    PhotonBM.makeBranch("GenSIdx","phoGenSIdx",VINT);
    PhotonBM.makeBranch("GenSDr","phoGenSDr",VFLOAT);
    PhotonBM.makeBranch("GenSDp","phoGenSDp",VFLOAT);

    PhotonBM.makeBranch("etaWidth","phoEtaWidth",VFLOAT,"Width of the photon supercluster in eta");//
    PhotonBM.makeBranch("phiWidth","phoPhiWidth",VFLOAT,"Width of the photon supercluster in phi");//
    PhotonBM.makeBranch("SMaj","phoSMaj",VFLOAT);
    PhotonBM.makeBranch("SMin","phoSMin",VFLOAT);
    PhotonBM.makeBranch("SAlp","phoSAlp",VFLOAT);
    PhotonBM.makeBranch("CovEtaEta","phoCovEtaEta",VFLOAT);
    PhotonBM.makeBranch("CovEtaPhi","phoCovEtaPhi",VFLOAT);
    PhotonBM.makeBranch("CovPhiPhi","phoCovPhiPhi",VFLOAT);

    PhotonBM.attachBranches(outTree);

}//<<>>setBranchesPhotons()

void KUCMSNtupilizer::processPhotons(){ 

    if( DEBUG ) std::cout << "Processing Photons" << std::endl;

	PhotonBM.clearBranches();

	uInt phoIdx = 0;
    for( const auto &photon : fphotons ){

		PhotonBM.fillBranch("IsOotPho",phoIsOotPho[phoIdx]);
        PhotonBM.fillBranch("Excluded",phoExcluded[phoIdx]);

        const float phoPt = photon.pt();
        const float phoEnergy = photon.energy();
        const float phoPhi = photon.phi();
        const float phoEta = photon.eta();
        const float phoPx = photon.px();
        const float phoPy = photon.py();
        const float phoPz = photon.pz();

		const float phoEnergyErr = photon.getCorrectedEnergyError(reco::Photon::regression2);
        const float haloTaggerMVAVal = photon.haloTaggerMVAVal();
		const bool phoHasPixelSeed = photon.hasPixelSeed();

        //const float phoHadOverEM = photon.hadronicOverEm();
        //const float phoHadOverEMVaid = photon.hadronicOverEmValid();
        const float phoHadTowOverEM = photon.hadTowOverEm();
        //const float phoHadTowOverEMValid = photon.hadTowOverEmValid();
        //const float phoMaxEnergyXtal = photon.maxEnergyXtal();
        //const float phoSigmaEtaEta = photon.sigmaEtaEta();
        //const float phoSigmaIEtaIEta = photon.sigmaIetaIeta();
        const float sieie = photon.full5x5_sigmaIetaIeta();
        const float sieip = photon.full5x5_showerShapeVariables().sigmaIetaIphi;
        const float sipip = photon.showerShapeVariables().sigmaIphiIphi;
		const float s4 = photon.full5x5_showerShapeVariables().e2x2/photon.full5x5_showerShapeVariables().e5x5;
		const float esEffSigmaRR = photon.full5x5_showerShapeVariables().effSigmaRR;

        //const float phoR1x5 = photon.r1x5();
        //const float phoR2x5 = photon.r2x5();
        const float phoR9 = photon.r9();

		const float hadronicOverEm = photon.hadronicOverEm();
        const float phoEcalRHSumEtConeDR04 = photon.ecalRecHitSumEtConeDR04();
        //const float phoHcalTwrSumEtConeDR04 = photon.hcalTowerSumEtConeDR04();
        const float phoHcalTowerSumEtBcConeDR04 = photon.hcalTowerSumEtBcConeDR04();
        const float phoTrkSumPtSolidConeDR04 = photon.trkSumPtSolidConeDR04();
        const float phoTrkSumPtHollowConeDR04 = photon.trkSumPtHollowConeDR04();
        //const float phoNTrkSolidConeDR04 = photon.nTrkSolidConeDR04();
        //const float phoNTrkHollowConeDR04 = photon.nTrkHollowConeDR04();
		const float phoTrkSumPtHollowConeDR03 = photon.trkSumPtHollowConeDR03();

		const float pfPhoIso03 = photon.photonIso();
		const float pfChargedIsoPFPV = photon.chargedHadronPFPVIso();
		const float pfChargedIsoWorstVtx = photon.chargedHadronWorstVtxIso();

        PhotonBM.fillBranch("Pt",phoPt);
        PhotonBM.fillBranch("Energy",phoEnergy);
        PhotonBM.fillBranch("Phi",phoPhi);
        PhotonBM.fillBranch("Eta",phoEta);
        PhotonBM.fillBranch("Px",phoPx);
        PhotonBM.fillBranch("Py",phoPy);
        PhotonBM.fillBranch("Pz",phoPz);

        PhotonBM.fillBranch("S4",s4);
        PhotonBM.fillBranch("esEffSigmaRR",esEffSigmaRR);
        //PhotonBM.fillBranch("SigmaEtaEta",phoSigmaEtaEta);
        PhotonBM.fillBranch("SigmaIEtaIEta",sieie);
        PhotonBM.fillBranch("SigmaIEtaIPhi",sieip);
        PhotonBM.fillBranch("SigmaIPhiIPhi",sipip);
        PhotonBM.fillBranch("EnergyErr",phoEnergyErr);
        PhotonBM.fillBranch("haloTaggerMVAVal",haloTaggerMVAVal);
        PhotonBM.fillBranch("hasPixelSeed",phoHasPixelSeed);

        PhotonBM.fillBranch("HadOverEM",hadronicOverEm);
        //PhotonBM.fillBranch("HadOverEMVaid",phoHadOverEmValid);
        PhotonBM.fillBranch("HadTowOverEM",phoHadTowOverEM);
        //PhotonBM.fillBranch("hadTowOverEMValid",phoHadTowOverEmValid);
        //PhotonBM.fillBranch("MaxEnergyXtal",phoMaxEnergyXtal);

        //PhotonBM.fillBranch("R1x5",phoR1x5);
        //PhotonBM.fillBranch("R2x5",phoR2x5);
        PhotonBM.fillBranch("R9",phoR9);

        PhotonBM.fillBranch("EcalRHSumEtConeDR04",phoEcalRHSumEtConeDR04);
        //PhotonBM.fillBranch("HcalTwrSumEtConeDR04",phoHcalTwrSumEtConeDR04);
        PhotonBM.fillBranch("HcalTowerSumEtBcConeDR04",phoHcalTowerSumEtBcConeDR04);
        PhotonBM.fillBranch("TrkSumPtSolidConeDR04",phoTrkSumPtSolidConeDR04);
        PhotonBM.fillBranch("TrkSumPtHollowConeDR04",phoTrkSumPtHollowConeDR04);
        PhotonBM.fillBranch("TrkSumPtHollowConeDR03",phoTrkSumPtHollowConeDR03);
        //PhotonBM.fillBranch("NTrkSolidConeDR04",phoNTrkSolidConeDR04);
        //PhotonBM.fillBranch("NTrkHollowConeDR04",phoNTrkHollowConeDR04);

        PhotonBM.fillBranch("pfPhoIso03",pfPhoIso03);
        PhotonBM.fillBranch("pfChargedIsoPFPV",pfChargedIsoPFPV);
        PhotonBM.fillBranch("pfChargedIsoWorstVtx",pfChargedIsoWorstVtx);

        if( DEBUG ) std::cout << " --- Proccesssing : " << photon << std::endl;
        const auto &phosc = photon.superCluster().isNonnull() ? photon.superCluster() : photon.parentSuperCluster();
        const auto scptr = phosc.get();

        const float phoEnergyRaw = scptr->rawEnergy();
		const bool isScEtaEB = abs(scptr->eta()) < 1.4442;
        const bool isScEtaEE = abs(scptr->eta()) > 1.566 && abs(scptr->eta()) < 2.5;
		const int seediEtaOriX = scptr->seedCrysIEtaOrIx();
		const int seediPhiOriY = scptr->seedCrysIPhiOrIy();
        const float x_calo = scptr->seed()->position().x();
        const float y_calo = scptr->seed()->position().y();
        const float z_calo = scptr->seed()->position().z();
        const float esEnergyOverRawE = scptr->preshowerEnergy()/phoEnergyRaw;
        const float etaWidth = scptr->etaWidth();
        const float phiWidth = scptr->phiWidth();

        const auto &seedDetId = scptr->seed()->seed();// seed detid
        const auto isEB = (seedDetId.subdetId() == EcalBarrel);// which subdet
       //const auto recHits = ( isEB ? &(*recHitsEB_) : &(*recHitsEE_) );
        const auto recHits = ( isEB ? recHitsEB_.product() : recHitsEE_.product() );

        if( DEBUG ) std::cout << " --- Finding Moments & Covariences : " << scptr << std::endl;
        const auto ph2ndMoments = noZS::EcalClusterTools::cluster2ndMoments( *(scptr->seed()), *recHits );
        const auto lCov = EcalClusterTools::covariances( *(scptr->seed()), recHits, ecalTopology, ecalGeometry );
        if( DEBUG ) std::cout << " --- Storing Moments & Covariences : " << scptr << std::endl;

        const float phoSMaj = ph2ndMoments.sMaj;
        const float phoSMin = ph2ndMoments.sMin;
        const float phoSAlp = ph2ndMoments.alpha;
        const float phoCovEtaEta = lCov[0];
        const float phoCovEtaPhi = lCov[1];
        const float phoCovPhiPhi = lCov[2];

        PhotonBM.fillBranch("isEB",isEB);
        PhotonBM.fillBranch("EnergyRaw",phoEnergyRaw);
        PhotonBM.fillBranch("isScEtaEB",isScEtaEB);
        PhotonBM.fillBranch("isScEtaEE",isScEtaEE);
        PhotonBM.fillBranch("seediEtaOriX",seediEtaOriX);
        PhotonBM.fillBranch("seediPhiOriY",seediPhiOriY);
        PhotonBM.fillBranch("x_calo",x_calo);
        PhotonBM.fillBranch("y_calo",y_calo);
        PhotonBM.fillBranch("z_calo",z_calo);
        PhotonBM.fillBranch("esEnergyOverRawE",esEnergyOverRawE);
        PhotonBM.fillBranch("etaWidth",etaWidth);
        PhotonBM.fillBranch("phiWidth",phiWidth);

        PhotonBM.fillBranch("SMaj",phoSMaj);
        PhotonBM.fillBranch("SMin",phoSMin);
        PhotonBM.fillBranch("SAlp",phoSAlp);
        PhotonBM.fillBranch("CovEtaEta",phoCovEtaEta);
        PhotonBM.fillBranch("CovEtaPhi",phoCovEtaPhi);
        PhotonBM.fillBranch("CovPhiPhi",phoCovPhiPhi);

		bool passelectronveto = conversions_.isValid() ? 
			not ConversionTools::hasMatchedPromptElectron( phosc, felectrons, *conversions_, beamSpot_->position() ) : false;
		PhotonBM.fillBranch("eleVeto",passelectronveto);

		if( DEBUG ) std::cout << " --- Gathering SC info : " << scptr << std::endl;
        scGroup phoSCGroup{*scptr};
        auto phoRhGroup = getRHGroup( phoSCGroup, 0.5 );
        auto phoRhIdsGroup = getRhGrpIDs( phoRhGroup );
        PhotonBM.fillBranch("RhIds",phoRhIdsGroup);
		setRecHitUsed(phoRhIdsGroup);
        if( DEBUG ) std::cout << " -- gedPhotons : " << scptr << " #: " << phoRhGroup.size() << std::endl;
        //auto tofTimes = getLeadTofRhTime( phoRhGroup, geVar("vtxX"), geVar("vtxY"), geVar("vtxZ") );
        //auto timeStats = getTimeDistStats( tofTimes, phoRhGroup );
        auto seedTOFTime = getSeedTofTime( *scptr, geVar("vtxX"), geVar("vtxY"), geVar("vtxZ") );
        //auto phoLeadTOFTime =  getLeadTofTime( phoRhGroup, geVar("vtxX"), geVar("vtxY"), geVar("vtxZ") );
        PhotonBM.fillBranch("SeedTOFTime",seedTOFTime);
        //PhotonBM.fillBranch("CMeanTime",timeStats[6]);

        // GenParticle Info for photon  -------------------------------------------------------------------
        if( cfFlag("hasGenInfo") ){

            auto genInfo = getGenPartMatch( scptr, fgenparts, phoPt );
			int idx = genInfo[0];
			int sidx = genInfo[3];
            PhotonBM.fillBranch("GenIdx",idx);
            PhotonBM.fillBranch("GenDr",genInfo[1]);
            PhotonBM.fillBranch("GenDp",genInfo[2]);
            PhotonBM.fillBranch("GenSIdx",sidx);
            PhotonBM.fillBranch("GenSDr",genInfo[4]);
            PhotonBM.fillBranch("GenSDp",genInfo[5]);
			if( DEBUG) std::cout << " Photon Match ------------------------- " << std::endl;

        }//<<>>if( hasGenInfo )

        phoIdx++;
    }//<<>>for( const auto &photon : fPhotons )

}//<<>>processPhotons( fphotons, fgenparts )

//-------------------------------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesElectrons(){

    ElectronBM.makeBranch("RhIds","eleRhIds",VVUINT); 
    ElectronBM.makeBranch("SeedTOFTime","eleSeedTOFTime",VFLOAT);
    ElectronBM.makeBranch("Pt","elePt",VFLOAT);
    ElectronBM.makeBranch("Energy","eleEnergy",VFLOAT);
    ElectronBM.makeBranch("Phi","elePhi",VFLOAT);
    ElectronBM.makeBranch("Eta","eleEta",VFLOAT);
    ElectronBM.makeBranch("Px","elePx",VFLOAT);
    ElectronBM.makeBranch("Py","elePy",VFLOAT);
    ElectronBM.makeBranch("Pz","elePz",VFLOAT);
    ElectronBM.makeBranch("GenIdx","eleGenIdx",VINT);
    ElectronBM.makeBranch("GenDr","eleGenDr",VFLOAT);
    ElectronBM.makeBranch("GenDr","eleGenDp",VFLOAT);
    ElectronBM.makeBranch("GenSIdx","eleGenSIdx",VINT);
    ElectronBM.makeBranch("GenSDr","eleGenSDr",VFLOAT);
    ElectronBM.makeBranch("GenSDr","eleGenSDp",VFLOAT);

    ElectronBM.attachBranches(outTree);

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

        ElectronBM.fillBranch("Pt",elePt);
        ElectronBM.fillBranch("Energy",eleEnergy);
        ElectronBM.fillBranch("Phi",elePhi);
        ElectronBM.fillBranch("Eta",eleEta);
        ElectronBM.fillBranch("Px",elePx);
        ElectronBM.fillBranch("Py",elePy);
        ElectronBM.fillBranch("Pz",elePz);

        if( DEBUG ) std::cout << " --- Proccesssing : " << electron << std::endl;
        const auto &elesc = electron.superCluster().isNonnull() ? electron.superCluster() : electron.parentSuperCluster();
        const auto scptr = elesc.get();
        scGroup eleSCGroup{*scptr};
        auto eleRhGroup = getRHGroup( eleSCGroup, 0.5 );
        auto eleRhIdsGroup = getRhGrpIDs( eleRhGroup );
        ElectronBM.fillBranch("RhIds",eleRhIdsGroup );
		setRecHitUsed( eleRhIdsGroup );
        if( DEBUG ) std::cout << " -- Electrons : " << scptr << " #: " << eleRhGroup.size() << std::endl;
        auto tofTimes = getLeadTofRhTime( eleRhGroup, geVar("vtxX"), geVar("vtxY"), geVar("vtxZ") );
        //auto timeStats = getTimeDistStats( tofTimes, eleRhGroup );
        float seedTOFTime = getSeedTofTime( *scptr, geVar("vtxX"), geVar("vtxY"), geVar("vtxZ") );

        ElectronBM.fillBranch("SeedTOFTime",seedTOFTime);

        // GenParticle Info for electron  -------------------------------------------------------------------
        if( cfFlag("hasGenInfo") ){

			auto genInfo = getGenPartMatch( scptr, fgenparts, elePt );
            if( DEBUG) std::cout << " Electron Match ------------------------- " << std::endl;
            ElectronBM.fillBranch("GenIdx",genInfo[0]);
            ElectronBM.fillBranch("GenDr",genInfo[1]);
            ElectronBM.fillBranch("GenDp",genInfo[2]);
            ElectronBM.fillBranch("GenSIdx",genInfo[3]);
            ElectronBM.fillBranch("GenSDr",genInfo[4]);
            ElectronBM.fillBranch("GenSDr",genInfo[5]);

        }//<<>>if( hasGenInfo )

    }//<<>>for( const auto electron : *electrons_ )

}//<<>>processElectrons( felectrons, fgenparts )

//----------------------------------------------------------------------
void KUCMSNtupilizer::setBranchesGenParts(){

    GenParticlesBM.makeBranch("genPt","genPt",VFLOAT);
    GenParticlesBM.makeBranch("genEnergy","genEnergy",VFLOAT);
    GenParticlesBM.makeBranch("genPhi","genPhi",VFLOAT);
    GenParticlesBM.makeBranch("genEta","genEta",VFLOAT);
    GenParticlesBM.makeBranch("genPx","genPx",VFLOAT);
    GenParticlesBM.makeBranch("genPy","genPy",VFLOAT);
    GenParticlesBM.makeBranch("genPz","genPz",VFLOAT);
    GenParticlesBM.makeBranch("genPdgId","genPdgId",VUINT);

	GenParticlesBM.attachBranches(outTree);

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

    EcalRecHitBM.makeBranch("rhEnergy","rhEnergy",VFLOAT);
    EcalRecHitBM.makeBranch("rhTime","rhTime",VFLOAT);
    EcalRecHitBM.makeBranch("rhTOF","rhTOF",VFLOAT);
    EcalRecHitBM.makeBranch("rhID","rhID",VUINT);
    EcalRecHitBM.makeBranch("rhisOOT","rhisOOT",VBOOL);
    EcalRecHitBM.makeBranch("rhSwCross","rhSwCross",VFLOAT);	

	EcalRecHitBM.attachBranches(outTree);

}//<<>>setBranchesRecHits()

void KUCMSNtupilizer::processRecHits(){

    if( DEBUG ) std::cout << "Processing RecHits" << std::endl;
    
	EcalRecHitBM.clearBranches();
	
	if( DEBUG ) std::cout << " - enetering RecHit loop" << std::endl;

	int nRecHits = frechits.size();
    for ( int it = 0; it < nRecHits; it++ ){

		auto recHit = frechits[it];
		auto used = frhused[it];
        //if( not used ) continue;
		if( not used && recHit.energy() < cfPrm("minRHEf") ) continue;

        const auto recHitID = getRawID(recHit);
        const bool isEB = getIsEB(recHit); // which subdet
        const auto geometry( isEB ? barrelGeometry : endcapGeometry );
        const auto recHitPos = geometry->getGeometry(recHit.detid())->getPosition();
        const auto rhX = recHitPos.x();
        const auto rhY = recHitPos.y();
        const auto rhZ = recHitPos.z();
        const float d_rh = hypo(rhX,rhY,rhZ)/SOL;
        const float d_pv = hypo(rhX-geVar("vtxX"),rhY-geVar("vtxY"),rhZ-geVar("vtxZ"))/SOL;
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


vector<float>  KUCMSNtupilizer::getGenPartMatch( const reco::SuperCluster* scptr, std::vector<reco::GenParticle> fgenparts , float pt ){

    // GenParticle Info   -------------------------------------------------------------------
    if( DEBUG ) std::cout << "Getting phoGenParton Information" << std::endl;
    // set defaults for no match here
    vector<float> results;
    float minDr(1.0);
    float minSDr(5.0);
	float minDp(1.0);
    float minSDp(5.0);
    int matchedIdx(-10);// 1
    int matchedSIdx(-10);// 1
    int index(0);
    for(const auto& genPart : fgenparts ){

        const auto rhX = scptr->x();
        const auto rhY = scptr->y();
        const auto rhZ = scptr->z();
        const auto gnX = genPart.vx();
        const auto gnY = genPart.vy();
        const auto gnZ = genPart.vz();
		const auto gpt = genPart.pt();
        auto cphoEta = std::asinh((rhZ-gnZ)/hypo(rhX-gnX,rhY-gnY));
        auto cphoPhi = std::atan2(rhY-gnY,rhX-gnX);
        auto dr = std::sqrt(reco::deltaR2(genPart.eta(), genPart.phi(), cphoEta, cphoPhi ));
		auto dp = std::abs(gpt-pt)/gpt;
        if( dr < minDr && dp < minDp ){
            minDr = dr;
            minDp = dp;
            matchedIdx = index;
        } else if( dr < minSDr && dp < minSDp ){
            minSDr = dr;
            minSDp = dp;
            matchedSIdx = index;
		}//<<>>if( dr < minDr && dp < minDp )
        index++;

    }//<<>>for(const auto& genPart : fgenparts  ) 

	float matDr(-1.0), matDp(-1.0), matSDr(-1.0), matSDp(-1.0);
	if( matchedIdx >=0 ){ matDr = minDr; matDp = minDp; matSDr = minSDr; matSDp = minDp; }

    results.push_back( matchedIdx );
    results.push_back( matDr );
    results.push_back( matDp );
    results.push_back( matchedSIdx );
    results.push_back( matSDr );
    results.push_back( matSDp );
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

