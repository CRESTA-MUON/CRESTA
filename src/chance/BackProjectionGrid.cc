#include "BackProjectionGrid.hh"


BackProjectionVoxel::BackProjectionVoxel() {
  contents = new std::vector<float>();
}

BackProjectionVoxel::~BackProjectionVoxel() {
}

void BackProjectionVoxel::Fill(float disc, float weight) {
  if (disc == 0.0 || weight == 0.0) {
    return;
  }
  contents->push_back(disc);

}

float BackProjectionVoxel::GetQuantile(float fraction) {

  if (contents->size() < 2) return 0.0;
  std::sort(contents->begin(), contents->end());
  return (*contents)[contents->size() * fraction];
}

void BackProjectionVoxel::UpdateVoxelValue(){
  value25 = GetQuantile(0.25);
  value50 = GetQuantile(0.50);
  value75 = GetQuantile(0.75);
  contents->clear();
  delete contents;
}



BackProjectionGrid::BackProjectionGrid(DBTable table) {
  // TODO
}

BackProjectionGrid::BackProjectionGrid(float sizx, float minx, float maxx, float sizy, float miny, float maxy, float sizz, float minz, float maxz) {
  fXSIZ = sizx;
  fYSIZ = sizy;
  fZSIZ = sizz;
  fXMIN = minx;
  fYMIN = miny;
  fZMIN = minz;
  fXMAX = maxx;
  fYMAX = maxy;
  fZMAX = maxz;
  fXnbins = (fXMAX - fXMIN) / fXSIZ;
  fYnbins = (fYMAX - fYMIN) / fYSIZ;
  fZnbins = (fZMAX - fZMIN) / fZSIZ;
  maxdisc = -1.0;
  mindisc = -1.0;
  fXOFF = 0.0;
  fYOFF = 0.0;
  fZOFF = 0.0;
}

void BackProjectionGrid::SetGridOffsets(float xoff, float yoff, float zoff) {
  fXMIN = fXMIN - fXOFF;
  fYMIN = fYMIN - fYOFF;
  fZMIN = fZMIN - fZOFF;
  fXMAX = fXMAX - fXOFF;
  fYMAX = fYMAX - fYOFF;
  fZMAX = fZMAX - fZOFF;

  fXOFF = xoff;
  fYOFF = yoff;
  fZOFF = zoff;

  fXMIN = fXMIN + fXOFF;
  fYMIN = fYMIN + fYOFF;
  fZMIN = fZMIN + fZOFF;
  fXMAX = fXMAX + fXOFF;
  fYMAX = fYMAX + fYOFF;
  fZMAX = fZMAX + fZOFF;
}

void BackProjectionGrid::SetPointOffsets(float xoff, float yoff, float zoff) {
  fXPFF = xoff;
  fYPFF = yoff;
  fZPFF = zoff;
}


BackProjectionGrid::~BackProjectionGrid() {
};

int BackProjectionGrid::GetVoxelID(float x, float y, float z, bool withoutoff) {

  
  if (!withoutoff) {
    x = x + fXPFF;
    y = y + fYPFF;
    z = z + fZPFF;
  }

  if (x > fXMAX || x < fXMIN) return -1;
  if (y > fYMAX || y < fYMIN) return -1;
  if (z > fZMAX || z < fZMIN) return -1;

  int xi = (x - fXMIN) / fXSIZ;
  int yi = (y - fYMIN) / fYSIZ;
  int zi = (z - fZMIN) / fZSIZ;

  return xi + yi * fXnbins + zi * fXnbins * fYnbins;

}

bool BackProjectionGrid::IsValidVertex(EventTracks & vertex) {
  return (GetVoxelID(vertex.x, vertex.y, vertex.z) >= 0);
}

void BackProjectionGrid::AddDiscriminatorsToVoxel(uint voxelid, float s1, float s2) {

  if (fVoxelHistograms.find(voxelid) == fVoxelHistograms.end()) {
    fVoxelHistograms[voxelid] = new BackProjectionVoxel();
  }
  fVoxelHistograms[voxelid]->Fill(s1);
  fVoxelHistograms[voxelid]->Fill(s2);
}

void BackProjectionGrid::AddVertexToValidVoxels(EventTracks vertex) {

  float xpos = vertex.x;
  float ypos = vertex.y;
  float zpos = vertex.z;

  float s1 = vertex.thx * vertex.mom / 3E3;
  float s2 = vertex.thy * vertex.mom / 3E3;

  if (s1 == 0 || s2 == 0) {
    std::cout << "Disc is 0 " << vertex.thx << " " << vertex.thy << " " << vertex.mom / 3E3 << std::endl;
  }

  std::vector<uint> filledids;

  int voxelid = GetVoxelID(xpos, ypos, zpos);
  if (voxelid < 0) return;

  if (voxelid > fLowVoxelLimit and voxelid < fHighVoxelLimit) {
    filledids.push_back(voxelid);
    AddDiscriminatorsToVoxel(voxelid, s1, s2);
  }

  // Skip forwards
  float stepdistance = 0.3 * fZSIZ;

  while (voxelid > 0) {
    xpos = xpos + stepdistance * vertex.px1;
    ypos = ypos + stepdistance * vertex.py1;
    zpos = zpos + stepdistance;
    voxelid = GetVoxelID(xpos, ypos, zpos);
    if (voxelid < 0) break;
    if (voxelid < fLowVoxelLimit or voxelid > fHighVoxelLimit) continue;
    if (std::find(filledids.begin(), filledids.end(), voxelid) != filledids.end()) continue;

    float distance = (xpos - vertex.x) * (xpos - vertex.x) + (ypos - vertex.y) * (ypos - vertex.y) + (zpos - vertex.z) * (zpos - vertex.z);
    float weight = 1.0;

    filledids.push_back(voxelid);
    AddDiscriminatorsToVoxel(voxelid, s1 * weight, s2 * weight);
  }

  // Reset and skip backwards
  xpos = vertex.x;
  ypos = vertex.y;
  zpos = vertex.z;
  voxelid = GetVoxelID(xpos, ypos, zpos);
  while (voxelid > 0) {
    xpos = xpos - stepdistance * vertex.px2;
    ypos = ypos - stepdistance * vertex.py2;
    zpos = zpos - stepdistance;
    voxelid = GetVoxelID(xpos, ypos, zpos);
    if (voxelid < 0) break;
    if (voxelid < fLowVoxelLimit or voxelid > fHighVoxelLimit) continue;
    if (std::find(filledids.begin(), filledids.end(), voxelid) != filledids.end()) continue;

    filledids.push_back(voxelid);

    float distance = (xpos - vertex.x) * (xpos - vertex.x) + (ypos - vertex.y) * (ypos - vertex.y) + (zpos - vertex.z) * (zpos - vertex.z);
    float weight = 1.0;

    AddDiscriminatorsToVoxel(voxelid, s1 * weight, s2 * weight);
  }

  filledids.clear();
}

void BackProjectionGrid::UpdateValidVoxelChunks() {
  std::cout << "Updating Voxel Chunks" << std::endl;
  uint maxvoxel = fXnbins + fYnbins * fXnbins + fXnbins * fXnbins * fYnbins;
  for (uint voxelid = fLowVoxelLimit; voxelid < fHighVoxelLimit && voxelid < maxvoxel; voxelid++) {
    if (fVoxelHistograms.find(voxelid) != fVoxelHistograms.end()) {
      fVoxelHistograms[voxelid]->UpdateVoxelValue();
    }
  }
}

void BackProjectionGrid::SetVoxelChunk(int ichunk, int totchunk) {
  uint maxvoxel = fXnbins + fYnbins * fXnbins + fXnbins * fXnbins * fYnbins;
  fLowVoxelLimit = int(maxvoxel * (float(ichunk) / float(totchunk)));
  fHighVoxelLimit = int(maxvoxel * (float(ichunk + 1) / float(totchunk)));
  std::cout << "Setting voxel processing chunk : " << fLowVoxelLimit << " vs " << fHighVoxelLimit << std::endl;
}

void BackProjectionGrid::Write() {

  // Now loop over all voxels and find the median of each distribution.
  TH3F* hist25 = new TH3F("hist25", "hist25", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3F* hist50 = new TH3F("hist50", "hist50", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3F* hist75 = new TH3F("hist75", "hist75", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3I* histNS = new TH3I("histNS", "histNS", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);

  int totalbins = histNS->GetNbinsX() * histNS->GetNbinsY() * histNS->GetNbinsZ();
  int printbins = totalbins / 100;
  int curntbins = 0;

  std::cout << "Max and Min Discriminators " << mindisc << " -> " << maxdisc << std::endl;


  for (int i = 0; i < histNS->GetNbinsX(); i++) {
    double x = histNS->GetXaxis()->GetBinCenter(i + 1);
    for (int j = 0; j < histNS->GetNbinsY(); j++) {
      double y = histNS->GetYaxis()->GetBinCenter(j + 1);
      for (int k = 0; k < histNS->GetNbinsZ(); k++) {
        double z = histNS->GetZaxis()->GetBinCenter(k + 1);

        int voxelid = GetVoxelID(x, y, z, true);
        curntbins++;

        if (curntbins % printbins == 0) std::cout << "Processed " << curntbins << "/" << totalbins << " voxels. " << std::endl;

        if (voxelid == -1) continue;

        if (fVoxelHistograms.find(voxelid) == fVoxelHistograms.end()) continue;
        BackProjectionVoxel* voxel = fVoxelHistograms[voxelid];
        float disc25 = voxel->value25;
        float disc50 = voxel->value25;
        float disc75 = voxel->value25;

        hist25->SetBinContent(i + 1, j + 1, k + 1, disc25);
        hist50->SetBinContent(i + 1, j + 1, k + 1, disc50);
        hist75->SetBinContent(i + 1, j + 1, k + 1, disc75);

        // Remove voxel
        fVoxelDiscriminators[voxelid].clear();
      }
    }
  }

  hist25->Write();
  hist50->Write();
  hist75->Write();

  delete hist25;
  delete hist50;
  delete hist75;



}
