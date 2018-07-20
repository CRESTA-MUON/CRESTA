#ifndef __TrueMCReader_HH__
#define __TrueMCReader_HH__

#include "db/ROOTHeaders.hh"

class TrueMCReader {
public:

  TrueMCReader();
  ~TrueMCReader();
  void ReadInputTTree(TTree* t, std::string prefixa, std::string prefixb);

  double GetTrueMass();
  double GetTrueEnergy();
  double GetTrueP();
  double GetTruePX();
  double GetTruePY();
  double GetTruePZ();
  double GetTrueV();
  double GetScatterAngle();
  bool GetPassFlag();

  double det_above_trueentrance_t;
  double det_above_trueentrance_px;
  double det_above_trueentrance_py;
  double det_above_trueentrance_pz;
  double det_above_trueentrance_x;
  double det_above_trueentrance_y;
  double det_above_trueentrance_z;
  int det_above_trueentrance_pdg;

  double det_above_trueexit_t;
  double det_above_trueexit_px;
  double det_above_trueexit_py;
  double det_above_trueexit_pz;
  double det_above_trueexit_x;
  double det_above_trueexit_y;
  double det_above_trueexit_z;
  int det_above_trueexit_pdg;

  double det_below_trueentrance_t;
  double det_below_trueentrance_px;
  double det_below_trueentrance_py;
  double det_below_trueentrance_pz;
  double det_below_trueentrance_x;
  double det_below_trueentrance_y;
  double det_below_trueentrance_z;
  int det_below_trueentrance_pdg;

  double det_below_trueexit_t;
  double det_below_trueexit_px;
  double det_below_trueexit_py;
  double det_below_trueexit_pz;
  double det_below_trueexit_x;
  double det_below_trueexit_y;
  double det_below_trueexit_z;
  int det_below_trueexit_pdg;

};

#endif
