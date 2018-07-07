#include "chance/TrueMCReader.hh"
#include "db/ROOTHeaders.hh"

TrueMCReader::TrueMCReader() {
}

TrueMCReader::~TrueMCReader() {
}

void TrueMCReader::ReadInputTTree(TTree* t, std::string prefixa, std::string prefixb) {

  // t->SetBranchAddress((prefixa + "_trueentrance_t").c_str(),  &det_above_trueentrance_t);
  // t->SetBranchAddress((prefixa + "_trueentrance_px").c_str(), &det_above_trueentrance_px);
  // t->SetBranchAddress((prefixa + "_trueentrance_py").c_str(), &det_above_trueentrance_py);
  // t->SetBranchAddress((prefixa + "_trueentrance_pz").c_str(), &det_above_trueentrance_pz);
  // t->SetBranchAddress((prefixa + "_trueentrance_x").c_str(),  &det_above_trueentrance_x);
  // t->SetBranchAddress((prefixa + "_trueentrance_y").c_str(),  &det_above_trueentrance_y);
  // t->SetBranchAddress((prefixa + "_trueentrance_z").c_str(),  &det_above_trueentrance_z);
  // t->SetBranchAddress((prefixa + "_trueentrance_pdg").c_str(),  &det_above_trueentrance_pdg);

  // t->SetBranchAddress((prefixa + "_trueexit_t").c_str(),  &det_above_trueexit_t);
  t->SetBranchAddress((prefixa + "_trueexit_px").c_str(), &det_above_trueexit_px);
  t->SetBranchAddress((prefixa + "_trueexit_py").c_str(), &det_above_trueexit_py);
  t->SetBranchAddress((prefixa + "_trueexit_pz").c_str(), &det_above_trueexit_pz);
  // t->SetBranchAddress((prefixa + "_trueexit_x").c_str(),  &det_above_trueexit_x);
  // t->SetBranchAddress((prefixa + "_trueexit_y").c_str(),  &det_above_trueexit_y);
  // t->SetBranchAddress((prefixa + "_trueexit_z").c_str(),  &det_above_trueexit_z);
  // t->SetBranchAddress((prefixa + "_trueexit_pdg").c_str(),  &det_above_trueexit_pdg);

  // t->SetBranchAddress((prefixb + "_trueentrance_t").c_str(),  &det_below_trueentrance_t);
  t->SetBranchAddress((prefixb + "_trueentrance_px").c_str(), &det_below_trueentrance_px);
  t->SetBranchAddress((prefixb + "_trueentrance_py").c_str(), &det_below_trueentrance_py);
  t->SetBranchAddress((prefixb + "_trueentrance_pz").c_str(), &det_below_trueentrance_pz);
  // t->SetBranchAddress((prefixb + "_trueentrance_x").c_str(),  &det_below_trueentrance_x);
  // t->SetBranchAddress((prefixb + "_trueentrance_y").c_str(),  &det_below_trueentrance_y);
  // t->SetBranchAddress((prefixb + "_trueentrance_z").c_str(),  &det_below_trueentrance_z);
  // t->SetBranchAddress((prefixb + "_trueentrance_pdg").c_str(),  &det_below_trueentrance_pdg);

  // t->SetBranchAddress((prefixb + "_trueexit_t").c_str(),  &det_below_trueexit_t);
  // t->SetBranchAddress((prefixb + "_trueexit_px").c_str(), &det_below_trueexit_px);
  // t->SetBranchAddress((prefixb + "_trueexit_py").c_str(), &det_below_trueexit_py);
  // t->SetBranchAddress((prefixb + "_trueexit_pz").c_str(), &det_below_trueexit_pz);
  // t->SetBranchAddress((prefixb + "_trueexit_x").c_str(),  &det_below_trueexit_x);
  // t->SetBranchAddress((prefixb + "_trueexit_y").c_str(),  &det_below_trueexit_y);
  // t->SetBranchAddress((prefixb + "_trueexit_z").c_str(),  &det_below_trueexit_z);
  // t->SetBranchAddress((prefixb + "_trueexit_pdg").c_str(),  &det_below_trueexit_pdg);

}
double TrueMCReader::GetTrueMass(){
  return 105.683745;
}
double TrueMCReader::GetTrueEnergy(){
  double p = GetTrueP();
  double m = GetTrueMass();
  return sqrt(p*p + m*m);
}

double TrueMCReader::GetTrueP(){
  return sqrt(GetTruePX()*GetTruePX() + GetTruePY()*GetTruePY() + GetTruePZ()*GetTruePZ());
}

double TrueMCReader::GetTruePX(){
  return det_above_trueexit_px;
}

double TrueMCReader::GetTruePY(){
  return det_above_trueexit_py;
}

double TrueMCReader::GetTruePZ(){
  return det_above_trueexit_pz;
}

double TrueMCReader::GetTrueV(){
  return GetTrueP() / GetTrueMass();
}

double TrueMCReader::GetScatterAngle(){

  TVector3 vec1 = TVector3(det_above_trueexit_px, det_above_trueexit_py, det_above_trueexit_pz);
  TVector3 vec2 = TVector3(det_below_trueentrance_px, det_below_trueentrance_py, det_below_trueentrance_pz);
  return vec1.Angle(vec2);
}

bool TrueMCReader::GetPassFlag(){
  return true;
}
