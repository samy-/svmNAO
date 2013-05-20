/**
 * \file naoKMeans2Bow.c
 * \brief Exécution des algorithmes KMeans et Bag of Words sur un échantillon de données.
 * 
 * \author HADAPTIC 2013
 * \version 0.1
 * \date 10 mars 2013
 *
 * Programme permettant d'effectuer l'algorithme de KMeans et Bag Of Words sur
 * un échantillon de points d'intérêts spatiaux-temporels provenant d'une vidéo.
 * Il faut avoir préalablement créé les fichiers contenant les données des centres de gravités ("training.means")
 * à l'aide du programme naoKMeansTraing.
 * 
 * Les données seront ensuite enregistrées dans le fichier "testing.bow" dans le format
 * analysable par la librairie libSVM.
 *
 */

#include <cstdlib>			// C standard includes
#include <iostream>			// C++ I/O
#include <string.h>			// C++ strings
#include <fstream>
#include "KMlocal.h"			// k-means algorithms

#define LAPTEV_DIMENSION 169 // point-type x y t sigma2 tau2 detector-confidence dscr-hog(72) dscr-hof(90)
#define STIPS_DIMENSION  162 // dscr-hog(72) dscr-hof(90)

using namespace std;		

void getArgs(int argc, char **argv);	// get command-line arguments
void printSummary(			// print final summary
		  const KMlocal&	theAlg,		// the algorithm
		  const KMdata&	dataPts,	// the points
		  KMfilterCenters&	ctrs);		// the centers
bool readPt(				// read a point
	    istream&		in,		// input stream
	    KMpoint&		p);		// point (returned)
void laptevParser(
		  istream& in, const KMdata& d, int *nPts  // number of STIPS
		  );       
void trainingMeansParser(
			 istream& in,
			 const KMfilterCenters& ctrs);
void printPt(				// print a point
	     ostream&		out,		// output stream
	     const KMpoint&	p);		// the point

//----------------------------------------------------------------------
//  Global parameters (some are set in getArgs())
//----------------------------------------------------------------------

int	k		= 100;		// number of centers
int	dim		= 2;		// dimension
int	maxPts		= 1000;		// max number of data points
int	stages		= 100;		// number of stages
istream* dataIn		= NULL;		// input data stream

//----------------------------------------------------------------------
//  Termination conditions
//	These are explained in the file KMterm.h and KMlocal.h.  Unless
//	you are into fine tuning, don't worry about changing these.
//----------------------------------------------------------------------
KMterm	term(100, 0, 0, 0,		// run for 100 stages
		0.10,			// min consec RDL
		0.10,			// min accum RDL
		3,			// max run stages
		0.50,			// init. prob. of acceptance
		10,			// temp. run length
		0.95);			// temp. reduction factor

//----------------------------------------------------------------------
//  Main program
//----------------------------------------------------------------------
int nao-kmeans(int argc, char **argv)
{
  ifstream importMeans; // permettra d'utiliser les centres enregistrés dans l'étape de training
  int nPts; // actual number of points

  getArgs(argc, argv);			// read command-line arguments
  term.setAbsMaxTotStage(stages);	// set number of stages
  
  // I- IMPORT STIPS
  nPts = 0;
  dim = STIPS_DIMENSION;
  KMdata dataPts(dim, maxPts);		// allocate data storage
  if (dataIn != NULL){
    laptevParser(*dataIn, dataPts, &nPts);
  }
  else{
      perror("Pas de données à lire !!!");
      return EXIT_FAILURE;
    }
  dataPts.setNPts(nPts);			// set actual number of pts
  dataPts.buildKcTree();			// build filtering structure
  
  // II- IMPORT TRAINING CENTERS
  KMfilterCenters ctrs(k, dataPts);		// allocate centers
  importMeans.open("training.means", ios::in);
  if(importMeans != NULL){
    trainingMeansParser(importMeans,ctrs);
    importMeans.close();
  }
  else{
    cerr << "Error while importing Means" << endl;
    return 2;
  }
   
  // III- GET ASSIGNMENTS 
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()]; // dataPts = 1 label
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist); 
  
  // IV- BAG OF WORDS STEP
  // initialisation de l'histogramme
  int* bowHistogram = NULL;
  bowHistogram = new int[k];
  for(int centre = 0; centre<k; centre++)
    bowHistogram[centre]=0;
  // remplissage de l'histogramme
  for(int point = 0; point < nPts ; point++){
    bowHistogram[closeCtr[point]]++;
  }
  delete closeCtr;
  delete[] sqDist;
  // exportation dans le fichier "testing.bow" sous un format pouvant être lu par libSVM
  char* KMeansToBow = NULL;
  KMeansToBow = new char[(256+1)];
  sprintf(KMeansToBow,"%s.bow",argv[1]);
  ofstream testingBow(KMeansToBow, ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert
  if(testingBow){
    testingBow << "0"; 
    for(int centre = 0; centre<k ; centre++){
      testingBow << " " << centre + 1 << ":" << bowHistogram[centre];
    }
    testingBow << endl;
    testingBow.close();
  }
  else
    cerr << "Impossible d'ouvrir le fichier !" << endl;
  // affichage
  cout << "Bag Of Words histogram" << endl;
  for(int centre = 0; centre<k; centre++){
  cout << "Centre " << centre << ": ";
    for (int i = 0; i<bowHistogram[centre]; i++)
      cout << "*";
    cout << endl;
  }
  delete[] bowHistogram;
  
  kmExit(0);
}

//----------------------------------------------------------------------
//  getArgs - get command line arguments
//----------------------------------------------------------------------

void getArgs(int argc, char **argv){
  static ifstream dataStream;			// data file stream
  static ifstream queryStream;		// query file stream
  
  if (argc != 2) {				// no arguments
    cerr << "Usage:\n\n"
	 << "naoKMeans2Bow data\n"
	 << endl;
    kmExit(0);
  }
  dataStream.open(argv[1], ios::in);
  if (!dataStream) {
    cerr << "Cannot open data file\n";
    kmExit(1);
  }
  dataIn = &dataStream;
}

//----------------------------------------------------------------------
//  Reading/Printing utilities
//	readPt - read a point from input stream into data storage
//		at position i.  Returns false on error or EOF.
//	printPt - prints a points to output file
//----------------------------------------------------------------------
bool readPt(istream& in, KMpoint& p)
{
  for (int d = 0; d < dim; d++) {
    if(!(in >> p[d])) return false;
  }
  return true;
}

void printPt(ostream& out, const KMpoint& p)
{
  out << "(" << p[0];
  for (int i = 1; i < dim; i++) {
    out << ", " << p[i];
  }
  out << ")\n";
}
/**
 * \fn void laptevParser(istream& in, const KMdata& data, int* nPts)
 * \brief Analyse du fichier contenant les STIPs et créé par le programme de laptev.
 * Les STIPs sont alors importés dans l'objet KMdata.
 *
 * \param [in] in flux entrant correspondant au fichier ;
 * \param [in,out] data référence sur l'objet KMdata où seront enregistrés les STIPs ;
 * \param [in,out] nPts nombre de points (STIPs) enregistrés.
 */
void laptevParser(istream& in, const KMdata& data, int* nPts){
  KMdata laptevData(LAPTEV_DIMENSION,maxPts);
  bool endOfLine = false;
  
  // Enregistrement de l'objet laptevData
  // Enregistre de chaque vecteurs
  while(!(in.eof()) && (*nPts) < maxPts){
    // enregistrement de chaque points
    int d = 0;
    while(!endOfLine && d < LAPTEV_DIMENSION){
      if(!(in >> laptevData[(*nPts)][d])){ // on enregistre le flux dans le buffer et 
	endOfLine = true; // s'il n'y a pas de caractère c'est qu'on a fini la ligne
      }
      d++;
    }
    (*nPts)++;
  }

  
  // Réécriture de l'objet laptevData dans l'objet data en ignorant les 7 premiers paramètres
  for(int p = 0 ; p < (*nPts) ; p++){
    for(int d = 0 ; d < dim ; d++){
      data[p][d] = laptevData[p][d+7];
    }
  }
}
void trainingMeansParser(istream& in, const KMfilterCenters& ctrs){
  bool endOfLine = false;
  
  int center = 0;
  while(!(in.eof()) && center < k){
    // enregistrement de chaque means
    int d = 0;
    while(!endOfLine && d < STIPS_DIMENSION){
      if(!(in >> ctrs[center][d])){
	endOfLine = true;
      }
      d++;
    }
    center++;
  }
}
//------------------------------------------------------------------------
//  Print summary of execution
//------------------------------------------------------------------------
void printSummary(
		  const KMlocal&		theAlg,		// the algorithm
		  const KMdata&		dataPts,	// the points
		  KMfilterCenters&		ctrs)		// the centers
{
  ofstream out;			// output data file stream
  out.open("output3.txt",ios::out);
  if(!out){  // si l'ouverture a échoué
    cerr << "Impossible d'ouvrir le fichier !" << endl;
    exit(0);
  }
  
  out << "Number of stages: " << theAlg.getTotalStages() << "\n";
  out << "Average distortion: " <<
    ctrs.getDist(false)/double(ctrs.getNPts()) << "\n";
  // print final center points
  out << "(Final Center Points:\n";
  ctrs.print();
  out << ")\n";
  // get/print final cluster assignments
  KMctrIdxArray closeCtr = new KMctrIdx[dataPts.getNPts()];
  double* sqDist = new double[dataPts.getNPts()];
  ctrs.getAssignments(closeCtr, sqDist); // obligé d'avoir la distance ?
  
  out	<< "(Cluster assignments:\n"
	<< "Point  Center :\n"
	<< "-----  ------  \n";
  for (int i = 0; i < dataPts.getNPts(); i++) {
    out	<< setw(5) << i
	<< setw(5) << closeCtr[i]
      
	<< "\n";
  }
  out << ")\n";
  delete [] closeCtr;
  delete [] sqDist;
  out.close();  // on ferme le fichier
}
