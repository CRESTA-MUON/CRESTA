#ifndef __BRISTOLPOCA_FITTER_HH__
#define __BRISTOLPOCA_FITTER_HH__


#include "Math/Functor.h"
#include "db/ROOTHeaders.hh"
#include <cassert>
#include "Minuit2/FCNBase.h"
#include "chance/ChannelMapping.hh"

namespace COSMIC {

class MuonHit {
public:
  MuonHit() {};

  MuonHit(double r, double t, double z, double e, double g, int ts) {
    recohit = r;
    truehit = t;
    zpos    = z;
    error   = e;
    ghost   = g;
    type    = ts;
    useghost = false;
    usetrue = false;
  }

  ~MuonHit() {};

  double GetHit() { 
    if (usetrue) return truehit;
    else { return useghost ? ghost : recohit; }
  };
  
double recohit;
  double truehit;
  double zpos;
  double error;
  double ghost;
  int type;
  bool useghost;
  bool usetrue;
};



class BristolPoCAFitter : public ROOT::Minuit2::FCNBase {
public:

	BristolPoCAFitter();
	~BristolPoCAFitter();

	void ReadInputTTree(TTree* t, std::string prefixa = "det_above", std::string prefixb = "det_below");

	double DoEval(const double* x) const;

	bool IsValidHit(int hittype, int hitrequired, bool isbelow = false) const;
	bool IsDriftHit(int hittype) const;
	bool IsRPCHit(int hittype) const;

	// double EvaluateTrackResidual( double pointx, double gradient, double pointz, int hitreq ) const;
	double EvaluateTrackResidual( double pointx, double gradient, double pointz, std::vector<MuonHit*>* xahits ) const;

	double GetAverageHit(int hitreq);
	double GetAverageZ(int hitreq);


	inline void SetUseRPC(bool f)   { fUseRPC = f; };
	inline void SetUseDrift(bool f) { fUseDrift = f; };


	void ApplyVectorOffset(std::vector<double>* vec);
	void ApplyOffsets();



	int GetNHits(int hitreq);

	int GetNAboveRPCX() { return GetNHits(kRPCAboveX); };
	int GetNAboveRPCY() { return GetNHits(kRPCAboveY); };
	int GetNBelowRPCX() { return GetNHits(kRPCBelowX); };
	int GetNBelowRPCY() { return GetNHits(kRPCBelowY); };

	int GetNAboveDriftX() { return GetNHits(kDriftAboveX); };
	int GetNAboveDriftY() { return GetNHits(kDriftAboveY); };
	int GetNBelowDriftX() { return GetNHits(kDriftBelowX); };
	int GetNBelowDriftY() { return GetNHits(kDriftBelowY); };

	void PrintCombos();


	virtual double operator()( const double* x ) const { return DoEval(x); };
	virtual double operator()( const std::vector<double>& par ) const { return DoEval( &par[0] ); };
	virtual double Up() const { return 1.0; };


	void FillComboVect(std::vector<std::vector<bool> >& combomap, int hitreq );
	double DoSingleEval(const double* x, std::vector<MuonHit*>* hits) const;

	double DoSingleTrackFitWithX(int hitreq, double* x = NULL, double* px = NULL);
	std::vector<bool> GetBestComboForDriftHits(int hitreq);
	double EvaluateTrackResidual( double pointx, double gradient, double pointz, int hitreq ) const;

	void PerformDoubleTrackPoCAFit(double* pocafitparams);

	double GetLowestZ(int hitreq);
	double GetHighestZ(int hitreq);
	double GetLowestHit(int hitreq);
	double GetHighestHit(int hitreq);

	void SetAboveComboX(std::vector<bool>* acombo);
	void SetAboveComboY(std::vector<bool>* acombo);
	void SetBelowComboX(std::vector<bool>* bcombo);
	void SetBelowComboY(std::vector<bool>* bcombo);

	void PreProcessData();

	std::vector<double>* fHits_Above_Reco;
	std::vector<double>* fHits_Above_True;
	std::vector<double>* fHits_Above_ZPos;
	std::vector<double>* fHits_Above_Error;
	std::vector<double>* fHits_Above_Ghost;
	std::vector<int>*    fHits_Above_Type;
	std::vector<bool>*   fHits_Above_Combo;

	std::vector<double>* fHits_Below_Reco;
	std::vector<double>* fHits_Below_True;
	std::vector<double>* fHits_Below_ZPos;
	std::vector<double>* fHits_Below_Error;
	std::vector<double>* fHits_Below_Ghost;
	std::vector<int>*    fHits_Below_Type;
	std::vector<bool>*   fHits_Below_Combo;

	std::vector<MuonHit*> fMuonHits_Above;
	std::vector<MuonHit*> fMuonHits_Below;
	mutable std::map<int, std::vector<MuonHit*> > fMuonHits_Map;

	std::vector<MuonHit*>* GetHitCombination(int hitreq) const;

	bool fUseRPC;
	bool fUseDrift;

	// temporary containers to speed things up
	mutable std::vector<MuonHit*>* xahits;
	mutable std::vector<MuonHit*>* yahits;
	mutable std::vector<MuonHit*>* xbhits;
	mutable std::vector<MuonHit*>* ybhits;

};


class CHANCESingleTrackFitterFCN : public ROOT::Minuit2::FCNBase {
public:
	inline CHANCESingleTrackFitterFCN(BristolPoCAFitter* f, int h) : fFitter(f) { fHits = fFitter->GetHitCombination(h);};
	inline double operator() (const double *x) const {
		return DoEval(x);
	}
	inline double operator()(const std::vector<double>& x) const {
		return DoEval(&x[0]);
	}
	inline double Up() const { return 1.0; };
	inline double DoEval(const double *x) const {
		return fFitter->DoSingleEval(x, fHits);
	}
	BristolPoCAFitter* fFitter;
	std::vector<MuonHit*>* fHits;
	int fHitReq;
};


}
#endif












