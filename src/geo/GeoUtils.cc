#include "GeoUtils.hh"

namespace COSMIC {
namespace GEO {

G4VSolid* GetSolidFromStore(std::string name, bool verbose) {
  return G4SolidStore::GetInstance()->GetSolid(name, verbose);
}

G4Box* GetBoxFromStore(std::string name, bool verbose) {
  G4VSolid* solid = G4SolidStore::GetInstance()->GetSolid(name, verbose);
  G4Box* box = static_cast<G4Box*>(solid);
  return box;
}

G4LogicalVolume* GetLogicalFromStore(std::string name, bool verbose) {
  return G4LogicalVolumeStore::GetInstance()->GetVolume(name, verbose);
}

G4LogicalVolume* GetMotherLogicalFromStore(DBTable tb, bool verbose) {
  if (!tb.Has("mother")) return 0;
  else return GetLogicalFromStore(tb.GetS("mother"), verbose);
}


void BuildMCMapFromCurrentGEO() {
  std::cout << "[GEO ]: Building mm scale MC Map" << std::endl;

  // Build a new navigation history
  DBTable configuration = DB::Get()->GetTable("DISCRIMINATOR", "config");

  double grid_sizex = configuration.GetD("grid_sizex");
  double xmin = configuration.GetD("grid_minx");
  double xmax = configuration.GetD("grid_maxx");
  int nbinsx = ((xmax - xmin) / grid_sizex);

  double grid_sizey = configuration.GetD("grid_sizey");
  double ymin = configuration.GetD("grid_miny");
  double ymax = configuration.GetD("grid_maxy");
  int nbinsy = ((ymax - ymin) / grid_sizey);

  double grid_sizez = configuration.GetD("grid_sizez");
  double zmin = configuration.GetD("grid_minz");
  double zmax = configuration.GetD("grid_maxz");
  int nbinsz = ((zmax - zmin) / grid_sizez);

  int nthrows = 100;
  if (configuration.Has("nthrows")) nthrows = configuration.GetI("nthrows");
  std::cout << "--> Considering " << nthrows << " random throws in each voxel." << std::endl;

  TH3D* density_hist = new TH3D("density_hist", "Voxel Density (g/cm3)", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* atomspv_hist = new TH3D("atomspv_hist", "Voxel Atoms Per Volume", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* elecspv_hist = new TH3D("elecspv_hist", "Voxel Electrons Per Volume", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* elecden_hist = new TH3D("elecden_hist", "Voxel Electron Density", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* radlens_hist = new TH3D("radlens_hist", "Voxel Radiation Length (mm)", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* nuclens_hist = new TH3D("nuclens_hist", "Voxel Nuclear Length (mm)", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);

  TH3D* density_ravg = new TH3D("density_ravg", "Rand. Avg. Density (g/cm3)", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* atomspv_ravg = new TH3D("atomspv_ravg", "Rand. Avg. Atoms Per Volume", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* elecspv_ravg = new TH3D("elecspv_ravg", "Rand. Avg. Electrons Per Volume", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* elecden_ravg = new TH3D("elecden_ravg", "Rand. Avg. Electron Density", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* radlens_ravg = new TH3D("radlens_ravg", "Rand. Avg. Radiation Length (mm)", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* nuclens_ravg = new TH3D("nuclens_ravg", "Rand. Avg. Nuclear Length (mm)", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);
  TH3D* counter_ravg = new TH3D("counter_ravg", "Rand. Avg. Counter", nbinsx, xmin, xmax, nbinsy, ymin, ymax, nbinsz, zmin, zmax);

  G4Navigator* nav =  G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  for (int i = 0; i < nbinsx; i++) {
    if (i % 5 == 0) std::cout << "--> Processed " << i << "/" << nbinsx << " 2D YZ slices." << std::endl;
    for (int j = 0; j < nbinsy; j++) {
      for (int k = 0; k < nbinsz; k++) {

        double x = xmin + (i + 0.5) * grid_sizex;
        double y = ymin + (j + 0.5) * grid_sizey;
        double z = zmin + (k + 0.5) * grid_sizez;

        G4VPhysicalVolume* phys = nav->LocateGlobalPointAndSetup(G4ThreeVector(x * mm, y * mm, z * mm));
        if (!phys) continue;

        G4LogicalVolume* log = phys->GetLogicalVolume();
        if (!log) continue;

        G4Material* mat = log->GetMaterial();
        if (!mat) continue;

        G4double density = mat->GetDensity() / (g / cm3);

        density_hist->SetBinContent(i, j, k, density);
        atomspv_hist->SetBinContent(i, j, k, mat->GetTotNbOfAtomsPerVolume());
        elecspv_hist->SetBinContent(i, j, k, mat->GetTotNbOfElectPerVolume());
        elecden_hist->SetBinContent(i, j, k, mat->GetElectronDensity());
        radlens_hist->SetBinContent(i, j, k, mat->GetRadlen());
        nuclens_hist->SetBinContent(i, j, k, mat->GetNuclearInterLength());

        for (int t = 0; t < nthrows; t++) {
          x = xmin + (i + G4UniformRand()) * grid_sizex;
          y = ymin + (j + G4UniformRand()) * grid_sizey;
          z = zmin + (k + G4UniformRand()) * grid_sizez;

          G4VPhysicalVolume* phys = nav->LocateGlobalPointAndSetup(G4ThreeVector(x * mm, y * mm, z * mm));
          if (!phys) continue;

          G4LogicalVolume* log = phys->GetLogicalVolume();
          if (!log) continue;

          G4Material* mat = log->GetMaterial();
          if (!mat) continue;

          G4double density = mat->GetDensity() / (g / cm3);

          density_ravg->SetBinContent(i, j, k, density_ravg->GetBinContent(i,j,k) + density);
          atomspv_ravg->SetBinContent(i, j, k, atomspv_ravg->GetBinContent(i,j,k) + mat->GetTotNbOfAtomsPerVolume());
          elecspv_ravg->SetBinContent(i, j, k, elecspv_ravg->GetBinContent(i,j,k) + mat->GetTotNbOfElectPerVolume());
          elecden_ravg->SetBinContent(i, j, k, elecden_ravg->GetBinContent(i,j,k) + mat->GetElectronDensity());
          radlens_ravg->SetBinContent(i, j, k, radlens_ravg->GetBinContent(i,j,k) + mat->GetRadlen());
          nuclens_ravg->SetBinContent(i, j, k, nuclens_ravg->GetBinContent(i,j,k) + mat->GetNuclearInterLength());
          counter_ravg->SetBinContent(i, j, k, counter_ravg->GetBinContent(i,j,k) + 1);
        }
      }
    }
  }
  std::cout << "--> Scan Complete! Writing histograms." << std::endl;

  // Write the voxel histograms
  density_hist->Write();
  atomspv_hist->Write();
  elecspv_hist->Write();
  elecden_hist->Write();
  radlens_hist->Write();
  nuclens_hist->Write();

  density_ravg->Divide(counter_ravg);
  atomspv_ravg->Divide(counter_ravg);
  elecspv_ravg->Divide(counter_ravg);
  elecden_ravg->Divide(counter_ravg);
  radlens_ravg->Divide(counter_ravg);
  nuclens_ravg->Divide(counter_ravg);

  density_ravg->Write();
  atomspv_ravg->Write();
  elecspv_ravg->Write();
  elecden_ravg->Write();
  radlens_ravg->Write();
  nuclens_ravg->Write();
  std::cout << "--> Complete!" << std::endl;

  return;
}



} // - namespace GEO
} // - namespace COSMIC
