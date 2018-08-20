#include "VertexGrid.hh"

static struct DescendingTrackSortWithP {
  Bool_t operator() (EventVertex* t1, EventVertex* t2) {return ( t1->th * (t1->mom / 3000.) > t2->th * (t2->mom / 3000.) );}
} TrackCompareWithMom;



VertexGrid::VertexGrid(DBTable table) {
  // TODO
}

VertexGrid::VertexGrid(float sizx, float minx, float maxx, float sizy, float miny, float maxy, float sizz, float minz, float maxz) {
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
  fTrackCut = -1;
  fDiscrCut = 0.0001;
  fXOFF = 0.0;
  fYOFF = 0.0;
  fZOFF = 0.0;

}

void VertexGrid::SetGridOffsets(float xoff, float yoff, float zoff) {
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

void VertexGrid::SetPointOffsets(float xoff, float yoff, float zoff) {
  fXPFF = xoff;
  fYPFF = yoff;
  fZPFF = zoff;
}


VertexGrid::~VertexGrid() {
  fVertices.clear();
};

int VertexGrid::GetVoxelID(float x, float y, float z, bool withoutoff) {

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

bool VertexGrid::IsValidVertex(EventVertex& vertex) {
  return (GetVoxelID(vertex.x, vertex.y, vertex.z) >= 0);
}

void VertexGrid::AddVertexToVoxel(uint voxelid, EventVertex vertex) {
  // Check if already inside the map
  if (fVertices.find(voxelid) != fVertices.end()) {
    fVertices[voxelid].push_back( vertex );
  } else {
    fVertices[voxelid] = std::vector<EventVertex>();
    fVertices[voxelid].push_back( vertex );
  }
}

void VertexGrid::AddVertex(EventVertex vertex) {
  int voxelid = GetVoxelID(vertex.x, vertex.y, vertex.z);
  if (voxelid < 0) return;
  AddVertexToVoxel(voxelid, vertex);
}

void VertexGrid::Write() {

  TH3F* hist25 = new TH3F("hist25", "hist25", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3F* hist50 = new TH3F("hist50", "hist50", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3F* hist75 = new TH3F("hist75", "hist75", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3I* histNS = new TH3I("histNS", "histNS", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3F* histBS = new TH3F("histBS", "histBS", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);
  TH3F* histPC = new TH3F("histPC", "histPC", fXnbins, fXMIN, fXMAX, fYnbins, fYMIN, fYMAX, fZnbins, fZMIN, fZMAX);

  int totalbins = histNS->GetNbinsX() * histNS->GetNbinsY() * histNS->GetNbinsZ();
  int printbins = totalbins / 100;
  int curntbins = 0;


  TTree* tree = new TTree("T", "T");
  double ClusterInfo[12];
  tree->Branch("clusters", &ClusterInfo, "cx/D:cy:cz:tracks:discr:isextremal:asr025:asr050:asr075:poca:x:y:z");

  // Get the average track count per bin
  long int avgcount = 0;
  long int bincount = 0;

  for (int i = 0; i < histNS->GetNbinsX(); i++) {
    double x = histNS->GetXaxis()->GetBinCenter(i + 1);
    for (int j = 0; j < histNS->GetNbinsY(); j++) {
      double y = histNS->GetYaxis()->GetBinCenter(j + 1);
      for (int k = 0; k < histNS->GetNbinsZ(); k++) {
        double z = histNS->GetZaxis()->GetBinCenter(k + 1);
        int voxelid = GetVoxelID(x, y, z, true);
        if (voxelid == -1) continue;
        if (fVertices.find(voxelid) == fVertices.end()) {
          avgcount += 1;
          bincount += 1;
        } else {
          avgcount += fVertices[voxelid].size();
          bincount += 1;
        }
      }
    }
  }
  std::cout << "Track Cut Setting : " << fTrackCut << std::endl;

  if (fTrackCut < 0) {
    fTrackCut = (avgcount) * 1.5 / (bincount);
    if (fTrackCut < 5) fTrackCut = 5;
    std::cout << "Setting min tracks for BS to " << fTrackCut << " (auto)." << std::endl;
  } else {
    std::cout << "Setting min tracks for BS to " << fTrackCut << std::endl;
  }


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
        if (fVertices.find(voxelid) == fVertices.end()) continue;

        std::vector<EventVertex> vertices = fVertices[voxelid];
        if (vertices.size() < 2) continue;

        ClusterInfo[0] = i;
        ClusterInfo[1] = j;
        ClusterInfo[2] = k;

        ClusterInfo[10] = x;
        ClusterInfo[11] = y;
        ClusterInfo[12] = z;

        ClusterInfo[3] = vertices.size();
        histNS->SetBinContent(i + 1, j + 1, k + 1, vertices.size());

        ClusterInfo[5] = 0.0; // Is Extremal?

        // Bristol Algorithm
        int nmintracks = fTrackCut;
        double discrim = 0.0;
        std::vector<double> disc;
        bool discard = false;

        if (vertices.size() >= nmintracks) {

          std::vector<EventVertex*> tempvertices;
          for (int q = 0; q < vertices.size(); q++) {
            tempvertices.push_back(&vertices[q]);
          }

          // Sort tracks with comparison
          std::sort(tempvertices.begin(), tempvertices.end(), TrackCompareWithMom);

          for (int q = 0; q < tempvertices.size() && q < nmintracks; q++) {
            EventVertex v1 = (*tempvertices[q]);
            TVector3 theV(v1.x, v1.y, v1.z);

            for (int p = 0; p < tempvertices.size() && p < nmintracks; p++) {
              if (p <= q) continue;
              EventVertex v2 = (*tempvertices[p]);

              TVector3 theOtherV(v2.x, v2.y, v2.z);
              theOtherV -= theV;

              float wmetric = theOtherV.Mag();
              float discr1 = v1.th * v1.mom / 3.E3;
              float discr2 = v2.th * v2.mom / 3.E3;
              float discr_cutoff = fDiscrCut;
              float discrprod = discr1 * discr2;

              if (discr1 < discr_cutoff || discr2 < discr_cutoff) {
                discard = false;
                continue;
              } else {
                wmetric /= discrprod;
                disc.push_back(log(wmetric));
              }
            }
          }

          if (!discard) {
            std::sort(disc.begin(), disc.end());
            discrim = disc[disc.size() / 2];
          }
        }

        ClusterInfo[4] = discrim;
        histBS->SetBinContent(i + 1, j + 1, k + 1, discrim);

        // Angular Statistics Algorithm
        disc.clear();
        for (int q = 0; q < vertices.size(); q++) {
          disc.push_back(vertices[q].thx * vertices[q].mom);
          disc.push_back(vertices[q].thy * vertices[q].mom);
        }
        std::sort(disc.begin(), disc.end());

        ClusterInfo[6] =  disc[0.25 * disc.size()];
        hist25->SetBinContent(i + 1, j + 1, k + 1, ClusterInfo[6]);

        ClusterInfo[7] =  disc[0.50 * disc.size()];
        hist50->SetBinContent(i + 1, j + 1, k + 1, ClusterInfo[7]);

        ClusterInfo[8] =  disc[0.75 * disc.size()];
        hist75->SetBinContent(i + 1, j + 1, k + 1, ClusterInfo[8]);

        // Point of closest approach algorithm (average angle)
        double angle = 0.0;
        for (int q = 0; q < vertices.size(); q++) {
          angle += vertices[q].th;
        }
        angle /= double(vertices.size());

        ClusterInfo[9] = angle;
        histPC->SetBinContent(i + 1, j + 1, k + 1, angle);

        // Fill outputs
        tree->Fill();

        // Remove voxel
        fVertices[voxelid].clear();
      }
    }
  }

  tree->Write();
  hist25->Write();
  hist50->Write();
  hist75->Write();
  histPC->Write();
  histNS->Write();
  histBS->Write();

  delete hist25;
  delete hist50;
  delete hist75;
  delete histNS;
  delete histBS;
}
