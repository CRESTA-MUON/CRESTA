#include "BristolPoCAFitter.hh"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnUserParameters.h"

namespace COSMIC {

bool debugger = false;

BristolPoCAFitter::BristolPoCAFitter() {
	xahits = NULL;
	xbhits = NULL;
	yahits = NULL;
	ybhits = NULL;
}


BristolPoCAFitter::~BristolPoCAFitter() {
}

void BristolPoCAFitter::ReadInputTTree(TTree* t, std::string prefixa, std::string prefixb) {

	fHits_Above_Type = new std::vector<int>(0, 0.0);
	fHits_Above_Reco = new std::vector<double>(0, 0.0);
	fHits_Above_True = new std::vector<double>(0, 0.0);
	fHits_Above_ZPos = new std::vector<double>(0, 0.0);
	fHits_Above_Error = new std::vector<double>(0, 0.0);
	fHits_Above_Ghost = new std::vector<double>(0, 0.0);
	t->SetBranchAddress( (prefixa + "_hit_type").c_str(),  &fHits_Above_Type);
	t->SetBranchAddress( (prefixa + "_hit_reco").c_str(),  &fHits_Above_Reco);
	t->SetBranchAddress( (prefixa + "_hit_true").c_str(),  &fHits_Above_True);
	t->SetBranchAddress( (prefixa + "_hit_zpos").c_str(),  &fHits_Above_ZPos);
	t->SetBranchAddress( (prefixa + "_hit_error").c_str(), &fHits_Above_Error);
	t->SetBranchAddress( (prefixa + "_hit_ghost").c_str(), &fHits_Above_Ghost);

	fHits_Below_Type = new std::vector<int>(0, 0.0);
	fHits_Below_Reco = new std::vector<double>(0, 0.0);
	fHits_Below_True = new std::vector<double>(0, 0.0);
	fHits_Below_ZPos = new std::vector<double>(0, 0.0);
	fHits_Below_Error = new std::vector<double>(0, 0.0);
	fHits_Below_Ghost = new std::vector<double>(0, 0.0);
	t->SetBranchAddress( (prefixb + "_hit_type").c_str(),  &fHits_Below_Type);
	t->SetBranchAddress( (prefixb + "_hit_reco").c_str(),  &fHits_Below_Reco);
	t->SetBranchAddress( (prefixb + "_hit_true").c_str(),  &fHits_Below_True);
	t->SetBranchAddress( (prefixb + "_hit_zpos").c_str(),  &fHits_Below_ZPos);
	t->SetBranchAddress( (prefixb + "_hit_error").c_str(), &fHits_Below_Error);
	t->SetBranchAddress( (prefixb + "_hit_ghost").c_str(), &fHits_Below_Ghost);

	fHits_Above_Combo = NULL;
	fHits_Below_Combo = NULL;
}

void BristolPoCAFitter::PreProcessData() {

	ApplyOffsets();



	for ( std::vector<MuonHit*>::iterator it = fMuonHits_Above.begin(); it != fMuonHits_Above.end(); ++it) {
		delete (*it);
	}
	fMuonHits_Above.clear();

	for ( std::vector<MuonHit*>::iterator it = fMuonHits_Below.begin(); it != fMuonHits_Below.end(); ++it) {
		delete (*it);
	}
	fMuonHits_Below.clear();

	std::map<int, std::vector<MuonHit*> >::iterator it2 = fMuonHits_Map.begin();
	for ( ; it2 != fMuonHits_Map.end(); it2++) {
		// for ( std::vector<MuonHit*>::iterator it = (it2->second).begin(); it != (it2->second).end(); ++it) {
		// 	if (*it) delete (*it);
		// }
		it2->second.clear();
	}
	fMuonHits_Map.clear();






	std::vector<int>::iterator    type_iter  = fHits_Above_Type->begin();
	std::vector<double>::iterator reco_iter  = fHits_Above_Reco->begin();
	std::vector<double>::iterator true_iter  = fHits_Above_True->begin();
	std::vector<double>::iterator zpos_iter  = fHits_Above_ZPos->begin();
	std::vector<double>::iterator error_iter = fHits_Above_Error->begin();
	std::vector<double>::iterator ghost_iter = fHits_Above_Ghost->begin();


	// Loop over all vectors
	while ( type_iter != fHits_Above_Type->end() ) {

		// Add to list
		fMuonHits_Above.push_back( new MuonHit( *reco_iter, *true_iter, *zpos_iter, *error_iter, *ghost_iter, *type_iter) );

		// Iterate
		type_iter++;
		reco_iter++;
		true_iter++;
		zpos_iter++;
		error_iter++;
		ghost_iter++;
	}

	type_iter  = fHits_Below_Type->begin();
	reco_iter  = fHits_Below_Reco->begin();
	true_iter  = fHits_Below_True->begin();
	zpos_iter  = fHits_Below_ZPos->begin();
	error_iter = fHits_Below_Error->begin();
	ghost_iter = fHits_Below_Ghost->begin();

	// Loop over all vectors
	while ( type_iter != fHits_Below_Type->end() ) {

		// Add to list
		fMuonHits_Below.push_back( new MuonHit( *reco_iter, *true_iter, *zpos_iter, *error_iter, *ghost_iter, *type_iter) );

		// Iterate
		type_iter++;
		reco_iter++;
		true_iter++;
		zpos_iter++;
		error_iter++;
		ghost_iter++;
	}



	xahits = NULL;
	xbhits = NULL;
	yahits = NULL;
	ybhits = NULL;

}

void BristolPoCAFitter::ApplyOffsets() {
	ApplyVectorOffset(fHits_Above_Reco);
	ApplyVectorOffset(fHits_Above_True);
	ApplyVectorOffset(fHits_Below_Reco);
	ApplyVectorOffset(fHits_Below_True);
}

void BristolPoCAFitter::ApplyVectorOffset(std::vector<double>* vec) {
  if (!vec) return;
  for (int i = 0; i < vec->size(); i++) {
    (*vec)[i] = (*vec)[i] - 250.0 / 1.5 + 250.0;
  }
}

std::vector<MuonHit*>* BristolPoCAFitter::GetHitCombination(int hitreq) const {

	if (fMuonHits_Map.find(hitreq) != fMuonHits_Map.end()) {
		return &fMuonHits_Map[hitreq];
	}
	std::vector<MuonHit*> temp;

	for (int i = 0; i < fMuonHits_Above.size(); i++) {
		MuonHit* val = (fMuonHits_Above[i]);
		if (IsValidHit(val->type, hitreq, false)) temp.push_back(val);
	}

	for (int i = 0; i < fMuonHits_Below.size(); i++) {
		MuonHit* val = (fMuonHits_Below[i]);
		if (IsValidHit(val->type, hitreq, true)) temp.push_back(val);
	}

	fMuonHits_Map[hitreq] = temp;

	return &fMuonHits_Map[hitreq];

}

double BristolPoCAFitter::DoEval(const double* x) const {

	double chi2 = 0.0;

	// Include all RPC and drift hits
	double pointx = x[0];
	double pointy = x[1];
	double pointz = x[2];
	double momx1  = x[3];
	double momx2  = x[4];
	double momy1  = x[5];
	double momy2  = x[6];

	if (!xahits) xahits = GetHitCombination(kAboveX);
	chi2 += EvaluateTrackResidual(pointx, momx2, pointz, xahits);

	if (!yahits) yahits = GetHitCombination(kAboveY);
	chi2 += EvaluateTrackResidual(pointy, momy2, pointz, yahits);

	if (!xbhits) xbhits = GetHitCombination(kBelowX);
	chi2 += EvaluateTrackResidual(pointx, momx1, pointz, xbhits);

	if (!ybhits) ybhits = GetHitCombination(kBelowY);
	chi2 += EvaluateTrackResidual(pointy, momy1, pointz, ybhits);

	return chi2;
}

bool BristolPoCAFitter::IsValidHit(int hittype, int hitrequired, bool isbelow) const {

	// If its a drift or RPC hit and we are ignoring it return false
	if (!fUseRPC   and (hittype == kRPCX || hittype == kRPCY) )     return false;
	if (!fUseDrift and (hittype == kDriftX || hittype == kDriftY) ) return false;

	// Accept all if required == 0
	if (hitrequired == 0) return true;

	// First check forexact hits
	if (hitrequired == kRPCAboveX   and !isbelow and hittype == kRPCX) return true;
	if (hitrequired == kRPCAboveY   and !isbelow and hittype == kRPCY) return true;
	if (hitrequired == kDriftAboveX and !isbelow and hittype == kDriftX) return true;
	if (hitrequired == kDriftAboveY and !isbelow and hittype == kDriftY) return true;

	if (hitrequired == kRPCBelowX and isbelow and hittype == kRPCX) return true;
	if (hitrequired == kRPCBelowY and isbelow  and hittype == kRPCY) return true;
	if (hitrequired == kDriftBelowX and isbelow and hittype == kDriftX) return true;
	if (hitrequired == kDriftBelowY and isbelow and hittype == kDriftY) return true;


	if (hitrequired == kAboveX and !isbelow and (hittype == kRPCX || hittype == kDriftX)) return true;
	if (hitrequired == kAboveY and !isbelow and (hittype == kRPCY || hittype == kDriftY)) return true;

	if (hitrequired == kBelowX and isbelow and (hittype == kRPCX || hittype == kDriftX)) return true;
	if (hitrequired == kBelowY and isbelow and (hittype == kRPCY || hittype == kDriftY)) return true;


	if (hitrequired == hittype) return true;
	// Now check for all Drifts
	// if (hitrequired == kRPCX and hittype == kRPCX) return true;
	// if (hitrequired == kRPCY and hittype == kRPCY) return true;
	// if (hitrequired == kDriftX and hittype == kDriftX) return true;
	// if (hitrequired == kDriftY and hittype == kDriftY) return true;

	if (hitrequired == kBelow and isbelow) return true;
	if (hitrequired == kAbove and !isbelow) return true;

	// Now check for all X or all Y
	if (hitrequired == kAllX and (hittype == kRPCX || hittype == kDriftX)) return true;
	if (hitrequired == kAllY and (hittype == kRPCY || hittype == kDriftY)) return true;

	// If none of the above
	return false;

}

bool BristolPoCAFitter::IsDriftHit(int hittype) const {
	return (hittype == kDriftX || hittype == kDriftY);
}

bool BristolPoCAFitter::IsRPCHit(int hittype) const {
	return (hittype == kRPCX || hittype == kRPCY);
}


double BristolPoCAFitter::EvaluateTrackResidual( double pointx, double gradient, double pointz, int hitreq ) const {
	return EvaluateTrackResidual(pointx, gradient, pointz, GetHitCombination(hitreq) );
}


double BristolPoCAFitter::EvaluateTrackResidual( double pointx, double gradient, double pointz, std::vector<MuonHit*>* xahits ) const {

	double chi2 = 0.0;

	std::vector<MuonHit*>::const_iterator xahits_iter = xahits->begin();
	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		double single = ( hit->GetHit() - (pointx + gradient * (pointz - hit->zpos) ) ) / hit->error;
		chi2 += single * single;
	}

	return chi2;
}

double BristolPoCAFitter::GetAverageHit(int hitreq) {

	double average = 0.0;
	int ncount = 0;
	std::vector<MuonHit*>* xahits = GetHitCombination(hitreq);
	std::vector<MuonHit*>::iterator xahits_iter = xahits->begin();

	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		average += hit->GetHit();
		ncount++;
	}

	if (ncount == 0) return 0.0;
	return average / double(ncount);
}

double BristolPoCAFitter::GetAverageZ(int hitreq) {

	double average = 0.0;
	int ncount = 0;

	std::vector<MuonHit*>* xahits = GetHitCombination(hitreq);
	std::vector<MuonHit*>::iterator xahits_iter = xahits->begin();

	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		average += hit->zpos;
		ncount++;
	}

	if (ncount == 0) return 0.0;
	return average / double(ncount);
}


double BristolPoCAFitter::GetLowestZ(int hitreq) {
	double minz = -999.;

	std::vector<MuonHit*>* xahits = GetHitCombination(hitreq);
	std::vector<MuonHit*>::iterator xahits_iter = xahits->begin();
	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		if (minz < hit->zpos || minz == -999.) minz = hit->zpos;
	}

	return minz;
}

double BristolPoCAFitter::GetHighestZ(int hitreq) {
	double maxz = -999.;

	std::vector<MuonHit*>* xahits = GetHitCombination(hitreq);
	std::vector<MuonHit*>::iterator xahits_iter = xahits->begin();
	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		if (maxz > hit->zpos || maxz == -999.) maxz = hit->zpos;
	}

	return maxz;
}

double BristolPoCAFitter::GetLowestHit(int hitreq) {
	double minz = -999.;
	double minhit = -999.;

	std::vector<MuonHit*>* xahits = GetHitCombination(hitreq);
	std::vector<MuonHit*>::iterator xahits_iter = xahits->begin();
	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		if (minz < hit->zpos || minz == -999.) {
			minz = hit->zpos;
			minhit = hit->GetHit();
		}
	}

	return minhit;
}

double BristolPoCAFitter::GetHighestHit(int hitreq) {
	double maxz = -999.;
	double maxhit = -999.;

	std::vector<MuonHit*>* xahits = GetHitCombination(hitreq);
	std::vector<MuonHit*>::iterator xahits_iter = xahits->begin();
	for ( ; xahits_iter != xahits->end(); xahits_iter++) {
		MuonHit* hit = (MuonHit*)(*xahits_iter);
		if (maxz > hit->zpos || maxz == -999.) {
			maxz = hit->zpos;
			maxhit = hit->GetHit();
		}
	}

	return maxhit;
}

int BristolPoCAFitter::GetNHits(int hitreq) {
	return GetHitCombination(hitreq)->size();
}


// ---------------------------------------

void BristolPoCAFitter::FillComboVect(std::vector<std::vector<bool> >& combomap, int ndrifthits ) {

	// Make drift only combinations
	std::vector<std::vector<bool> > driftcombos;
	for (int i = 0; i < (1 << ndrifthits); i++) {
		std::vector<bool> combo;
		for (int j = 0; j < ndrifthits; j++) {
			combo.push_back(((i & (1 << j)) ? 1 : 0));
		}
		combomap.push_back(combo);
	}

}

double BristolPoCAFitter::DoSingleEval(const double* x, std::vector<MuonHit*>* hits) const {
	return EvaluateTrackResidual(x[0], x[1], 0.0, hits);
}


double BristolPoCAFitter::DoSingleTrackFitWithX(int hitreq, double* x, double* px) {

	ROOT::Minuit2::MnUserParameters mn_param;

	double startx = GetAverageHit(hitreq);
	mn_param.Add("x", startx,  0.1);
	mn_param.Add("px1", 0.0, 10.0);

	std::vector<MuonHit*>* hits = GetHitCombination(hitreq);

	CHANCESingleTrackFitterFCN* singlefcn = new CHANCESingleTrackFitterFCN(this, hitreq);
	ROOT::Minuit2::MnMigrad migrad( *singlefcn, mn_param, 2 ); //Strategy 2
	ROOT::Minuit2::FunctionMinimum min = migrad();
	ROOT::Minuit2::MnAlgebraicVector MinParams = min.Parameters().Vec();
	double xx[2];
	xx[0] = MinParams[0];
	xx[1] = MinParams[1];
	double chi2 = DoSingleEval(xx, hits);
	if (x) *x = xx[0];
	if (px) *px = xx[1];

	delete singlefcn;
	return chi2;

}

void SetHitCombo(std::vector<MuonHit*>* muonhits, std::vector<bool>* combo) {
	for (int i = 0; i < muonhits->size(); i++) {
		(*muonhits)[i]->useghost = (*combo)[i];
	}
}

std::vector<bool> BristolPoCAFitter::GetBestComboForDriftHits(int hitreq) {

	std::vector<MuonHit*>* muonhits = GetHitCombination(hitreq);
	int nhits = muonhits->size();
	std::vector<std::vector<bool> > xcombomapa;
	std::vector<bool> xcomboa;

	FillComboVect(xcombomapa, nhits);

	// Loop over all iterations, find best chi2
	double bestchi2 = -1;
	int n = xcombomapa.size();
	int jointreq = 0;

	int fullhitreq = hitreq;
	if (hitreq == kDriftAboveX and fUseRPC) fullhitreq = kAboveX;
	if (hitreq == kDriftAboveY and fUseRPC) fullhitreq = kAboveY;
	if (hitreq == kDriftBelowX and fUseRPC) fullhitreq = kBelowX;
	if (hitreq == kDriftBelowY and fUseRPC) fullhitreq = kBelowY;

	// Iterate over all combinations
	for (int j = 0; j < n; j++) {

		SetHitCombo( muonhits, &xcombomapa[j] );

		// Get the results
		double chi2 = DoSingleTrackFitWithX(fullhitreq);

		if (bestchi2 < 0 or chi2 <= bestchi2) {
			// if (j != 0){
			// std::cout << "Adding  ghost combo " << j << " " << chi2 << " " << bestchi2 << std::endl;
			// }
			xcomboa = xcombomapa[j];
			bestchi2 = chi2;

		}
	}

	// std::cout << " Best Chi2 From Drift : " << bestchi2 << std::endl;

	SetHitCombo(muonhits, &xcomboa);
	// muonhits = GetHitCombination(hitreq);
	// for (int i = 0; i < muonhits->size(); i++){
// std::cout << "SINGLE HIT " << muonhits->at(i)->type << " -> " << muonhits->at(i)->useghost << std::endl;
	// }
	return xcomboa;
}

void BristolPoCAFitter::PrintCombos() {

	std::vector<MuonHit*>* muonhits = GetHitCombination(0);
	for (int i = 0; i < muonhits->size(); i++) {
		std::cout << "HIT " << muonhits->at(i)->type << " -> " << muonhits->at(i)->useghost << std::endl;
	}

}

void BristolPoCAFitter::PerformDoubleTrackPoCAFit(double* pocafitparams) {

	// Get fit values
	double temp_above_x = 0.0;
	double temp_above_px = 0.0;
	double temp_above_y = 0.0;
	double temp_above_py = 0.0;

	double temp_below_x = 0.0;
	double temp_below_px = 0.0;
	double temp_below_y = 0.0;
	double temp_below_py = 0.0;

	// // Perform an Upper X fit
	DoSingleTrackFitWithX(kAboveX, &temp_above_x, &temp_above_px);

	// // Perform an Upper Y fit
	DoSingleTrackFitWithX(kAboveY, &temp_above_y, &temp_above_py);

	// // Perform an Lower X fit
	DoSingleTrackFitWithX(kBelowX, &temp_below_x, &temp_below_px);

	// // Perform an Lower Y fit
	DoSingleTrackFitWithX(kBelowY, &temp_below_y, &temp_below_py);


	// Perform a dodgy joint ABOVE+BELOW X Fit
	double xChi2 = DoSingleTrackFitWithX(kAllX);
	double yChi2 = DoSingleTrackFitWithX(kAllY);


	// Get vectors from the fits
	TVector3 posA = TVector3(temp_above_x, temp_above_y, 0.0);
	TVector3 momA = TVector3(temp_above_px, temp_above_py, 1.0);

	TVector3 posB = TVector3(temp_below_x, temp_below_y, 0.0);
	TVector3 momB = TVector3(temp_below_px, temp_below_py, 1.0);

	// Get the PoCA
	TVector3 w0 = posA - posB;
	double a = momA.Dot(momA);
	double b = momA.Dot(momB);
	double c = momB.Dot(momB);
	double d = momA.Dot(w0);
	double e = momB.Dot(w0);

	double sc = ((b * e) - (c * d)) / ((a * c) - (b * b));
	double tc = ((a * e) - (b * d)) / ((a * c) - (b * b));

	TVector3 poca = 0.5 * (posA + sc * momA + posB + tc * momB);

	// Get the distance between points of closest approach
	// double distance = (posA + sc*momB - posB - tc*momB).Mag();

	// Get the scatter angles
	TVector3 grad1X(temp_above_px, 0., 1.);
	TVector3 grad2X(temp_below_px, 0., 1.);

	TVector3 grad1Y(0., temp_above_py, 1.);
	TVector3 grad2Y(0., temp_below_py, 1.);

	TVector3 grad1(temp_above_px, temp_above_py, 1.);
	TVector3 grad2(temp_below_px, temp_below_py, 1.);

	// double scatter_x  = grad1X.Angle(grad2X);
	// double scatter_y  = grad1Y.Angle(grad2Y);
	double scatter_3d = grad2.Angle(grad1);

	// Fill variables and return
	pocafitparams[0] = temp_above_x;
	pocafitparams[1] = temp_above_px;
	pocafitparams[2] = temp_above_y;
	pocafitparams[3] = temp_above_py;
	pocafitparams[4] = temp_below_x;
	pocafitparams[5] = temp_below_px;
	pocafitparams[6] = temp_below_y;
	pocafitparams[7] = temp_below_py;

	pocafitparams[8]  = atan( abs((temp_above_px - temp_below_px) / (1 + temp_above_px * temp_below_px)) );
	pocafitparams[9]  = atan( abs((temp_above_py - temp_below_py) / (1 + temp_above_py * temp_below_py)) );
	pocafitparams[10] = scatter_3d;

	// std::cout << "Filling" << std::endl;
	pocafitparams[11] = poca[0];
	pocafitparams[12] = poca[1];
	pocafitparams[13] = -poca[2];
	pocafitparams[14] = xChi2;
	pocafitparams[15] = yChi2;


	return;
}

}









