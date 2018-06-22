#include "BristolPoCAFitter.hh"
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "sd/LongDriftSD.hh"
#include "sd/SimpleScintillatorSD.hh"

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnUserParameters.h"



namespace COSMIC {

bool debugger = false;

BristolPoCAFitter::BristolPoCAFitter() {

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

	// if (fHits_Above_Combo) std::cout << "EVAL " << fHits_Above_Reco->size() << " " << fHits_Above_Type->size() << " " << fHits_Above_Combo << " " << fHits_Above_Combo->size() <<  std::endl;
	// if (fHits_Below_Combo) {
	// std::cout << "EVAL2 " << fHits_Below_Reco->size() << " " << fHits_Below_Type->size() << " " << fHits_Below_Combo << std::endl; std::cout << " -> " << fHits_Below_Combo->size() << std::endl;
	// }

	// for (uint i = 0; i < fHits_Above_Reco->size(); i++) {
	// 	int hittype = fHits_Above_Type->at(i);

	// 	double point    = 0.0;
	// 	double gradient = 0.0;
	// 	if (IsValidHit(hittype, kAboveX, false)) { point = pointx; gradient = momx2; }
	// 	else if (IsValidHit(hittype, kAboveY, false)) { point = pointy; gradient = momy2; }
	// 	else continue;

	// 	double x = fHits_Above_Reco->at(i);
	// 	double g = fHits_Above_Ghost->at(i);
	// 	double z = fHits_Above_ZPos->at(i);
	// 	double e = fHits_Above_Error->at(i);



	// 	if (!fHits_Above_Combo->at(i)) chi2 += pow( ( x - (point + gradient * (pointz - z) ) ) / e, 2 );
	// 	else chi2 += pow( ( g - (point + gradient * (pointz - z) ) ) / e, 2 );

	// }

	// for (uint i = 0; i < fHits_Below_Reco->size(); i++) {
	// 	int hittype = fHits_Below_Type->at(i);

	// 	double point    = 0.0;
	// 	double gradient = 0.0;
	// 	if (IsValidHit(hittype, kBelowX, true)) { point = pointx; gradient = momx1; }
	// 	else if (IsValidHit(hittype, kBelowY, true)) { point = pointy; gradient = momy1; }
	// 	else continue;

	// 	double x = fHits_Below_Reco->at(i);
	// 	double g = fHits_Below_Ghost->at(i);
	// 	double z = fHits_Below_ZPos->at(i);
	// 	double e = fHits_Below_Error->at(i);


	// 	if (!fHits_Below_Combo->at(i)) chi2 += pow( ( x - (point + gradient * (pointz - z) ) ) / e, 2 );
	// 	else chi2 += pow( ( g - (point + gradient * (pointz - z) ) ) / e, 2 );
	// }


	chi2 += EvaluateTrackResidual( pointx, momx2, pointz, kRPCAboveX);
	chi2 += EvaluateTrackResidual( pointy, momy2, pointz, kRPCAboveY);
	chi2 += EvaluateTrackResidual( pointx, momx1, pointz, kRPCBelowX);
	chi2 += EvaluateTrackResidual( pointy, momy1, pointz, kRPCBelowY);

	chi2 += EvaluateTrackResidual( pointx, momx2, pointz, kDriftAboveX);
	chi2 += EvaluateTrackResidual( pointy, momy2, pointz, kDriftAboveY);
	chi2 += EvaluateTrackResidual( pointx, momx1, pointz, kDriftBelowX);
	chi2 += EvaluateTrackResidual( pointy, momy1, pointz, kDriftBelowY);

	// std::cout << "Final chi2 : " << chi2 << std::endl;
	// sleep(10);

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

	double chi2 = 0.0;

	// if (fHits_Above_Combo) std::cout << "EVAL " << fHits_Above_Reco->size() << " " << fHits_Above_Type->size() << " " << fHits_Above_Combo << " " << fHits_Above_Combo->size() <<  std::endl;
	// if (fHits_Below_Combo) {
	// std::cout << "EVAL2 " << fHits_Below_Reco->size() << " " << fHits_Below_Type->size() << " " << fHits_Below_Combo << std::endl; std::cout << " -> " << fHits_Below_Combo->size() << std::endl;
	// }

	for (uint i = 0; i < fHits_Above_Reco->size(); i++) {
		int hittype = fHits_Above_Type->at(i);

		if (!IsValidHit(hittype, hitreq, false)) continue;

		double x = fHits_Above_Reco->at(i);
		double g = fHits_Above_Ghost->at(i);
		double z = fHits_Above_ZPos->at(i);
		double e = fHits_Above_Error->at(i);

		if (!fHits_Above_Combo->at(i)) chi2 += pow( ( x - (pointx + gradient * (pointz - z) ) ) / e, 2 );
		else chi2 += pow( ( g - (pointx + gradient * (pointz - z) ) ) / e, 2 );

	}

	for (uint i = 0; i < fHits_Below_Reco->size(); i++) {
		int hittype = fHits_Below_Type->at(i);

		if (!IsValidHit(hittype, hitreq, true)) continue;

		double x = fHits_Below_Reco->at(i);
		double g = fHits_Below_Ghost->at(i);
		double z = fHits_Below_ZPos->at(i);
		double e = fHits_Below_Error->at(i);

		if (!fHits_Below_Combo->at(i)) chi2 += pow( ( x - (pointx + gradient * (pointz - z) ) ) / e, 2 );
		else chi2 += pow( ( g - (pointx + gradient * (pointz - z) ) ) / e, 2 );
	}

	// std::cout << "Chi2 : " << chi2 << " " << hitreq << std::endl;
	// if (pointx != 0.0) sleep(10);

	return chi2;
}

double BristolPoCAFitter::GetAverageHit(int hitreq) {

	double average = 0.0;
	int ncount = 0;

	for (uint i = 0; i < fHits_Above_Reco->size(); i++) {
		int hittype = fHits_Above_Type->at(i);

		if (!IsValidHit(hittype, hitreq, false)) continue;

		if (fHits_Above_Combo->at(i)) average += fHits_Above_Ghost->at(i);
		else  average += fHits_Above_Reco->at(i);
		ncount++;
	}

	for (uint i = 0; i < fHits_Below_Reco->size(); i++) {
		int hittype = fHits_Below_Type->at(i);

		if (!IsValidHit(hittype, hitreq, true)) continue;

		if (fHits_Below_Combo->at(i)) average += fHits_Below_Ghost->at(i);
		else average += fHits_Below_Reco->at(i);
		ncount++;
	}

	if (ncount == 0) return 0.0;
	return average / double(ncount);
}

double BristolPoCAFitter::GetAverageZ(int hitreq) {

	double average = 0.0;
	int ncount = 0;

	for (uint i = 0; i < fHits_Above_ZPos->size(); i++) {
		int hittype = fHits_Above_Type->at(i);

		if (!IsValidHit(hittype, hitreq, false)) continue;
		average += fHits_Above_ZPos->at(i);
		ncount++;
	}

	for (uint i = 0; i < fHits_Below_ZPos->size(); i++) {
		int hittype = fHits_Below_Type->at(i);

		if (!IsValidHit(hittype, hitreq, true)) continue;
		average += fHits_Below_ZPos->at(i);
		ncount++;
	}

	if (ncount == 0) return 0.0;
	return average / double(ncount);
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

int BristolPoCAFitter::GetNHits(int hitreq) {

	int ncount = 0;
	for (uint i = 0; i < fHits_Above_Type->size(); i++) {
		int hittype = fHits_Above_Type->at(i);
		if (!IsValidHit(hittype, hitreq, false)) continue;
		ncount++;
	}
	for (uint i = 0; i < fHits_Below_Type->size(); i++) {
		int hittype = fHits_Below_Type->at(i);
		if (!IsValidHit(hittype, hitreq, true)) continue;
		ncount++;
	}

	return ncount;
}


// ---------------------------------------

void BristolPoCAFitter::FillComboVect(std::vector<std::vector<bool> >& combomap, int hitreq ) {

	// Get Number of Drift Chambers
	int ndrifthits = GetNHits(hitreq);

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

double BristolPoCAFitter::DoSingleEval(const double* x, int hitreq) const {

	double chi2 = 0.0;

	// Include all RPC and drift hits
	double pointx = x[0];
	double momx   = x[1];
	double pointz = 0.0;

	// Get individual Track Fit
	chi2 += EvaluateTrackResidual(pointx, momx, pointz, hitreq);

	return chi2;
}


double BristolPoCAFitter::DoSingleTrackFitWithX(int hitreq, double* x, double* px) {

	ROOT::Minuit2::MnUserParameters mn_param;
	double startx = GetAverageHit(hitreq);

    mn_param.Add("x", startx,  0.1);
    mn_param.Add("px1", 0.0, 10.0);


	CHANCESingleTrackFitterFCN* singlefcn = new CHANCESingleTrackFitterFCN(this, hitreq);
    ROOT::Minuit2::MnMigrad migrad( *singlefcn, mn_param, 2 ); //Strategy 2
    ROOT::Minuit2::FunctionMinimum min = migrad();
    ROOT::Minuit2::MnAlgebraicVector MinParams = min.Parameters().Vec();
    double xx[2];
    xx[0] = MinParams[0];
    xx[1] = MinParams[1];
    double chi2 = DoSingleEval(xx, hitreq);
    if (x) *x = xx[0];
	if (px) *px = xx[1];
	return chi2;

 //    // Minimisation
 //    ROOT::Minuit2::MnMigrad migrad( *pocafit, mn_param, 2 ); //Strategy 2
 //    ROOT::Minuit2::FunctionMinimum min = migrad();

 //    ROOT::Minuit2::MnAlgebraicSymMatrix MinCovarMatrix = min.Error().Matrix();
 //    ROOT::Minuit2::MnAlgebraicVector MinParams = min.Parameters().Vec();

 //    for (int j = 0; j < npars; j++) {
 //      fBestFitPars[j] = MinParams[j];
 //    }





	// // Create Minimizer Object
	// ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
	// min->SetPrintLevel(-1);

	// // Setup Functor for the fitter
	// int npars = 2;
	// CHANCESingleTrackFitterFCN* singlefcn = new CHANCESingleTrackFitterFCN(this, hitreq);
	// ROOT::Math::Functor func(*singlefcn, npars);
	// min->SetFunction(func);

	// double startx = GetAverageHit(hitreq);

	// // Run the fit
	// min->SetVariable(0, "x",   startx,  0.1);
	// min->SetVariable(1, "px1", 0.0,  10.0);
	// min->Minimize();

	// // Get the results
	// const double* xx = min->X();
	// double chi2 = DoSingleEval(xx, hitreq);
	// if (x) *x = xx[0];
	// if (px) *px = xx[1];

	// // Clean and return
	// delete min;
	// return chi2;
}


std::vector<bool> BristolPoCAFitter::GetBestComboForDriftHits(int hitreq) {

	// Get Combo Map
	std::vector<std::vector<bool> > xcombomapa;
	std::vector<bool> xcomboa;
	FillComboVect( xcombomapa, hitreq );

	// Loop over all iterations, find best chi2
	double bestchi2 = -1;
	int n = xcombomapa.size();
	int jointreq = 0;

	// Iterate over all combinations
	for (int j = 0; j < n; j++) {

		// Update combination based on type
		if (hitreq == kDriftAboveX) {
			jointreq = kAboveX;
			SetAboveComboX( &xcombomapa[j] );
		}
		if (hitreq == kDriftAboveY) {
			jointreq = kAboveY;
			SetAboveComboY( &xcombomapa[j] );
		}
		if (hitreq == kDriftBelowX) {
			jointreq = kBelowX;
			SetBelowComboX( &xcombomapa[j] );
		}
		if (hitreq == kDriftBelowY) {
			jointreq = kBelowY;
			SetBelowComboY( &xcombomapa[j] );
		}

		// Get the results
		double chi2 = DoSingleTrackFitWithX(jointreq);

		// Somehow end results can sometimes give same chi2
		// If better than current, update
		// std::cout << j << " CHI2 " << chi2 << " : " << xcombomapa[j][0] << std::endl;
		if (bestchi2 < 0 or chi2 <= bestchi2) {


			if (chi2 == bestchi2) {
				std::cout << "Chi2 Match somehow!" << std::endl;

				for (int i = 0; i < xcomboa.size(); i++) {
					std::cout << i << " C : " << xcomboa[i] << " vs " << xcombomapa[j][i] << std::endl;
				}

				sleep(10);
			}



			xcomboa = xcombomapa[j];
			bestchi2 = chi2;
			// std::cout << j << " Best CHI2 : " << bestchi2 << std::endl;
		}
	}

	return xcomboa;
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
	double xChi2 = 0.0; //DoSingleTrackFitWithX(kAllX);
	double yChi2 = 0.0; //DoSingleTrackFitWithX(kAllY);


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

double BristolPoCAFitter::GetLowestZ(int hitreq) {
	double minz = -999.;

	for (uint i = 0; i < fHits_Above_ZPos->size(); i++) {
		int hittype = fHits_Above_Type->at(i);
		if (!IsValidHit(hittype, hitreq, false)) continue;
		if (fHits_Above_ZPos->at(i) < minz || minz == -999.) {
			minz = fHits_Above_ZPos->at(i);
		}
	}

	for (uint i = 0; i < fHits_Below_ZPos->size(); i++) {
		int hittype = fHits_Below_Type->at(i);
		if (!IsValidHit(hittype, hitreq, true)) continue;
		if (fHits_Below_ZPos->at(i) < minz || minz == -999.) {
			minz = fHits_Below_ZPos->at(i);
		}
	}

	return minz;
}

double BristolPoCAFitter::GetHighestZ(int hitreq) {
	double maxz = -999.;

	for (uint i = 0; i < fHits_Above_ZPos->size(); i++) {
		int hittype = fHits_Above_Type->at(i);
		if (!IsValidHit(hittype, hitreq, false)) continue;
		if (fHits_Above_ZPos->at(i) > maxz || maxz == -999.) {
			maxz = fHits_Above_ZPos->at(i);
		}
	}

	for (uint i = 0; i < fHits_Below_ZPos->size(); i++) {
		int hittype = fHits_Below_Type->at(i);
		if (!IsValidHit(hittype, hitreq, true)) continue;
		if (fHits_Below_ZPos->at(i) > maxz || maxz == -999.) {
			maxz = fHits_Below_ZPos->at(i);
		}
	}

	return maxz;
}

double BristolPoCAFitter::GetLowestHit(int hitreq) {
	double minz = -999.;
	double minhit = -999.;

	for (uint i = 0; i < fHits_Above_ZPos->size(); i++) {
		int hittype = fHits_Above_Type->at(i);
		if (!IsValidHit(hittype, hitreq, false)) continue;
		if (fHits_Above_ZPos->at(i) < minz || minz == -999.) {
			minz = fHits_Above_ZPos->at(i);
			if (!fHits_Above_Combo->at(i)) {
				minhit = fHits_Above_Reco->at(i);
			} else {
				minhit = fHits_Above_Ghost->at(i);
			}
		}
	}

	for (uint i = 0; i < fHits_Below_ZPos->size(); i++) {
		int hittype = fHits_Below_Type->at(i);
		if (!IsValidHit(hittype, hitreq, true)) continue;
		if (fHits_Below_ZPos->at(i) < minz || minz == -999.) {
			minz = fHits_Below_ZPos->at(i);
			if (!fHits_Below_Combo->at(i)) {
				minhit = fHits_Below_Reco->at(i);
			} else {
				minhit = fHits_Below_Ghost->at(i);
			}
		}
	}

	return minhit;
}

double BristolPoCAFitter::GetHighestHit(int hitreq) {
	double maxz = -999.;
	double maxhit = -999.;

	for (uint i = 0; i < fHits_Above_ZPos->size(); i++) {
		int hittype = fHits_Above_Type->at(i);
		if (!IsValidHit(hittype, hitreq, false)) continue;
		if (fHits_Above_ZPos->at(i) > maxz || maxz == -999.) {
			maxz = fHits_Above_ZPos->at(i);
			if (!fHits_Above_Combo->at(i)) {
				maxhit = fHits_Above_Reco->at(i);
			} else {
				maxhit = fHits_Above_Ghost->at(i);
			}
		}
	}

	for (uint i = 0; i < fHits_Below_ZPos->size(); i++) {
		int hittype = fHits_Below_Type->at(i);
		if (!IsValidHit(hittype, hitreq, true)) continue;
		if (fHits_Below_ZPos->at(i) > maxz || maxz == -999.) {
			maxz = fHits_Below_ZPos->at(i);
			if (!fHits_Below_Combo->at(i)) {
				maxhit = fHits_Below_Reco->at(i);
			} else {
				maxhit = fHits_Below_Ghost->at(i);

			}
		}
	}

	return maxhit;
}

void BristolPoCAFitter::SetAboveComboX(std::vector<bool>* acombo) {

	// Make if needed
	if (!fHits_Above_Combo) {
		fHits_Above_Combo = new std::vector<bool>(fHits_Above_Type->size(), false);
		fHits_Above_Combo->resize( fHits_Above_Type->size() );
	}

	if (debugger) {
		// Print Initial Combo
		std::cout << "Setting Combo" << std::endl;
		for (int i = 0; i < acombo->size(); i++) {
			std::cout << "--> Drift " << i << " : " << acombo->at(i) << std::endl;
		}
	}

	// Loop over all hits above
	int driftcount = 0;
	for (int i = 0; i < fHits_Above_Type->size(); i++) {
		int hittype = fHits_Above_Type->at(i);

		if (!IsValidHit(hittype, kAboveX, false)) continue;
		if (debugger) {
			std::cout << "Above Hit Types " << hittype << std::endl;
		}

		if (IsRPCHit(hittype)) {
			(*fHits_Above_Combo)[i] = false;
		} else {
			(*fHits_Above_Combo)[i] = (*acombo)[driftcount];
			driftcount++;
		}
	}

	if (debugger) {
		// Print Combo
		std::cout << "Full Combo" << std::endl;
		for (int i = 0; i < fHits_Above_Type->size(); i++) {
			int hittype = fHits_Above_Type->at(i);
			if (IsRPCHit(hittype)) {
				std::cout << "--> RPC " << i << " : " << hittype << " " << (*fHits_Above_Combo)[i] << std::endl;
			} else {
				std::cout << "--> Drift " << i << " : " << hittype << " " << (*fHits_Above_Combo)[i] << std::endl;
			}
			// if (hittype == 4 and (*fHits_Above_Combo)[i]) sleep(2);
		}
	}

}

void BristolPoCAFitter::SetAboveComboY(std::vector<bool>* acombo) {

	// Make if needed
	if (!fHits_Above_Combo) {
		fHits_Above_Combo = new std::vector<bool>(fHits_Above_Type->size(), false);
		fHits_Above_Combo->resize( fHits_Above_Type->size() );
	}

	if (debugger) {
		std::cout << "Setting Combo Y" << std::endl;
		for (int i = 0; i < acombo->size(); i++) {
			std::cout << "--> Drift " << i << " : " << acombo->at(i) << std::endl;
		}
	}


	// Loop over all hits above
	int driftcount = 0;
	for (int i = 0; i < fHits_Above_Type->size(); i++) {
		int hittype = fHits_Above_Type->at(i);

		if (!IsValidHit(hittype, kAboveY, false)) continue;
		if (IsRPCHit(hittype)) {
			(*fHits_Above_Combo)[i] = false;
		} else {
			(*fHits_Above_Combo)[i] = (*acombo)[driftcount];
			driftcount++;
		}
	}

	if (debugger) {
		// Print Combo
		std::cout << "Full Combo Y" << std::endl;
		for (int i = 0; i < fHits_Above_Type->size(); i++) {
			int hittype = fHits_Above_Type->at(i);
			if (IsRPCHit(hittype)) {
				std::cout << "--> RPC " << i << " : " << hittype << " " << (*fHits_Above_Combo)[i] << std::endl;
			} else {
				std::cout << "--> Drift " << i << " : " << hittype << " " << (*fHits_Above_Combo)[i] << std::endl;
			}
			// if (hittype == 4 and (*fHits_Above_Combo)[i]) sleep(2);
		}
	}


}

void BristolPoCAFitter::SetBelowComboX(std::vector<bool>* bcombo) {

	// Make if needed
	if (!fHits_Below_Combo) {
		fHits_Below_Combo = new std::vector<bool>(fHits_Below_Type->size(), false);
		fHits_Below_Combo->resize( fHits_Below_Type->size() );
	}

	if (debugger) {
		std::cout << "Below Setting Combo X " << std::endl;
		for (int i = 0; i < bcombo->size(); i++) {
			std::cout << "--> Drift " << i << " : " << bcombo->at(i) << std::endl;
		}
	}

	// Loop over all hits Below
	int driftcount = 0;
	for (int i = 0; i < fHits_Below_Type->size(); i++) {
		int hittype = fHits_Below_Type->at(i);

		if (!IsValidHit(hittype, kBelowX, true)) continue;

		if (debugger) {
			std::cout << "Below Hit Types X " << hittype << std::endl;
		}
		if (IsRPCHit(hittype)) {
			(*fHits_Below_Combo)[i] = false;
		} else {
			(*fHits_Below_Combo)[i] = (*bcombo)[driftcount];
			driftcount++;
		}
	}
	if (debugger) {

		std::cout << "Below Full Combo X" << std::endl;
		for (int i = 0; i < fHits_Below_Type->size(); i++) {
			int hittype = fHits_Below_Type->at(i);
			if (IsRPCHit(hittype)) {
				std::cout << "--> RPC " << i << " : " << hittype << " " << (*fHits_Below_Combo)[i] << std::endl;
			} else {
				std::cout << "--> Drift " << i << " : " << hittype << " " << (*fHits_Below_Combo)[i] << std::endl;
			}
			// if (hittype == 4 and (*fHits_Below_Combo)[i]) sleep(2);
		}
	}

}

void BristolPoCAFitter::SetBelowComboY(std::vector<bool>* bcombo) {

	// Make if needed
	if (!fHits_Below_Combo) {
		fHits_Below_Combo = new std::vector<bool>(fHits_Below_Type->size(), false);
		fHits_Below_Combo->resize( fHits_Below_Type->size() );
	}

	if (debugger) {

		std::cout << "Below Setting Combo Y " << std::endl;
		for (int i = 0; i < bcombo->size(); i++) {
			std::cout << "--> Drift " << i << " : " << bcombo->at(i) << std::endl;
		}
	}

	// Loop over all hits Below
	int driftcount = 0;
	for (int i = 0; i < fHits_Below_Type->size(); i++) {
		int hittype = fHits_Below_Type->at(i);

		if (!IsValidHit(hittype, kBelowY, true)) continue;
		if (debugger) {
			std::cout << "Below Hit Types Y " << hittype << std::endl;
		}

		if (IsRPCHit(hittype)) {
			(*fHits_Below_Combo)[i] = false;
		} else {
			(*fHits_Below_Combo)[i] = (*bcombo)[driftcount];
			driftcount++;
		}
	}

	if (debugger) {
		std::cout << "Below Full Combo Y" << std::endl;
		for (int i = 0; i < fHits_Below_Type->size(); i++) {
			int hittype = fHits_Below_Type->at(i);
			if (IsRPCHit(hittype)) {
				std::cout << "--> RPC " << i << " : " << hittype << " " << (*fHits_Below_Combo)[i] << std::endl;
			} else {
				std::cout << "--> Drift " << i << " : " << hittype << " " << (*fHits_Below_Combo)[i] << std::endl;
			}
			// if (hittype == 4 and (*fHits_Below_Combo)[i]) sleep(2);
		}
	}

}

void BristolPoCAFitter::PreProcessData() {

	if (fHits_Below_Combo) {
		fHits_Below_Combo->clear();
		delete fHits_Below_Combo;
	}

	if (fHits_Above_Combo) {
		fHits_Above_Combo->clear();
		delete fHits_Above_Combo;
	}

	fHits_Above_Combo = new std::vector<bool>();
	fHits_Above_Combo->resize( fHits_Above_Type->size() );

	fHits_Below_Combo = new std::vector<bool>();
	fHits_Below_Combo->resize( fHits_Below_Type->size() );
}


void BristolPoCAFitter::PrintCombos() {

	for (int i = 0; i < fHits_Above_Type->size(); i++) {
		int hittype = fHits_Above_Type->at(i);
		double reco = fHits_Above_Reco->at(i);
		if ((*fHits_Above_Combo)[i]) reco = fHits_Above_Ghost->at(i);
		if (IsRPCHit(hittype)) {
			std::cout << "--> ARPC " << i << " : " << hittype << " " << reco << " " << (*fHits_Above_Combo)[i] << std::endl;
		} else {
			std::cout << "--> ADrift " << i << " : " << hittype << " " << reco << " " << (*fHits_Above_Combo)[i] << std::endl;
		}
	}

	for (int i = 0; i < fHits_Below_Type->size(); i++) {
		int hittype = fHits_Below_Type->at(i);
		double reco = fHits_Below_Reco->at(i);
		if ((*fHits_Below_Combo)[i]) reco = fHits_Below_Ghost->at(i);
		if (IsRPCHit(hittype)) {
			std::cout << "--> BRPC " << i << " : " << hittype << " " << reco << " " << (*fHits_Below_Combo)[i] << std::endl;
		} else {
			std::cout << "--> BDrift " << i << " : " << hittype << " " << reco << " " << (*fHits_Below_Combo)[i] << std::endl;
		}
	}
}

}









