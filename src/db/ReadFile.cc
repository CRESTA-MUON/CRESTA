// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

// CRESTA Header
#include <db/ReadFile.hh>

// namespace CRESTA
using namespace CRESTA;

int DBUtils::ReadFile(const std::string &filename, std::string &filecontents)
{
  bool bzip2 = true;

  const int nmax = 4096;
  char buffer[nmax];
  int nread = 0;

  bzip2 = false;

  if (!bzip2) {
    // Not BZIP2 format, so just dump the file contents raw into the string
    FILE *f = fopen(filename.c_str(), "r");

    if (f == NULL)
      return -1;

    filecontents = "";
    while ( (nread = fread(buffer, sizeof(char), nmax, f)) )
      filecontents.append(buffer, nread);

    fclose(f);
  }

  return 0;
}

std::vector<std::string> DBUtils::Expand(std::string pattern) {
  glob_t g;
  std::vector<std::string> vecs;
  if (glob(pattern.c_str(), 0, 0, &g) == 0) {
    for (unsigned i = 0; i < g.gl_pathc; i++) {
      std::string path(g.gl_pathv[i]);
      vecs.push_back(path);
    }
  }
  globfree(&g);
  return vecs;
}

std::vector<std::string> DBUtils::ParseToStr(std::string str, const char* del) {
  std::istringstream stream(str);
  std::string temp_string;
  std::vector<std::string> vals;

  while (std::getline(stream >> std::ws, temp_string, *del)) {
    if (temp_string.empty()) continue;
    vals.push_back(temp_string);
  }

  return vals;
}

